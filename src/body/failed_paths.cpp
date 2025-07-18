#include "paramotopy/failed_paths.hpp"


//the controller function for doing stuff to the failed points.  called by paramotopy's main.

void failinfo::MainMenu(ProgSettings & paramotopy_settings, runinfo & paramotopy_info){
	
  //make sure the run was completed.
  if (!paramotopy_info.test_if_finished()) {
    std::cout << "\n\nthe run for " << paramotopy_info.inputfilename << " appears not completed.  please complete the run." << std::endl;
    return;  //run was not completed, ought not continue.
  }
  
  //set settings for path failure.
  
  initial_fails.clear();
  controller_fails.clear();
  terminal_fails.clear();
  
  failinfo::find_failed_paths(paramotopy_info,0,0);
  initial_fails[0] = this->controller_fails;   //seed the data
  
  boost::filesystem::path makeme = paramotopy_info.base_dir;
  makeme /= "failure_analysis";
  boost::filesystem::create_directory(makeme);
  
  
  failinfo::RecoverProgress(paramotopy_info);//gets how far we were
  
  paramotopy_settings.set_path_failure_settings();//sets the current settings from the base settings.
  std::cout << "current iteration " << current_iteration << std::endl;
  std::cout << "\nCurrently " << this->totalfails << " points with path failures.\n";
  std::stringstream menu;
  menu << "\n\nPath Failure Menu:\n\n"
	<< "1) ReRun Failed Paths\n"
	<< "2) Clear Failed Path Data (start over)\n"
	<< "3) Change path failure settings (resets tolerance tightening)\n"
	<< "4) Report failed paths to screen\n"
	<< "*\n"
	<< "0) Go Back\n"
	<< "\n: ";
  int choice = -1001;
  while (choice!=0) {
    paramotopy_settings.DisplayCurrentSettings("PathFailure");
    choice = get_int_choice(menu.str(),0,4);
    
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
				paramotopy_settings.set_path_failure_settings();
				break;
				
			case 3:
				paramotopy_settings.PathFailureMenu();
				paramotopy_settings.set_path_failure_settings();//sets the current settings from the base
				break;
				
			case 4:
				failinfo::report_failed_paths(paramotopy_info);
				break;
				
			default:
				
				break;
    }
    paramotopy_info.UpdateAndSave();
    
  }
  
  
  
  return;
  
	
}


void failinfo::StartOver(runinfo & paramotopy_info){
	
  boost::filesystem::path dirtosearch = paramotopy_info.base_dir;
  dirtosearch /= "failure_analysis";
  std::vector< boost::filesystem::path > found_runs = FindDirectories(dirtosearch, "^pass");
  
  for (int ii = 0; ii<int(found_runs.size()); ++ii) {
    boost::filesystem::remove_all(found_runs[ii]);
  }
  
  this->controller_fails.clear();
  this->initial_fails.clear();
  this->terminal_fails.clear();
  
  paramotopy_info.location = paramotopy_info.base_dir;
  failinfo::find_failed_paths(paramotopy_info,0,0);
  initial_fails[0] = this->controller_fails;
  
  
  this->current_iteration=0;
  failinfo::set_location_fail(paramotopy_info);
  failinfo::write_successful_resolves(paramotopy_info);
  paramotopy_info.location = paramotopy_info.base_dir;
  
  failinfo::write_successful_resolves(paramotopy_info);
  
  
  
  paramotopy_info.GetPrevRandom();
  
  
  return;
}








