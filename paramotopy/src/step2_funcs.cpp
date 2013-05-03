#include "step2_funcs.hpp"



void parse_input_file_bertini(unsigned int & currentSeed, int & MPType){
	
	
	int trackType, genType, userHom, sharpenOnly, needToDiff, remove_temp, useParallelDiff;
	parse_input("input", &trackType, &MPType, &genType, &userHom, &currentSeed, &sharpenOnly, &needToDiff, &remove_temp, useParallelDiff, 0, 1, 0);
}



void run_zero_dim_main(int MPType, unsigned int currentSeed){
	zero_dim_main(MPType, (double) 0, currentSeed, "start", 0,1,0);
}




void SetUpFolders(std::string base_dir,
				int numprocs,
				int numfilesatatime,
				  std::string templocation){
	
	
	std::string DataCollectedbase_dir = base_dir;
	DataCollectedbase_dir.append("/step2/DataCollected/");
	  
#ifdef timingstep2
	  std::string timingdir = base_dir;
	  timingdir.append("/timing/");
	if (boost::filesystem::exists(timingdir)){
		boost::filesystem::remove_all(timingdir);
	}
	  mkdirunix(timingdir.c_str());
#endif
	  

	  
	  
	//make text file with names of folders with data in them
    std::string mydirfname = base_dir;
    mydirfname.append("/folders");
    std::ofstream fout(mydirfname.c_str());
	if (!fout.is_open()){
		std::cerr << "failed to open " << mydirfname << " to write folder names!\n";
	}
	else{
		for (int i = 1; i < numprocs;++i){
		  std::stringstream tmpfolder;
		  tmpfolder << DataCollectedbase_dir;
		  tmpfolder << "c";
		  tmpfolder << i;
		  fout << tmpfolder.str() << (i!=numprocs-1 ? "\n" :  "");      
		}
	}
    fout.close();
}





//creates a string containing the file for step2 input file. note that the specific points at which we solve are written to the num.out file by each worker, so this is only written once.  then the associated .out files are created in a step2.1 solve, and left intact for the step2.2 solves, of which there are many.
std::string WriteStep2(std::vector<std::pair<double, double> > CurrentValues,
		       ProgSettings paramotopy_settings,
		       runinfo paramotopy_info){
  
  bool standardstep2;
  int sstep2 = paramotopy_settings.settings["MainSettings"]["standardstep2"].intvalue;
  if (sstep2 == 0){
    standardstep2 = false;
  }
  else{
    standardstep2 = true;
  }
	std::stringstream inputstringstream;
	inputstringstream << paramotopy_settings.WriteConfigStepTwo();
	inputstringstream << paramotopy_info.WriteInputStepTwo(CurrentValues, standardstep2);
	
	return inputstringstream.str();
}


//creates a string containing the file for step2 input file. note that the specific points at which we solve are written to the num.out file by each worker, so this is only written once.  then the associated .out files are created in a step2.1 solve, and left intact for the step2.2 solves, of which there are many.
std::string WriteFailStep2(std::vector<std::pair<double, double> > CurrentValues,
			   ProgSettings paramotopy_settings,
			   runinfo paramotopy_info){
	
  bool standardstep2;
  int sstep2 = paramotopy_settings.settings["MainSettings"]["standardstep2"].intvalue;
  if (sstep2 == 0){
    standardstep2 = false;
  }
  else{
    standardstep2 = true;
  }


	std::stringstream inputstringstream;
	
	inputstringstream << paramotopy_settings.WriteConfigFail();
	
	inputstringstream << paramotopy_info.WriteInputStepTwo(CurrentValues, standardstep2);
	
	return inputstringstream.str();
}



//constructs a file name for reading/writing
std::string MakeTargetFilename(std::string base_dir,
			       ToSave * TheFiles,
			       int index){
  std::stringstream ss;
  ss << base_dir
     << TheFiles[index].filename
     << TheFiles[index].filecount;
  return ss.str();
}





//SetFileCount is called initially.  guarantees that no old data will be overwritten.
void SetFileCount(ProgSettings & paramotopy_settings,
				  std::string DataCollectedBaseDir){

	settingmap::iterator iter;
	struct stat filestatus;
	
	for (iter=paramotopy_settings.settings["SaveFiles"].begin(); iter!=paramotopy_settings.settings["SaveFiles"].end();++iter){
		if ((*iter).second.intvalue==1) {//supposed to retrieve the integer 
			
			int filecount = 0;
			std::stringstream ss;
			ss << DataCollectedBaseDir
				<< (*iter).first
				<< filecount;
			while ( stat( ss.str().c_str(), &filestatus ) ==0) { //while can find a file of appropriate number
				ss.str("");
				ss.clear();
				++filecount;
				ss << DataCollectedBaseDir
					<< (*iter).first
					<< filecount;				
			}
			paramotopy_settings.settings["SaveFiles"][(*iter).first].filenumber=filecount;
		}
	}
}





