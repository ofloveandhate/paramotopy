#include "failed_paths.h"


//the master function for doing stuff to the failed points.  called by paramotopy's main.

void failinfo::MainMenu(ProgSettings & paramotopy_settings, runinfo & paramotopy_info){
	
	//make sure the run was completed.  
	if (!paramotopy_info.test_if_finished()) {
		std::cout << "\n\nthe run for " << paramotopy_info.inputfilename << " appears not completed.  please complete the run.\n";
		return;  //run was not completed, ought not continue.
	}

	//set settings for path failure, based on the step2 settings.


	failinfo::RecoverProgress(paramotopy_info);//gets how far we were

	
	std::cout << "current iteration " << current_iteration << std::endl;
	
	std::stringstream menu;
	menu << "\n\nPath Failure Menu:\n\n"
	<< "1) ReRun Failed Paths\n"
	<< "2) Clear Failed Path Data (start over)\n"
	<< "3) Change path failure settings\n"
	<< "*\n"
	<< "0) Go Back\n"
	<< "\n: ";
	int choice = -1001;
	while (choice!=0) {
		paramotopy_settings.DisplayCurrentSettings("PathFailure");
		choice = get_int_choice(menu.str(),0,7);
		
		switch (choice) {
			case 0:
				std::cout << "exiting path failure\n";
				paramotopy_info.GetPrevRandom();//restore to main random values
				break;
				
			case 1:
				failinfo::PerformAnalysis(paramotopy_settings, paramotopy_info);
				break;
				
			case 2:
				//delete path failure analysis files, start over
				if (get_int_choice("are you sure? 0 no, 1 yes\n: ",0,1)==1) {
					failinfo::StartOver(paramotopy_info);
				}
				break;
				
				
			case 3:
				paramotopy_settings.PathFailureMenu();
				break;
				
			default:
				std::cout << "todo: finish this section\n";
				break;
		}
		paramotopy_info.UpdateAndSave();
	}
	
	

	return;

	
}


void failinfo::StartOver(runinfo & paramotopy_info){
	
	std::string dirtosearch = paramotopy_info.base_dir;
	dirtosearch.append("/failure_analysis");
	std::vector< boost::filesystem::path > found_runs = FindDirectories(dirtosearch, "^pass");
	
	for (int ii = 0; ii<int(found_runs.size()); ++ii) {
		std::stringstream command;
		command << "rm -rf "
			<< found_runs[ii].string();
		std::cout << command.str() << std::endl;
		system(command.str().c_str());
	}

	paramotopy_info.location = paramotopy_info.base_dir;
	failinfo::find_failed_paths(paramotopy_info);
	failinfo::report_failed_paths(paramotopy_info);
	
	current_iteration = 0;
	
	return;
}

void failinfo::RecoverProgress(runinfo & paramotopy_info){
	//need to set current folder number
	std::string dirtosearch = paramotopy_info.base_dir;
	
	std::vector< boost::filesystem::path > failure_paths = FindDirectories(dirtosearch,"^failure_analysis");
	if (failure_paths.size()==0) {
		current_iteration = 0;
		paramotopy_info.location = paramotopy_info.base_dir;
	}
	else{
		dirtosearch.append("/failure_analysis");
		
		std::vector< boost::filesystem::path > found_runs = FindDirectories(dirtosearch, "^pass");
		int tmpiteration;
		if (found_runs.size()==0) {
			current_iteration=0;
			paramotopy_info.location = paramotopy_info.base_dir;
		}
		else{
			int highestiteration = 0;
			for (int ii = 0; ii<int(found_runs.size()); ++ii) {
				std::cout << "found " << found_runs[ii].string() << std::endl;
				std::string tmppath = found_runs[ii].string();
				tmppath.append("/info");
				std::ifstream fin(tmppath.c_str());
				std::string tmpstr;
				getline(fin,tmpstr);
				std::stringstream ss;
				ss << tmpstr;
				ss >> tmpiteration;
				fin.close();
				
				if (tmpiteration > highestiteration){
					highestiteration = tmpiteration;
				}
			}
			
			current_iteration = highestiteration+1;
			paramotopy_info.location = paramotopy_info.base_dir;
			paramotopy_info.location.append("/failure_analysis/pass");
			std::stringstream ss;
			ss << (current_iteration-1);
			paramotopy_info.location.append(ss.str());
		}
	}

	failinfo::find_failed_paths(paramotopy_info);
	failinfo::report_failed_paths(paramotopy_info);
	
	return;
}