/// this function does three things: recovers the iteration number, recovers the list of points which have path failures, and reports the paths to the user.
void failinfo::RecoverProgress(runinfo & paramotopy_info){
  
	
  //first, we need to set current folder number
  boost::filesystem::path dirtosearch = paramotopy_info.base_dir;
  std::vector< boost::filesystem::path > failure_paths = FindDirectories(dirtosearch,"^failure_analysis");
  if (failure_paths.size()==0) {
    this->current_iteration=0;
//		failinfo::set_location_fail(paramotopy_info);
    failinfo::write_successful_resolves(paramotopy_info);
    paramotopy_info.location = paramotopy_info.base_dir;
  }
  else{
    dirtosearch /= "failure_analysis";
    
    std::vector< boost::filesystem::path > found_runs = FindDirectories(dirtosearch, "^pass"); // carat means beginning is `pass'
    int tmpiteration;
    if (found_runs.size()==0) {
      this->current_iteration=0;
      //			failinfo::set_location_fail(paramotopy_info);
      failinfo::write_successful_resolves(paramotopy_info);
      paramotopy_info.location = paramotopy_info.base_dir;
    }
    else{
      int highestiteration = -1;
      for (int ii = 0; ii<int(found_runs.size()); ++ii) {
				std::cout << "found " << found_runs[ii].string() << std::endl;
				boost::filesystem::path tmppath = found_runs[ii];
				tmppath /= "info";
				std::ifstream fin(tmppath.c_str());
				std::string tmpstr;
				getline(fin,tmpstr);
				std::stringstream ss;
				ss << tmpstr;
				ss >> tmpiteration;
				fin.close();
				
				
				
				if (!TestIfFinished(found_runs[ii])){
					std::cout << "removing folder " << found_runs[ii] << " because was not finished..." << std::endl;
					boost::filesystem::remove_all(found_runs[ii]);
					continue;
				}
				
				if (tmpiteration > highestiteration){
					highestiteration = tmpiteration;
				}
				
				
				
				this->current_iteration = tmpiteration;
				failinfo::set_location_fail(paramotopy_info);
				failinfo::find_failed_paths(paramotopy_info,1,current_iteration+1);
				
				if (current_iteration>0) {
					this->terminal_fails[current_iteration-1] = initial_fails[current_iteration];
				}
				
				
				
      }
			
      
      this->current_iteration = highestiteration+1;
      failinfo::set_location_fail(paramotopy_info);
      
      datagatherer lets_gather_some_data(paramotopy_info.base_dir,paramotopy_info.fundamental_dir,paramotopy_info.numvariables);
      std::map< int, point > successful_resolves = lets_gather_some_data.ReadSuccessfulResolves();
      failinfo::make_controller_from_recovered(successful_resolves);
      
      
    }
    
    
  }
  
	
//  failinfo::report_restored_data();
  
  
  return;
}

void failinfo::report_restored_data(){
	
  std::map< int, std::vector< point> >::iterator iter;
  
  std::cout << "initial:" << std::endl;
  for (iter=initial_fails.begin(); iter!=initial_fails.end(); iter++) {
    std::cout << iter->first << std::endl;
    
    for (int ii=0; ii<int(iter->second.size()); ++ii) {
      std::cout << iter->second[ii].index << std::endl;
    }
  }
  
  std::cout << "terminal:" << std::endl;
  for (iter=terminal_fails.begin(); iter!=terminal_fails.end(); iter++) {
    for (int ii=0; ii<int(iter->second.size()); ++ii) {
      std::cout << iter->second[ii].index << std::endl;
    }
  }
  
  std::cout << "controller:" << std::endl;
  for (int ii=0; ii< int(controller_fails.size()); ++ii) {
    std::cout << controller_fails[ii].index << std::endl;
  }
  return;
}



void failinfo::make_controller_from_recovered(std::map< int, point > successful_resolves){
	
//  std::cout << "successful_resolves has " << successful_resolves.size() << " elements." << std::endl;
  
  std::map< int, point >::iterator iter;
  std::vector< point > tmpcrap;
//TODO:  this really needs to be rewritten to be memory efficient.
  for (iter=successful_resolves.begin(); iter!= successful_resolves.end(); iter++) {
    
//    std::cout << iter->first << std::endl;
    
    tmpcrap.push_back(iter->second);
    
  }
  
  std::sort(tmpcrap.begin(),tmpcrap.end());
  
  controller_fails = tmpcrap;
  
  return;
}



void failinfo::set_location_fail(runinfo & paramotopy_info){
  
  
  
  paramotopy_info.location = paramotopy_info.base_dir;
  paramotopy_info.location /= "failure_analysis/pass";
  std::stringstream ss;
  ss << (this->current_iteration);
  paramotopy_info.location += ss.str();
  
//  std::cout << "setting location fail " << paramotopy_info.location.string() << std::endl;
//  boost::filesystem::create_directory(paramotopy_info.location);
  
  return;
}








