#include "failed_paths.h"


//the master function for doing stuff to the failed points.  called by paramotopy's main.

//returned integer indicates success or something.
int failedpaths_case(int numparam, 
					 int numvariables, 
					 std::string base_dir,
					 std::string filename,
					 int & iteration){
	
	 
	std::cout << base_dir;
	bool finished = test_if_finished(base_dir);  //make sure the run was completed.  
	
	if (finished == false) {
		std::cout << "\n\nthe run for " << filename << " appears not completed.  please complete the run.\n";
		return -1;  //run was not completed, ought not continue.
	}
	
	
	//first, get the folders that contain the data.  
	std::vector< std::string > foldervector = get_folders_for_fail(base_dir);

	
	int totalfails;                                                          //will contain the number of points that had failures.
	std::vector< std::vector< std::pair<double,double> > > fail_vector;      //will contain the actual points in parameter space, that had fails.
	std::vector< int > index_vector;                                         //will contain the indexes of the points that had failures.
	int num_points_inspected = find_failed_paths(index_vector, fail_vector, totalfails, numparam, numvariables, base_dir, foldervector);
	
	
	if (totalfails == 0) {
		std::cout << "congratulations, your run had 0 path failures total!\n";
		return 0;
	}
	else {
	std::cout << "There were " << totalfails << " points with path failures, out of\n " << num_points_inspected << " total points.\n";
	
	
	
	// now have the number of points in original sample which had at least one failed path. 
	// also have the points themselves, as well as the indexes associated with those points in the starting sample.
	
	
	//copy the original paramotopy input file,
	
	
	//run a (step1) run to get new start point.
	
	//run a step2 on the failed points.
	
	//rerun this?
	iteration++;
	//failedpaths_case(numparam,numvariables,base_dir,filename,iteration);
	
	return 0;
	}
	
}






int find_failed_paths(std::vector< int > & index_vector, 
					  std::vector< std::vector< std::pair<double,double> > > & fail_vector, 
					  int & totalfails, 
					  int numparam, 
					  int numvariables, 
					  std::string base_dir,
					  std::vector< std::string > foldervector){

	//	Data members used throughout the program  
	
	std::string location, inputfilename, outputfilename, wasteme, tmp, tmpstr;
	std::ifstream fin;
	std::ofstream fout;

	std::stringstream ss, intstream;
	int linenumber, num_points_inspected = 0;

	
	std::vector< std::pair<double,double> > tempparam; 
	tempparam.resize(numparam);	
	std::string tempname;
	
	totalfails = 0; //initialize to 0;
	
	
	
	
	for (int bla=0; bla<int(foldervector.size()); ++bla){
		
		inputfilename = foldervector[bla];
		inputfilename.append("/failed_paths");
		
		
		int filenum = 0, totalfails_thisfolder = 0;
		struct stat filestatus;
		
		tempname = inputfilename;
		tempname.append("0");
		
		while (   stat( tempname.c_str(), &filestatus ) ==0  ) {  //while can open file
			fin.open(tempname.c_str());
			getline(fin,tmpstr); // gets the parameter names.  no need to keep
			
			while ( getline(fin,tmpstr) ) { // get line.  either blank (no more) or a # (line num or index)
				if (tmpstr.length()==0 ){  // if true, then blank.  file ought to be done
					break;
				}
				else { //not blank.  work to do
					++num_points_inspected;
					ss << tmpstr;
					ss >> linenumber; //get the line number
					ss.clear();
					ss.str("");
					
					getline(fin,tmpstr); //  gets the parameter values for this line.  need to save if have fails
					ss << tmpstr;

					//set the temporary parameter values
					for (int i = 0; i < numparam; ++i) {
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
							for (int i=0; i<3+numvariables; ++i) {
								getline(fin,tmpstr);
							}
							tempfailnum++;
						}
						
						
					}
					if (tempfailnum > 0){
//						std::cout << tempparam.size();
//						for (int ii=0; ii < numparam; ++ii) {
//							std::cout << tempparam[ii].first << " " << tempparam[ii].second << " ";
//						} 
//						std::cout << "\n";
						index_vector.push_back(linenumber);
						fail_vector.push_back(tempparam);
						totalfails_thisfolder++;
					}
				}
				
			}
			
			std::cout << tempname << " " << totalfails_thisfolder << "\n";
			
			
			
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



std::vector< std::string > get_folders_for_fail(std::string base_dir){

	//read in folder file.
	std::string foldername = base_dir;
	foldername.append("/folders");
	std::ifstream folderstream;
	folderstream.open(foldername.c_str());
	std::string tmpstr;
	std::vector< std::string > foldervector;
//	std::cout << "folders to analyze for failed paths:\n";
	while (getline(folderstream,tmpstr)) {
		foldervector.push_back(tmpstr);
//		std::cout << tmpstr << "\n";
	}
	folderstream.close();

	return foldervector;
}


