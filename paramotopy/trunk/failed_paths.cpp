#include "failed_paths.h"

int failedpaths_case(int numparam, int numvariables, std::string base_dir){
	
	
	std::vector< std::string > foldervector = get_folders_for_fail(base_dir);

	
	
	int totalfails = find_failed_paths(numparam, numvariables, base_dir, foldervector);
	
	return 0;
}






int find_failed_paths(int numparam, int numvariables, std::string base_dir,std::vector< std::string > foldervector){

	//	Data members used throughout the program  
	
	std::string location, inputfilename, outputfilename, wasteme, tmp, tmpstr;
	std::ifstream fin;
	std::ofstream fout;

	std::stringstream ss, intstream;
	int linenumber;

	std::vector< std::vector< std::pair<double,double> > > fail_vector;
	std::vector< std::pair<double,double> > tempparam; 
	tempparam.resize(numparam);	
	std::string tempname;
	
	int totalfails = 0;
	
	
	
	
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
//				std::cout << tmpstr << "\n";
				if (tmpstr.length()==0 ){  // if true, then blank.  file ought to be done
					break;
				}
				else { //not blank.  work to do
					
					ss << tmpstr;
					ss >> linenumber; //make the line number
					ss.clear();
					ss.str("");
					
					getline(fin,tmpstr); //  gets the parameter values for this line.  need to save if have fails
					ss << tmpstr;

					
					for (int i = 0; i < numparam; ++i) {
						ss >> tempparam[i].first;
						ss >> tempparam[i].second;
					}
					ss.clear();
					ss.str("");
					
//					std::cout << tempparam[0].first << "\n";
					
					
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
	
	return totalfails;
}



std::vector< std::string > get_folders_for_fail(std::string base_dir){

	//read in folder file.
	std::string foldername = base_dir;
	foldername.append("/folders");
	std::ifstream folderstream;
	folderstream.open(foldername.c_str());
	std::string tmpstr;
	std::vector< std::string > foldervector;
	std::cout << "folders to analyze for failed paths:\n";
	while (getline(folderstream,tmpstr)) {
		foldervector.push_back(tmpstr);
		std::cout << tmpstr << "\n";
	}
	folderstream.close();

	return foldervector;
}