void failinfo::PerformAnalysis(ProgSettings & paramotopy_settings, runinfo & paramotopy_info){
	
	
	
	
	
  for (int mm=0; (mm< paramotopy_settings.settings["PathFailure"]["maxautoiterations"].intvalue) && (totalfails>0); ++mm) {
    
    
    
    std::cout << "analyzing failed paths, iteration " << mm+1 << " of " << paramotopy_settings.settings["PathFailure"]["maxautoiterations"].intvalue << "\n";
    
    failinfo::set_location_fail(paramotopy_info);
    boost::filesystem::create_directory(paramotopy_info.location);
		
    boost::filesystem::path openme = paramotopy_info.location;
    openme /= "info";
    std::ofstream fout(openme.c_str());
    fout << this->current_iteration;
    fout.close();
    failinfo::write_failed_paths(paramotopy_info, current_iteration);
    
    
    std::cout << "writing modified paramotopy\n";
    paramotopy_info.WriteModifiedParamotopy( paramotopy_info.location, mm);
    
		//controls whether tolerances are tightened,
    if (paramotopy_settings.settings["PathFailure"]["tightentolerances"].intvalue==1){
      paramotopy_settings.tightentolerances();
    }
    
    
    //do a new step 1 solve (is actually a step 2 solve, to new random points)
    boost::filesystem::path makeme = paramotopy_info.location;
    boost::filesystem::create_directories(makeme/="step1");
    
    if (paramotopy_settings.settings["PathFailure"]["newrandommethod"].intvalue==1) {
      std::cout << "doing new step one\n";
      failinfo::new_step_one(paramotopy_settings, paramotopy_info);
    }
    else{
      failinfo::copy_step_one(paramotopy_info.base_dir, paramotopy_info.location, mm, paramotopy_settings.settings["mode"]["startfilename"].value());
    }
    
    
    //run a step2 on the failed points.
    
    
		std::string previous_start_file = "";
    if (paramotopy_settings.settings["PathFailure"]["newrandommethod"].intvalue==1) {
      previous_start_file = paramotopy_settings.settings["mode"]["startfilename"].value();
      paramotopy_settings.setValue("mode","startfilename",std::string("nonsingular_solutions"));
    }
    
    int terminationint = GetMcNumLines(paramotopy_info.location,paramotopy_info.numparam);
    
    if (terminationint==0) {
      std::cerr << "have 0 points to solve for some reason." << std::endl;
      break;
    }
		
    int previous_mainmode = paramotopy_settings.settings["mode"]["main_mode"].intvalue;
    int previous_num_procs = paramotopy_settings.settings["parallelism"]["numprocs"].intvalue;
    int previous_numfilesatatime = paramotopy_settings.settings["parallelism"]["numfilesatatime"].intvalue;
    // here put code for turning on/off parsing
    if (terminationint+1 < previous_num_procs) {
      paramotopy_settings.setValue("parallelism","numprocs",terminationint+1);
      paramotopy_settings.setValue("parallelism","numfilesatatime",1);
    }
    else{
      double fractpart, intpart;
      
      fractpart = modf ( (terminationint+1) / previous_num_procs , &intpart);
      
      int numprocstouse = std::max(double(1),intpart/5);
      paramotopy_settings.setValue("parallelism","numfilesatatime",numprocstouse);
    }
    
    paramotopy_settings.setValue("mode","main_mode",int(0));
    
		
		
    // the true boolean is to assume all failed paths are being done
    // with a standard parameter continuation resolve
    // and not any other option (i.e. solve at particular parameter
    // point at finer settings because it doesn't match the
    // NID at a random point. -- work needs to be done
    // on failed path analysis stuff after this other implementation goes through
		
    steptwo_case(paramotopy_settings, paramotopy_info); // in menu_cases.cpp
    
    
    //recall the old values for a couple of settings
    if (paramotopy_settings.settings["PathFailure"]["newrandommethod"].intvalue==1) {
      paramotopy_settings.setValue("mode","startfilename",std::string(previous_start_file));
    }
    
		paramotopy_settings.setValue("mode","main_mode",previous_mainmode);
    paramotopy_settings.setValue("parallelism","numprocs",previous_num_procs);
    paramotopy_settings.setValue("parallelism","numfilesatatime",previous_numfilesatatime);
		paramotopy_info.GetPrevRandom();
    //need to check integrity of completed run -- sometimes will fail / be cancelled, and need to be able to recover from these halts.
    
    boost::filesystem::path path_to_check = paramotopy_info.location;
    if (!TestIfFinished(path_to_check)){
      //
      std::cout << "it appears the run failed.  removing files." << std::endl;
      break;
    }
    
    failinfo::find_failed_paths(paramotopy_info,2,this->current_iteration); // we find the points which had failures.
    
    failinfo::find_successful_resolves(paramotopy_info);
    
    failinfo::write_successful_resolves(paramotopy_info);
    
    std::cout << "\nThere were " << this->totalfails << " points with path failures, out of " << this->num_points_inspected << " total points.\n";
    
		
    this->initial_fails[this->current_iteration+1] = this->terminal_fails[this->current_iteration];
    this->current_iteration++;
	}
  //run a (step1) run to get new start point.
  
  return;
  
}//re: perform_analysis





