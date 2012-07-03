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
    for (int i = 1; i < numprocs;++i){
      std::stringstream tmpfolder;
      tmpfolder << DataCollectedbase_dir;
      tmpfolder << "c";
      tmpfolder << i;
      fout << tmpfolder.str() << (i!=numprocs-1 ? "\n" :  "");      
    }
    fout.close();
}






std::string WriteStep2(std::string config, 
					   std::vector<std::pair<double, double> > CValues,
					   std::vector<std::string> FunctVector, 
					   std::vector<std::string> VarGroupVector,
					   std::vector<std::string> ParamVector,
					   std::vector<std::string> ParamStrings,
					   std::vector<std::string> Consts,
					   std::vector<std::string> ConstantStrings,
					   std::vector<std::pair<double,double> > RandomValues,
					   int numfunct,
					   int numvar,
					   int numparam,
					   int numconsts){
  
	std::stringstream inputstringstream;
	
  MakeConfig(config, inputstringstream);
//	fout.precision(16);
  inputstringstream << "\n\nINPUT\n\n";
  // Variable Group portion
  inputstringstream << "variable ";  
  for (int i = 0; i < int(VarGroupVector.size());++i){
    inputstringstream << VarGroupVector[i] 
					  << ( i != int(VarGroupVector.size())-1? ",":";\n\n" );    
    
    
  }
  // constant portion for here and rand in the parameter homotopy
	inputstringstream << "constant ";
	for (int i = 0; i < int(ParamStrings.size());++i){
		inputstringstream << "rand" << ParamStrings[i]
						<< (i!= int(ParamStrings.size())-1?",":";\n");
	}
	inputstringstream << "constant ";
	for (int i = 0; i < int(ParamStrings.size());++i){
	inputstringstream << "here" << ParamStrings[i]
			<< (i!= int(ParamStrings.size()) -1?",":";\n");
	}


  // user given constants
  if (Consts.size()!=0){
    inputstringstream << Consts[0] << "\n";
  }

  // Define Path Variable and the parameter homotopies
  
  inputstringstream << "pathvariable t;\n"
       << "parameter ";
  for (int i =0;i < int(ParamStrings.size());++i){
    inputstringstream << ParamStrings[i] << (i!= int(ParamStrings.size())-1?",":";\n");
  }

  // Declare Functions
  MakeDeclareFunctions(inputstringstream,numfunct);
  
  MakeConstantsStep2(inputstringstream,
		     RandomValues,
		     CValues,
		     ParamStrings,
		     Consts,
		     ConstantStrings,
		     numparam);
  
  MakeFunctions(inputstringstream,FunctVector);
  inputstringstream << "END;\n";
  
	return inputstringstream.str();
}


void MakeConstantsStep2(std::stringstream & inputstringstream,
						std::vector< std::pair<double,double> > RandomValues,
						std::vector< std::pair<double,double> > CValues,
						std::vector<std::string> ParamStrings,
						std::vector<std::string> Consts,
						std::vector<std::string> ConstantStrings,
						int numparam){
  
  for (int i = 0; i < int(ParamStrings.size()); ++i){
    inputstringstream << "rand" 
	 << ParamStrings[i]
	 << " = "
	 << RandomValues[i].first << " + " << RandomValues[i].second
	 << "*I;\n";
  }

 for (int i = 0; i < int(ParamStrings.size()); ++i){
    inputstringstream << "here" 
	 << ParamStrings[i]
	 << " = "
	 << CValues[i].first << " + " << CValues[i].second
	 << "*I;\n";
  }
  for (int i = 0; i < int(ConstantStrings.size());++i){
    inputstringstream << ConstantStrings[i]
	 << "\n";
  }
  
  for (int i = 0; i < int(ParamStrings.size()); ++i){
    inputstringstream << ParamStrings[i]
	 << "= t*rand"
	 << ParamStrings[i]
	 << " + (1-t)*here"
	 << ParamStrings[i]
	 << ";\n";
  }
  
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





