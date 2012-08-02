#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <sstream>
#include "step1_funcs.h"
#include "step2_funcs.h"
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <cmath>
#include <sys/types.h> 




void SetUpFolders(std::string base_dir,
				int numprocs,
				int numfilesatatime,
				  std::string templocation){
	
	
	std::string filenamestep2=templocation;
	filenamestep2.append("/");
	filenamestep2.append(base_dir);
	filenamestep2.append("/step2/tmp/");
	std::string DataCollectedbase_dir = base_dir;
	DataCollectedbase_dir.append("/step2/DataCollected/");
	  
#ifdef timingstep2
	  std::string timingdir = base_dir;
	  timingdir.append("/timing/");
	  mkdirunix(timingdir.c_str());
#endif
	  
	std::cout << "setting up temp folders, " << numprocs << " procs.\n";
	  //make the tmp folders.  
	  for (int i=1; i<numprocs; ++i) { //everybody gets one
		  std::stringstream ss;
		  ss <<filenamestep2 << i;
		  mkdirunix(ss.str().c_str());//make the folder for the run.
		  ss.clear();
		  ss.str("");
	  }
	  
	  
	//make text file with names of folders with data in them
    std::string mydirfname = base_dir;
    mydirfname.append("/folders");
    std::ofstream fout(mydirfname.c_str());
	if (!fout.is_open()){
		std::cerr << "failed to open " << mydirfname << "\n";
	}
    for (int i = 1; i < numprocs;++i){
      std::stringstream tmpfolder;
      tmpfolder << DataCollectedbase_dir;
      tmpfolder << "c";
      tmpfolder << i;
      fout << tmpfolder.str() << (i!=numprocs-1 ? "\n" :  "");      
    }
    fout.close();
}





//creates a string containing the file for step2 input file. note that the specific points at which we solve are written to the num.out file by each worker, so this is only written once.  then the associated .out files are created in a step2.1 solve, and left intact for the step2.2 solves, of which there are many.
std::string WriteStep2(std::vector<std::pair<double, double> > CValues,
					   ProgSettings paramotopy_settings,
					   runinfo paramotopy_info){
  
	std::stringstream inputstringstream;
	

	inputstringstream << paramotopy_settings.WriteConfigStepTwo();
	inputstringstream << paramotopy_info.WriteInputStepTwo(CValues);
	
	

	return inputstringstream.str();
}


//creates a string containing the file for step2 input file. note that the specific points at which we solve are written to the num.out file by each worker, so this is only written once.  then the associated .out files are created in a step2.1 solve, and left intact for the step2.2 solves, of which there are many.
std::string WriteFailStep2(std::vector<std::pair<double, double> > CValues,
					   ProgSettings paramotopy_settings,
					   runinfo paramotopy_info){
	
	std::stringstream inputstringstream;
	
	
	inputstringstream << paramotopy_settings.WriteConfigFail();
	inputstringstream << paramotopy_info.WriteInputStepTwo(CValues);
	
	
	
	return inputstringstream.str();
}




std::string MakeTargetFilename(std::string base_dir,
							   ToSave * TheFiles,
							   int index){
  std::stringstream ss;
  
	
	
  ss << base_dir
     << TheFiles[index].filename
     << TheFiles[index].filecount;
  return ss.str();


}


// assumes base_dir ends in a '/'
void TouchFilesToSave(ProgSettings paramotopy_settings,
					  std::string base_dir){
	
	settingmap::iterator iter;
	
	for (iter=paramotopy_settings.settings["SaveFiles"].begin(); iter!=paramotopy_settings.settings["SaveFiles"].end();++iter){
		// File Count has been set correctly already
		// touch the appropriate files so no yelling ...
		if ((*iter).second.intvalue==1){
			std::stringstream command;
			command << "touch ";
			command << base_dir
				<< (*iter).first
				<< paramotopy_settings.settings["SaveFiles"][(*iter).first].filenumber;
			system(command.str().c_str());
		}
	}
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