void failinfo::write_successful_resolves(runinfo paramotopy_info){
  
  
  boost::filesystem::path file_to_write_to = paramotopy_info.base_dir;
  
  
  file_to_write_to /= "failure_analysis/successful_resolves";
  
  std::ofstream outputfile(file_to_write_to.string().c_str());
  
  if (!outputfile.is_open()) {
    std::cerr << "failed to open " << file_to_write_to.string() << " to write resolve data" << std::endl;
    return;
  }
  
  outputfile << controller_fails.size() << "\n";
  for (int ii = 0; ii<int( controller_fails.size() ); ++ii) {
    outputfile << controller_fails[ii].index << " " << controller_fails[ii].collected_data.size() << "\n";
//    std::cout << controller_fails[ii].index << " " << controller_fails[ii].collected_data.size() << std::endl;
    
    
    if ( int(controller_fails[ii].collected_data.size())>0){
      std::map<std::string, std::string>::iterator iter;
      std::stringstream namelist;
      std::stringstream datalist;
      
      for ( iter=controller_fails[ii].collected_data.begin(); iter!=controller_fails[ii].collected_data.end(); ++iter) {
				namelist << iter->first << " ";
				datalist << iter->second;
      }
      outputfile << namelist.str() << "\n";
      outputfile << datalist.str();
    }
  }
  outputfile.close();
  
  
  boost::filesystem::path blabla = paramotopy_info.base_dir;
  blabla /= "failure_analysis/resolved_file";
  
  std::ofstream outputfile2(blabla.string().c_str());
  outputfile2 << file_to_write_to.string();
  outputfile2.close();
  return;
}


void failinfo::find_successful_resolves(runinfo & paramotopy_info){
  
  datagatherer lets_gather_some_data(paramotopy_info.numvariables, paramotopy_info.location);
  
  
  std::vector< point > current_successful_resolves;
  lets_gather_some_data.GatherDataForFails(this->terminal_fails[this->current_iteration],current_successful_resolves);
  
  //have successful_resolves, a vector of points, containing the data for re-solved points from this iteration.  indices are WRT the current iteration.
  
  std::vector< int > relative_indices, final_indices;
  relative_indices.resize(current_successful_resolves.size());
  final_indices.resize(current_successful_resolves.size());
  
  
  for (int ii=0; ii<int(current_successful_resolves.size()); ++ii) {
    int failed_again_index = current_successful_resolves[ii].index;
    
    if (current_iteration == 0){
      relative_indices[ii] = failed_again_index;
    }
    
    for (int jj = current_iteration; jj>=0; --jj) {
      failed_again_index = initial_fails[jj][failed_again_index].index;
      if (jj==1) {
				relative_indices[ii] = failed_again_index;
      }
      if (jj==0) {
				final_indices[ii] = failed_again_index;
      }
    }
    
  }
  
  failinfo::merge_successful_resolves(current_successful_resolves,relative_indices,final_indices); //merges the data into the controller list.
  
  
	
  return;
}


void failinfo::merge_successful_resolves(std::vector< point > current_successful_resolves, std::vector< int > relative_indices, std::vector< int > final_indices){
	
	
  for (int ii=0; ii<int( current_successful_resolves.size() ); ++ii) {
    if (controller_fails[relative_indices[ii]].index != final_indices[ii]) {
      std::cerr << "mismatch in indices when commiting to controller list" << std::endl;
    }
    else{
      controller_fails[relative_indices[ii]].collected_data = current_successful_resolves[ii].collected_data;
    }
  }
  
  return;
}

