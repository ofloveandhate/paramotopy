#include "paramotopy/step1_funcs.hpp"



void CallBertiniStep1(ProgSettings paramotopy_settings, 
					  runinfo paramotopy_info){
	
	boost::filesystem::path startingfolder = boost::filesystem::current_path();//stackoverflow_getcwd();  //get the current directory
	
	boost::filesystem::path runfolder;
	runfolder = paramotopy_info.location;
	runfolder /= "step1";

	chdir(runfolder.c_str());
	
	
	std::stringstream command;
	if (paramotopy_settings.settings["parallelism"]["parallel"].intvalue==1) {
		command << paramotopy_settings.settings["parallelism"]["architecture"].value() << " ";
		if (paramotopy_settings.settings["parallelism"]["usemachine"].intvalue==1){
			std::string homedir = getenv("HOME");
			homedir.append("/");

			
			command << " -machinefile " 
					<< homedir
					<< paramotopy_settings.settings["parallelism"]["machinefile"].value() 
					<< " ";
		}
		command << " -n " <<  paramotopy_settings.settings["parallelism"]["numprocs"].value();
	
		command << " " << paramotopy_settings.settings["system"]["bertinilocation"].value() << "/bertini";
	}
	else{
		
		command << " " << paramotopy_settings.settings["system"]["bertinilocation"].value() << "/bertini";
	}
	
	command << paramotopy_settings.settings["parallelism"]["post_command_text"].value();

	std::cout << "\n\n\n\n\n" << command.str() << std::endl;
	if (paramotopy_settings.settings["parallelism"]["just_print_system_command"].intvalue)
	  std::cout << "not actually running, please run the command above yourself from `" << runfolder << "`, and then come back in to paramotopy" << std::endl;
	else{
		std::cout << "\n\n\n\n\n"; 
	  system(command.str().c_str());  //make the system call to bertini
	}
	
	chdir(startingfolder.c_str());  // return to the initial folder
	return;
}







//creates a step 1 input file for bertini, from the parsed paramotopy input file and saves preferences settings.
//prints results to screen and to a file
void WriteStep1(ProgSettings paramotopy_settings,
				runinfo paramotopy_info){

	
	std::string config = paramotopy_settings.WriteConfigStepOne();
	std::string input   = paramotopy_info.WriteInputStepOne(paramotopy_settings);
	


	boost::filesystem::path fname2 = paramotopy_info.location;
	fname2 /= "step1/input";
	
	// open a file output stream to the input file used by bertini for step 1
	std::ofstream fout(fname2.c_str());
	if (!fout.is_open()) {
		std::cout << fname2 << " failed to open for writing step1 input file.\n";
	}
	else {
		fout << config << input << "\n";
	}
	fout.close();
	
	std::cout << "your input file:\n\n"
		<< config << input << "\n\n";
	
	return;
}







//gets the config file opened elsewhere as fin, and writes it to fout.
// assuming we open the input file stream and the output filestream
void MakeConfig(std::ifstream & fin, std::ofstream & fout){
	std::string token; 
	while (getline(fin,token)){
		fout << token;
		fout << "\n";
	}
}
void MakeConfig(std::ifstream & fin, std::stringstream & fout){
	std::string token;
	while (getline(fin,token)){
		fout << token;
		fout << "\n";
	}
}

//a recursive call to write a mesh to a plain text file.
void WriteMeshToMonteCarlo(int level, 
			   std::vector<std::vector<std::pair<double, double> > > Values, 
			   std::string dirfilename, 
			   std::string runningline){
  
  if (level==int(Values.size())-1){
    // at the last end of the parameters ....
    for (int i = 0; i < int(Values[level].size());++i){
      std::stringstream ss;
      ss << runningline;
      ss << " ";
      ss << Values[level][i].first;
      ss << " ";
      ss << Values[level][i].second;
      ss << "\n";
      std::ofstream fout(dirfilename.c_str(),std::ios::app);
      fout << ss.str();
      fout.close();
    }
  }
  else{
    for (int i= 0; i < int(Values[level].size());++i){
      std::stringstream ss;
      ss << runningline;
      ss << " ";
      ss << Values[level][i].first;
      ss << " ";
      ss << Values[level][i].second;
      ss << " ";

      WriteMeshToMonteCarlo(level+1,Values,dirfilename,ss.str());
    }
  } 
}