void failinfo::PerformAnalysis(ProgSettings & paramotopy_settings, runinfo & paramotopy_info){


	paramotopy_settings.set_path_failure_settings();


	for (int mm=0; (mm< paramotopy_settings.settings["PathFailure"]["maxautoiterations"].intvalue) && (totalfails>0); ++mm) {
		
		
		
		std::cout << "analyzing failed paths, iteration " << mm+1 << " of " << paramotopy_settings.settings["PathFailure"]["maxautoiterations"].intvalue << "\n";
		
		paramotopy_info.location = paramotopy_info.base_dir;
		paramotopy_info.location.append("/failure_analysis/pass");
		std::stringstream ss;
		ss << current_iteration;
		paramotopy_info.location.append(ss.str());
		mkdirunix(paramotopy_info.location);
		std::string openme = paramotopy_info.location;
		openme.append("/info");
		std::ofstream fout(openme.c_str());
		fout << current_iteration;
		fout.close();
		failinfo::write_failed_paths(paramotopy_info, current_iteration);
		
		
		std::cout << "writing modified paramotopy\n";
		paramotopy_info.WriteModifiedParamotopy( paramotopy_info.location, mm);
		
		//controls whether tolerances are tightened,
		if (paramotopy_settings.settings["PathFailure"]["tightentolerances"].intvalue==1){
			paramotopy_settings.tightentolerances();
		}
		
		
		//do a new step 1 solve (is actually a step 2 solve, to new random points)
		std::string makeme = paramotopy_info.location;
		mkdirunix(makeme.append("/step1"));
		
		if (paramotopy_settings.settings["PathFailure"]["newrandommethod"].intvalue==1) {
			std::cout << "doing new step one\n";
			failinfo::new_step_one(paramotopy_settings, paramotopy_info);
			
		}
		else{
			
			failinfo::copy_step_one(paramotopy_info.base_dir, paramotopy_info.location, mm);
		}
		//run a step2 on the failed points.
		
		steptwo_case(paramotopy_settings, paramotopy_info);
		failinfo::find_failed_paths(paramotopy_info);
		
		std::cout << "\nThere were " << totalfails << " points with path failures, out of " << num_points_inspected << " total points.\n";
		std::cout << "completed " << mm+1 << " iterations" << std::endl;
		current_iteration++;
	}
	//run a (step1) run to get new start point.
	
	
}




//write failed path info to the screen
void failinfo::report_failed_paths(runinfo paramotopy_info){
	

	
	if (totalfails == 0) {
		std::cout << "congratulations, your run had 0 path failures total!\n";
		return;
	}

	
	
	int premie = std::cout.precision();//for restoring at end of function
	std::cout << setiosflags(std::ios::fixed | std::ios::showpoint);
	std::cout.precision(3);
	std::cout << "the ";
	if (totalfails>30) {
		std::cout << "last ";
	}
	std::cout << std::min(30,totalfails-30) << " parameter points with failures:\n\n";
	


	//get whether we have imaginary components for each column
	
	std::vector< bool > have_imaginary;
	have_imaginary.resize(paramotopy_info.numparam);
	for (int ii = 0;ii< paramotopy_info.numparam; ++ii) {
		have_imaginary[ii] = false;
	}
	
	
	for (int ii=std::max(0,totalfails-30) ; ii < totalfails; ++ii) {
		for (int jj = 0; jj<paramotopy_info.numparam; ++jj) {
			if (fabs(fail_vector[ii][jj].second)>0.0001){
				have_imaginary[jj] = true;
			}
		}
	}
	std::vector< int > column_widths;
	column_widths.resize(paramotopy_info.numparam);
	for (int ii = 0; ii<paramotopy_info.numparam; ++ii) {
		if (have_imaginary[ii]) {
			column_widths[ii] = 13;
		}
		else{
			column_widths[ii] = 7;
		}
	}
	for (int ii = 0; ii<paramotopy_info.numparam; ++ii) {
		std::cout << "  " << paramotopy_info.ParameterNames[ii];
		for (int jj=0; jj<(column_widths[ii]-paramotopy_info.ParameterNames[ii].length()+2); ++jj) {
			std::cout << " ";
		}
	}
	std::cout << std::endl << std::endl;
	for (int ii=std::max(0,totalfails-30) ; ii < totalfails; ++ii) {
		for (int jj = 0; jj<paramotopy_info.numparam; ++jj) {
			std::stringstream ss;
			ss << printf("%.3f",fail_vector[ii][jj].first);
			if (fail_vector[ii][jj].second!=0){
				ss << "+i*" << fail_vector[ii][jj].second;
			}
			std::string tmpstr = ss.str();
			std::cout << tmpstr;
			for (int kk=0; kk<(column_widths[jj]-tmpstr.size()); ++kk) {
				std::cout << " ";
			}
		}
		std::cout << std::endl;
	}
	
	
	std::cout << "\nThere were " << totalfails << " points with path failures, out of " << num_points_inspected << " total points.\n";

	std::cout.precision(premie);
	std::cout.unsetf(std::ios::fixed | std::ios::showpoint);
	return;
}