//write failed path info to the screen
void failinfo::report_failed_paths(runinfo paramotopy_info){
  
  
	
  if (this->initial_fails[current_iteration].size() == 0) {
    std::cout << current_iteration << " congratulations, your run had 0 path failures total!\n";
    return;
  }
  
  
  
  int premie = std::cout.precision();//for restoring at end of function
	std::cout << std::setiosflags(std::ios::fixed | std::ios::showpoint);
  std::cout.precision(3);
  std::cout << "the ";
  if (this->initial_fails[current_iteration].size()>30) {
    std::cout << "last ";
  }
  std::cout << std::min(30,int(this->initial_fails[current_iteration].size())) << " parameter points with failures:\n\n";
  
  
  
  //get whether we have imaginary components for each column
  
  //preallocate
  std::vector< bool > have_imaginary;
  have_imaginary.resize(paramotopy_info.numparam);
  for (int ii = 0;ii< paramotopy_info.numparam; ++ii) {
    have_imaginary[ii] = false;
  }
  
  //assign actual values
  for (int ii=std::max(0,int(this->initial_fails[current_iteration].size())-30) ; ii < int(this->initial_fails[current_iteration].size()); ++ii) {
    for (int jj = 0; jj<paramotopy_info.numparam; ++jj) {
      if (fabs( this->initial_fails[current_iteration][ii].parameter_values[jj].second  )>0.0001){
				have_imaginary[jj] = true;
      }
    }
  }
  
  std::vector< int > column_widths;
  column_widths.resize(paramotopy_info.numparam);
  for (int ii = 0; ii<paramotopy_info.numparam; ++ii) {
    if (have_imaginary[ii]) {
      column_widths[ii] = 16;
    }
    else{
      column_widths[ii] = 9;
    }
  }
  for (int ii = 0; ii<paramotopy_info.numparam; ++ii) {
    std::cout << "  " << paramotopy_info.ParameterNames[ii];
    for (int jj=0; jj<(column_widths[ii]- int(paramotopy_info.ParameterNames[ii].length())+2); ++jj) {
      std::cout << " ";
    }
  }
  std::cout << std::endl << std::endl;
  for (int ii=std::max(0,int(this->initial_fails[current_iteration].size())-30);ii < int(this->initial_fails[current_iteration].size()); ++ii) {
    for (int jj = 0; jj<paramotopy_info.numparam; ++jj) {
      std::stringstream ss;
      ss << printf("%.3f",initial_fails[current_iteration][ii].parameter_values[jj].first);
      if (initial_fails[current_iteration][ii].parameter_values[jj].second!=0){
				ss << "+i*" << initial_fails[current_iteration][ii].parameter_values[jj].second;
      }
      std::string tmpstr = ss.str();
      std::cout << tmpstr;
      for (int kk=0; kk<( column_widths[jj]-int(tmpstr.size()) ); ++kk) {
				std::cout << " ";
      }
    }
    std::cout << std::endl;
  }
  
  
  std::cout << "\nThere were " << this->initial_fails[current_iteration].size() << " points with path failures, out of " << num_points_inspected << " total points.\n";
  
  std::cout.precision(premie);
  std::cout.unsetf(std::ios::fixed | std::ios::showpoint);
  return;
}




