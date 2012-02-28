#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <sstream>
#include "step1.h"
#include "step2.h"
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <cmath>
#include <sys/types.h> 




void SetUpFolders(std::string base_dir,
			 int numprocs,
			 int numfilesatatime){
	
	
	std::string filenamestep2=base_dir;
	filenamestep2.append("/step2/tmp/");
	std::string DataCollectedbase_dir = base_dir;
	DataCollectedbase_dir.append("/step2/DataCollected/");
	  
#ifdef timingstep2
	  std::string timingdir = base_dir;
	  timingdir.append("/timing/");
	  mkdirunix(timingdir.c_str());
#endif
	  
	std::cout << "setting up folders, " << numprocs << " procs.\n";
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






void WriteStep2(std::vector< std::string > configvector, 
		std::ofstream & fout,
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
  
  MakeConfig(configvector, fout);
	fout.precision(16);
  fout << "\n\nINPUT\n\n";
  // Variable Group portion
  fout << "variable ";  
  for (int i = 0; i < int(VarGroupVector.size());++i){
    fout << VarGroupVector[i] 
	 << ( i != int(VarGroupVector.size())-1? ",":";\n\n" );    
    
    
  }
  // constant portion for here and rand in the parameter homotopy
	fout << "constant ";
	for (int i = 0; i < int(ParamStrings.size());++i){
		fout << "rand" << ParamStrings[i]
		<< (i!= int(ParamStrings.size())-1?",":";\n");
	}
	fout << "constant ";
	for (int i = 0; i < int(ParamStrings.size());++i){
	fout << "here" << ParamStrings[i]
	 << (i!= int(ParamStrings.size()) -1?",":";\n");
	}


  // user given constants
  if (Consts.size()!=0){
    fout << Consts[0] << "\n";
  }

  // Define Path Variable and the parameter homotopies
  
  fout << "pathvariable t;\n"
       << "parameter ";
  for (int i =0;i < int(ParamStrings.size());++i){
    fout << ParamStrings[i] << (i!= int(ParamStrings.size())-1?",":";\n");
  }

  // Declare Functions
  MakeDeclareFunctions(fout,numfunct);
  
  MakeConstantsStep2(fout,
		     RandomValues,
		     CValues,
		     ParamStrings,
		     Consts,
		     ConstantStrings,
		     numparam);
  
  MakeFunctions(fout,FunctVector);
  fout << "END;\n";
  
  
}


void MakeConstantsStep2(std::ofstream & fout,
			std::vector< std::pair<double,double> > RandomValues,
		        std::vector< std::pair<double,double> > CValues,
			std::vector<std::string> ParamStrings,
			std::vector<std::string> Consts,
			std::vector<std::string> ConstantStrings,
			int numparam){
  
  for (int i = 0; i < int(ParamStrings.size()); ++i){
    fout << "rand" 
	 << ParamStrings[i]
	 << " = "
	 << RandomValues[i].first << " + " << RandomValues[i].second
	 << "*I;\n";
  }

 for (int i = 0; i < int(ParamStrings.size()); ++i){
    fout << "here" 
	 << ParamStrings[i]
	 << " = "
	 << CValues[i].first << " + " << CValues[i].second
	 << "*I;\n";
  }
  for (int i = 0; i < int(ConstantStrings.size());++i){
    fout << ConstantStrings[i]
	 << "\n";
  }
  
  for (int i = 0; i < int(ParamStrings.size()); ++i){
    fout << ParamStrings[i]
	 << "= t*rand"
	 << ParamStrings[i]
	 << " + (1-t)*here"
	 << ParamStrings[i]
	 << ";\n";
  }
  
}

//void CallCopyStartStep2(std::string base_dir, std::string param_dir){
//
//  std::string mystr = "./cpstartstep2.sh";
//
//  mystr.append(" ");
//  mystr.append(base_dir);
//  mystr.append(" ");
//  mystr.append(param_dir);
//  system(mystr.c_str());
//
//}




//antiquated october 13, 2011 daniel brake
//void CallBertiniStep2(std::string param_dir){
//
////	std::string home = getenv("HOME");
//  
////	std::string mystr = home;
//	std::string mystr = "sh .";
//	mystr.append("/callbertinistep2.sh ");	
//	mystr.append(param_dir);
//	
//#ifdef verbosestep2
//	std::cout << "calling the shell script as the following: \n"
//	    << mystr << "\n";
//#endif
//	system(mystr.c_str());
//}






std::string MakeTargetFilename(std::string base_dir,
			       ToSave * TheFiles,
			       int index){
  std::stringstream ss;
  
	
	
  ss << "../../../../"
	 << base_dir
     << TheFiles[index].filename
     << TheFiles[index].filecount;
  return ss.str();


}


// assumes base_dir ends in a '/'
void TouchFilesToSave(ToSave *TheFiles,int numfilespossible,
		      std::string base_dir){
  //std::cout << "Entering TouchFilesToSave(Blah)\n";
  for (int i = 0; i < numfilespossible;++i){
    
    // File Count has been set correctly already
    // touch the appropriate files so no yelling ...
    if (TheFiles[i].saved){
      std::string command = "touch ";
      //      int filecount = TheFiles[i].filecount;
      std::stringstream thefilename;
      thefilename << base_dir
		  << TheFiles[i].filename
		  << TheFiles[i].filecount;
      command.append(thefilename.str());
      
    }
  }

}


//SetFileCount is called initially.  guarantees that no old data will be overwritten.
void SetFileCount(ToSave *TheFiles, int numfiles,
		  std::string DataCollectedBaseDir){

	
  for (int i = 0; i < numfiles;++i){
    if (TheFiles[i].saved){
      int cfilecount = 0;
      std::stringstream ss;
      ss << DataCollectedBaseDir
		 << TheFiles[i].filename
		 << cfilecount;
      // while we can open the file ...
      std::ifstream fin(ss.str().c_str());
      while(fin.is_open()){
		ss.str("");
		ss.clear();
		++cfilecount;
		ss << DataCollectedBaseDir
		   << TheFiles[i].filename
		   << cfilecount;
		fin.close();
		fin.open(ss.str().c_str());
      }
      TheFiles[i].filecount=cfilecount;
    }    
  }
}