void failinfo::copy_step_one(std::string from_dir, std::string to_dir, int iteration){
	
	std::string tmpstr;
	std::string from_file = from_dir, to_file = to_dir;
	from_file.append("/step1/nonsingular_solutions");
	to_file.append("/step1/nonsingular_solutions");
	
	std::ifstream fin(from_file.c_str());
	if (!fin.is_open()) {
		std::cerr << "failed to open " << from_file << " to read" << std::endl;
	}
	std::ofstream fout(to_file.c_str());
	if (!fout.is_open()) {
		std::cerr << "failed to open " << to_file << " to write" << std::endl;
	}
	
	
	
	
	while (getline(fin,tmpstr)) {
		fout << tmpstr << "\n";
	}
	
	fin.close();
	fout.close();
	
	return;
	
}




std::string failinfo::new_step_one(ProgSettings paramotopy_settings,runinfo paramotopy_info){
	
	
	std::string start;
	
	
	GetStart(paramotopy_info.base_dir,
			 start);
	
	//write step2 to memory.
	paramotopy_info.RandomValues = paramotopy_info.MakeRandomValues(42);//lol  the integer passed here is only to use polymorphism to get a slightly different function.  joke's on you.  lol lol lollololooolol.  
	
	//now need to write to file in location.
	std::string inputstring = WriteFailStep2(paramotopy_info.RandomValues,
										 paramotopy_settings,
										 paramotopy_info);
	
	std::string bertinifilename = paramotopy_info.location;
	bertinifilename.append("/step1");
	mkdirunix(bertinifilename);
	bertinifilename.append("/input");
	std::ofstream fout;
	fout.open(bertinifilename.c_str());
	if (!fout.is_open()) {
		std::cerr << "writing step1(2) input for failed paths FAILED to open\n";
	}
	fout << inputstring;
	fout.close();
	
	bertinifilename = paramotopy_info.location;
	bertinifilename.append("/step1/start");
	fout.open(bertinifilename.c_str());
	if (!fout.is_open()) {
		std::cerr << "writing step1(2) start file for failed paths FAILED to open\n";
	}
	fout << start;
	fout.close();
	
	
	//call bertini, in parallel if possible
	CallBertiniStep1(paramotopy_settings, 
					 paramotopy_info);
	
	
	
	//get the start string
	std::string start_string = "unset start string";
	
	return start_string;
}









//writes the failed points and indices to two separate files in location.
void failinfo::write_failed_paths(runinfo paramotopy_info, int iteration){
	
	std::stringstream ss;
	ss << paramotopy_info.location << "/mc";
	struct stat filestatus;
	
	if (stat(ss.str().c_str(), &filestatus)==0) {
		int success = remove(ss.str().c_str());	
		std::cout << "tried to remove previous mc file, with code " << success << std::endl;
	}
	
	
	
	std::ofstream fout;
	fout.open(ss.str().c_str());
	
	for (int ii = 0; ii < totalfails; ++ii) {
		for (int jj = 0;  jj < paramotopy_info.numparam; ++jj) {
			fout << fail_vector[ii][jj].first << " " << fail_vector[ii][jj].second << " "; 
		}
		fout << "\n";
	}
	
	fout.close();
	
	ss.clear();
	ss.str("");
	
	ss << paramotopy_info.location << "/fail_indices";
	fout.open(ss.str().c_str());
	for (int ii = 0; ii < totalfails; ++ii) {
		fout << index_vector[ii] << "\n";
	}
	fout.close();
	
	return;
}