void failinfo::copy_step_one(boost::filesystem::path from_dir, boost::filesystem::path to_dir,
														 int iteration, std::string startfilename){
  
  std::string tmpstr;
  boost::filesystem::path from_file = from_dir;
	
  from_file /= "step1";
  from_file /= startfilename;
	
	boost::filesystem::path to_file = to_dir;
  to_file /= "step1";
  to_file /= startfilename;
  
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




std::string failinfo::new_step_one(ProgSettings & paramotopy_settings,runinfo & paramotopy_info){
	
  
  std::string start;
  
  
  int num_paths_to_track = GetStart(paramotopy_info.base_dir,
																		start,
																		paramotopy_settings.settings["mode"]["startfilename"].value());
  
  
	//write step2 to memory.
  
  std::vector< std::pair< double, double> > new_random_values = paramotopy_info.MakeRandomValues(42);//lol  the integer passed here is only to use polymorphism to get a slightly different function.  joke's on you.  lol lol lollololooolol.
  
  //now need to write to file in location.
  std::string inputstring = WriteFailStep2(new_random_values,
																					 paramotopy_settings,
																					 paramotopy_info); // bool set to true for standardstep2
  
  paramotopy_info.RandomValues = new_random_values;
  
  
  boost::filesystem::path bertinifilename = paramotopy_info.location;
  bertinifilename /= "step1";
	boost::filesystem::create_directories(bertinifilename);
  bertinifilename /= "input";
  std::ofstream fout;
  fout.open(bertinifilename.c_str());
  if (!fout.is_open()) {
    std::cerr << "writing step1(2) input for failed paths FAILED to open to write\n";
  }
  fout << inputstring;
  fout.close();
  
  bertinifilename = paramotopy_info.location;
  bertinifilename /= "step1";
	bertinifilename /= "start";
  fout.open(bertinifilename.c_str());
  if (!fout.is_open()) {
    std::cerr << "writing step1(2) start file for failed paths FAILED to open\n";
  }
  fout << start;
  fout.close();
  
  
  //be intelligent about the number of processors to use.
  int previous_num_procs = paramotopy_settings.settings["parallelism"]["numprocs"].intvalue;
  // here put code for turning on/off parsing
  if (num_paths_to_track < previous_num_procs) {
    paramotopy_settings.setValue("parallelism","numprocs",num_paths_to_track+1);
  }
  
  
  //call bertini, in parallel if possible
  CallBertiniStep1(paramotopy_settings,
									 paramotopy_info);
  
	
  //restore the previous value for the numprocs setting
  if (num_paths_to_track < previous_num_procs) {
    paramotopy_settings.setValue("parallelism","numprocs",previous_num_procs);
  }
  
  
  
  //get the start string
  std::string start_string = "unset start string";
  
  return start_string;
}









//writes the failed points and indices to two separate files in location.
void failinfo::write_failed_paths(runinfo paramotopy_info, int iteration){
	


	boost::filesystem::path mcfilename = paramotopy_info.location;
	mcfilename /= "mc";
	
  if (boost::filesystem::exists(mcfilename)) {
    boost::filesystem::remove(mcfilename);
  }
  
  std::cout << "writing " << initial_fails[current_iteration].size() << " points to mc file" << std::endl;
  
  
  std::ofstream fout;
  fout.open(mcfilename.c_str());
	if (!fout.is_open()) {
		std::cerr << "failed to open " << mcfilename << " to write mc file.  cannot continue." << std::endl;
		exit(451);
	}
	
  fout.precision(16);
  fout << initial_fails[current_iteration].size() << "\n";
  for (int ii = 0; ii < int(initial_fails[current_iteration].size()); ++ii) {
    for (int jj = 0;  jj < paramotopy_info.numparam; ++jj) {
      fout << initial_fails[current_iteration][ii].parameter_values[jj].first << " " << initial_fails[current_iteration][ii].parameter_values[jj].second << " ";
    }
    fout << "\n";
  }
  fout.close();
  

  
  boost::filesystem::path failname = paramotopy_info.location;
	failname /= "fail_indices";
  fout.open(failname.c_str());
  for (int ii = 0; ii < totalfails; ++ii) {
    fout << initial_fails[current_iteration][ii].index << "\n";
  }
  fout.close();
  
  return;
}




//parses a series of 'failed_paths' files, searching for points which had such paths.

//make sure you set paramotopy_info.location before calling this function
int failinfo::find_failed_paths(runinfo paramotopy_info, int temporal_flag, int iteration){
	
  if (temporal_flag==0) {
    failinfo::get_folders_for_fail(paramotopy_info.base_dir);
  }
  else{
    failinfo::get_folders_for_fail(paramotopy_info.location);
  }
  
  //start by resetting some data members of the runinfo object.
  this->num_points_inspected = 0;
  this->totalfails = 0;
  
  std::vector< point > temporary_fails;
  
  //	Data members used throughout the function
  std::string inputfilename, outputfilename, wasteme, tmp, tmpstr; // location,
  std::ifstream fin;
  std::ofstream fout;
  
  std::stringstream ss, intstream;
  //int linenumber;
  
  
  point temppoint; //has data members index, parameter_values.
  temppoint.parameter_values.resize(paramotopy_info.numparam);
  
  
  
  std::string tempname;
  
  for (int folder_counter=0; folder_counter<int(foldervector.size()); ++folder_counter){
    
    inputfilename = foldervector[folder_counter];
    inputfilename.append("/failed_paths");
	  
    
    int filenum = 0, totalfails_thisfolder = 0;
    struct stat filestatus;
    
    tempname = inputfilename;
    tempname.append("0");
    
    //todo: rewrite using boost
    while (   stat( tempname.c_str(), &filestatus ) ==0  ) {  //while can open file
      fin.open(tempname.c_str());
      getline(fin,tmpstr); //the top line of the file should be the parameters, causing us to have to read in this line before we start anything.
      int num_points_inspected_individual = 0; //keeps track of the number of points inspected in an individual file.
      while ( getline(fin,tmpstr) ) { // get line.  either blank (no more) or a # (line num or index)
				if (tmpstr.length()==0 ){  // if true, then blank.  file ought to be done
					
					break;
				}
				else { //not blank.  work to do
					
					
					++num_points_inspected;  // increment the point counters.
					++num_points_inspected_individual;
					ss << tmpstr;
					ss >> temppoint.index; //get the line number
					ss.clear();
					ss.str("");
					
					getline(fin,tmpstr); //  gets the parameter values for this line.  need to save if have fails
					ss << tmpstr;
					
					//set the temporary parameter values
					for (int i = 0; i < paramotopy_info.numparam; ++i) {
						ss >> temppoint.parameter_values[i].first;
						ss >> temppoint.parameter_values[i].second;
					}
					ss.clear();
					ss.str("");
					
					
					// parse the (previously) copied 'failedpaths' file.
					int tempfailnum = ParseFailedPaths(fin,paramotopy_info.numvariables);  // in para_aux_funcs.cpp
					
					
					if (tempfailnum > 0){
						temporary_fails.push_back(temppoint);
						totalfails_thisfolder++;
					}
				}
				
      }
      fin.close();
      
      //output the folder's summary to the screen
      std::cout << tempname << " "
			<< totalfails_thisfolder << " "
			<< num_points_inspected_individual << "\n";
      
      
      filenum++;//increment
      ss << filenum;  //make new file name
      tempname = inputfilename; // reset.
      tempname.append(ss.str()); // append the integer onto the end of the filename.  will be the next to open (provided it exists).
      ss.clear();
      ss.str("");
    } // re: while
    
    
    totalfails += totalfails_thisfolder;
  }	//re:for (int folder_counter=0; folder_counter<foldervector.size(); ++folder_counter)
  
  std::sort( temporary_fails.begin(), temporary_fails.end() );
  
  //actually assign the data
  
	
  
  switch (temporal_flag) {
		case 0:
			this->controller_fails.clear();
			this->controller_fails = temporary_fails;
			break;
			
		case 1:
			this->initial_fails[iteration] = temporary_fails;
			break;
			
		case 2:
			this->terminal_fails[iteration] = temporary_fails;
			break;
		default:
			std::cerr << "invalid temporal flag passed in find_failed_paths" << std::endl;
			break;
  }
  
  
  return num_points_inspected;
}












void failinfo::get_folders_for_fail(boost::filesystem::path dir){
	
  foldervector.clear();
  
  
  //read in folder file.
  boost::filesystem::path foldername = dir;
  foldername /= "folders";
  std::ifstream folderstream;
  folderstream.open(foldername.c_str());
  
  if (!folderstream.is_open()) {
    std::cerr << "failed to open file to read folders " << foldername.string() << std::endl;
  }
  else{
    std::string tmpstr;
    
    
    while (getline(folderstream,tmpstr)) {
      foldervector.push_back(tmpstr);
    }
    folderstream.close();
  }
  
  return;
}







//creates a string containing the file for step2 input file. note that the specific points at which we solve are written to the num.out file by each worker, so this is only written once.  then the associated .out files are created in a step2.1 solve, and left intact for the step2.2 solves, of which there are many.
std::string WriteFailStep2(std::vector<std::pair<double, double> > CurrentValues,
						   ProgSettings paramotopy_settings,
						   runinfo paramotopy_info){
	
	bool standardstep2;
	int sstep2 = paramotopy_settings.settings["mode"]["standardstep2"].intvalue;
	if (sstep2 == 0){
		standardstep2 = false;
	}
	else{
		standardstep2 = true;
	}
	
	
	std::stringstream inputstringstream;
	
	inputstringstream << paramotopy_settings.WriteConfigFail();
	
	inputstringstream << paramotopy_info.WriteInputStepTwo(CurrentValues, paramotopy_settings, true);
	
	return inputstringstream.str();
}