int failinfo::find_failed_paths(runinfo paramotopy_info){
	
	failinfo::get_folders_for_fail(paramotopy_info.location);
	
	num_points_inspected = 0;
	index_vector.clear();
	fail_vector.clear();
	totalfails = 0;
	//	Data members used throughout the program  
	
	std::string location, inputfilename, outputfilename, wasteme, tmp, tmpstr;
	std::ifstream fin;
	std::ofstream fout;

	std::stringstream ss, intstream;
	int linenumber;
	num_points_inspected = 0;
	
	std::vector< std::pair<double,double> > tempparam; 
	tempparam.resize(paramotopy_info.numparam);	
	std::string tempname;
		
	for (int bla=0; bla<int(foldervector.size()); ++bla){
		
		inputfilename = foldervector[bla];
		inputfilename.append("/failed_paths");
		
		
		int filenum = 0, totalfails_thisfolder = 0;
		struct stat filestatus;
		
		tempname = inputfilename;
		tempname.append("0");
		
		while (   stat( tempname.c_str(), &filestatus ) ==0  ) {  //while can open file
			fin.open(tempname.c_str());
			getline(fin,tmpstr); // superfluous as of july 3 2012.  remove next viewing of this comment.
			int num_points_inspected_individual = 0;
			while ( getline(fin,tmpstr) ) { // get line.  either blank (no more) or a # (line num or index)
				if (tmpstr.length()==0 ){  // if true, then blank.  file ought to be done
					
					//std::cout << "line prematurely blank? " << tmpstr << "\n";
					break;
				}
				else { //not blank.  work to do
					++num_points_inspected;
					++num_points_inspected_individual;
					ss << tmpstr;
					ss >> linenumber; //get the line number
					//std::cout << linenumber << "\n";
					ss.clear();
					ss.str("");
					
					getline(fin,tmpstr); //  gets the parameter values for this line.  need to save if have fails
					ss << tmpstr;

					//set the temporary parameter values
					for (int i = 0; i < paramotopy_info.numparam; ++i) {
						ss >> tempparam[i].first;
						ss >> tempparam[i].second;
					}
					ss.clear();
					ss.str("");
					
				
					
					int tempfailnum = 0;
					while (1) {
						
						getline(fin,tmpstr);
						if (tmpstr.length()==0) { // if do not have a solution
							break;
						}
						else { //have a soln
							for (int i=0; i<3+paramotopy_info.numvariables; ++i) {
								getline(fin,tmpstr);
							}
							tempfailnum++;
						}
						
						
					}
					if (tempfailnum > 0){

						index_vector.push_back(linenumber);
						fail_vector.push_back(tempparam);
						totalfails_thisfolder++;
					}
				}
				
			}
			
			std::cout << tempname << " " << totalfails_thisfolder << " " << num_points_inspected_individual << "\n";
			
			
			
			fin.close();
			filenum++;//increment
			ss << filenum;  //make new file name
			tempname = inputfilename;
			tempname.append(ss.str());
			ss.clear();
			ss.str("");
		} // re: while
		
		
		totalfails += totalfails_thisfolder;
	}	//re:for (int bla=0; bla<foldervector.size(); ++bla)
	
	return num_points_inspected;
}



void failinfo::get_folders_for_fail(std::string dir){

	foldervector.clear();
	
	
	//read in folder file.
	std::string foldername = dir;
	foldername.append("/folders");
	//std::cout << foldername << std::endl;  //remove me
	std::ifstream folderstream;
	folderstream.open(foldername.c_str());
	
	if (!folderstream.is_open()) {
		std::cerr << "failed to open file to read folders " << foldername << std::endl;
	}
	else{
		std::string tmpstr;

		
		while (getline(folderstream,tmpstr)) {
			foldervector.push_back(tmpstr);
			//std::cout << tmpstr << std::endl; //comment me out
		}
		folderstream.close();
	}
	
	return;
}


