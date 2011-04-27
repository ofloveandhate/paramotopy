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
#define verbosestep2
#include <sys/types.h> 

void WriteStep2(std::vector< std::string > configvector, 
		std::ofstream & fout,
		std::vector<std::pair<float, float> > CValues,
		std::vector<std::string> FunctVector, 
		std::vector<std::string> VarGroupVector,
		std::vector<std::string> ParamVector,
		std::vector<std::string> ParamStrings,
		std::vector<std::string> Consts,
		std::vector<std::string> ConstantStrings,
		std::vector<std::pair<float,float> > RandomValues,
		int numfunct,
		int numvar,
		int numparam,
		int numconsts){
  
  MakeConfig(configvector, fout);
	fout.precision(20);
  fout << "\n\nINPUT\n\n";
  // Variable Group portion
  fout << "variable ";  
  for (int i = 0; i < VarGroupVector.size();++i){
    fout << VarGroupVector[i] 
	 << ( i != VarGroupVector.size()-1? ",":";\n\n" );    
    
    
  }
  // constant portion for here and rand in the parameter homotopy
  fout << "constant ";
  for (int i = 0; i < ParamStrings.size();++i){
    fout << "here" << ParamStrings[i]
	 << (i!=ParamStrings.size()-1?",":";\n");
  }
  fout << "constant ";
  for (int i = 0; i < ParamStrings.size();++i){
    fout << "rand" << ParamStrings[i]
	 << (i!=ParamStrings.size()-1?",":";\n");
  }

  // user given constants
  if (Consts.size()!=0){
    fout << Consts[0] << "\n";
  }

  // Define Path Variable and the parameter homotopies
  
  fout << "pathvariable t;\n"
       << "parameter ";
  for (int i =0;i < ParamStrings.size();++i){
    fout << ParamStrings[i] << (i!=ParamStrings.size()-1?",":";\n");
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
			std::vector< std::pair<float,float> > RandomValues,
		        std::vector< std::pair<float,float> > CValues,
			std::vector<std::string> ParamStrings,
			std::vector<std::string> Consts,
			std::vector<std::string> ConstantStrings,
			int numparam){
  
  for (int i = 0; i < ParamStrings.size(); ++i){
    fout << "rand" 
	 << ParamStrings[i]
	 << " = "
	 << RandomValues[i].first << " + " << RandomValues[i].second
	 << "*I;\n";
  }

 for (int i = 0; i < ParamStrings.size(); ++i){
    fout << "here" 
	 << ParamStrings[i]
	 << " = "
	 << CValues[i].first << " + " << CValues[i].second
	 << "*I;\n";
  }
  for (int i = 0; i < ConstantStrings.size();++i){
    fout << ConstantStrings[i]
	 << "\n";
  }
  
  for (int i = 0; i < ParamStrings.size(); ++i){
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





void CallBertiniStep2(std::string param_dir){

//	std::string home = getenv("HOME");
  
//	std::string mystr = home;
	std::string mystr = "sh .";
	mystr.append("/callbertinistep2.sh ");	
	mystr.append(param_dir);
	
#ifdef verbosestep2
	std::cout << "calling the shell script as the following: \n"
	    << mystr << "\n";
#endif
	system(mystr.c_str());
}




void WriteData(int runid, 
	       std::string orig_file, 
	       std::string target_file,
	       std::vector<std::string> ParamStrings,
	       std::vector<std::pair<float, float> > CValues){

  std::string cline;
  
  std::ifstream fin(orig_file.c_str());
  std::ofstream fout;
  // test if file target file is open

  std::ifstream fintarget(target_file.c_str());
  if (fintarget.is_open()){
    fintarget.close();
    fout.open(target_file.c_str(),std::ios::app);
  }
  else{
    fintarget.close();
    fout.open(target_file.c_str());
    
    for (int i = 0; i < ParamStrings.size();++i){
      fout << ParamStrings[i] << (i != ParamStrings.size()-1? " ": "\n");
    }
  }
  fout << runid << "\n";
  for (int i = 0; i < CValues.size();++i){
    fout << CValues[i].first << " " << CValues[i].second << " ";
  }
  fout << "\n";
  while(getline(fin,cline)){
    fout << cline << "\n";
  }
  fin.close();
  fout.close();
}



std::string MakeTargetFilename(std::string base_dir,
			       ToSave * TheFiles,
			       int index){
  std::stringstream ss;
  
	
  ss << base_dir
     << TheFiles[index].filename
     << TheFiles[index].filecount;
  return ss.str();
  /*
  int currentfilebytecount;
  int currentbytecount;
  
  std::string wccommand = "wc -c ";
  wccommand.append(base_dir);
  wccommand.append(filename);
  ss << wccommand;
  ss << currentfilecount;
  wccommand=ss.str();
  ss.str("");
  ss.clear();
  wccommand.append(" > ./");
  wccommand.append(tmp_base_dir);
  wccommand.append("tmpbytecounttargetfile");
  //  std::cout  << "wc command1 = " << wccommand << "\n";
  system(wccommand.c_str());

  // load the currentbytecount into memory
  std::string tmpfileloc="./";
  tmpfileloc.append(tmp_base_dir);
  tmpfileloc.append("tmpbytecounttargetfile");
  
  std::ifstream fin(tmpfileloc.c_str());

  std::string cline;
  getline(fin,cline);
  fin.close();
  // put the line on a string stream buffer
  
  
  ss << cline;
  ss >> currentbytecount;
  std::string thefile;
  ss >> thefile;
  ss.str("");
  ss.clear();
  
  // say that our max size of any given data storage file is 100 megs
  // or 100 * 2^10 * 2^10 
  
  // load  the currentfilebytecount into memory
  
  std::string wccommand2="wc -c ";
  wccommand2.append(tmp_base_dir);
  //  wccommand2.append("/");
  wccommand2.append(tmpfilename);
  wccommand2.append(" > ./");
  wccommand2.append(tmp_base_dir);
  wccommand2.append("tmpbytecount");

  //  std::cout  << "wc command2 = " << wccommand2 << "\n";
  //  wccommand2.append("tmpbytecount");
  system(wccommand2.c_str());
  std::string fileloc2="./";
  fileloc2.append(tmp_base_dir);
  fileloc2.append("tmpbytecount");
  
  fin.open(fileloc2.c_str());
  getline(fin,cline);
  ss << cline;
  ss >> currentfilebytecount;
  std::string thefile2;
  ss >> thefile2;
  ss.str("");
  ss.clear();
  int maxsize = int(100*double(pow(double(2),double(20))));

  std::string returnstring;
 
  if (maxsize > currentfilebytecount+currentbytecount){
    // ss << base_dir;
    ss << tmpfilename;
    ss << currentfilecount;
    returnstring=ss.str();
    if (currentbytecount)
      append=true;
    else
      append=false;

  }
  else{
    if(currentbytecount){
      ++currentfilecount;
    }
    // ss << base_dir;
    ss << filename;
    ss << currentfilecount;

    if (currentbytecount){
      append=true;
    }
    else{
      append=false;
    }

    returnstring=ss.str();
  }
  //  std::cout << "append = " << (append? "true\n":"false\n");  
  return returnstring;
  */

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
  //std::cout << "Leaving TouchFilesToSave(Blah)\n";
  /* 
    std::string command = "touch ";
    int filecount=TheFiles[i].filecount;
    std::stringstream thefilename;
    thefilename << base_dir
      //		  << "n"
      //		  << j
      //		  << "/"
		<< TheFiles[i].filename
		<< TheFiles[i].filecount;
    //   std::cout << "filecount = " << filecount << "\n";
    std::ifstream toopen(thefilename.str().c_str());
    bool empty = false;
    while(toopen.is_open() && !empty){
      // test if the file is empty
      std::string wccommand = "wc -c ";
      wccommand.append(thefilename.str());
      wccommand.append(" > ./");
      //      wccommand.append(base_dir);
      wccommand.append("tmpbytecount");
      system(wccommand.c_str());
      std::string tmpfileloc = "";
      tmpfileloc.append("./tmpbytecount");
      std::ifstream fin2(tmpfileloc.c_str());
      int bytecount;
      fin2 >> bytecount;
      std::cout << "In TouchFilesToSave : wccommand = "
		<< wccommand << ", tmpfileloc = " << tmpfileloc
		<< "\n";
      std::cout << "tmpfileloc.is_open() : "
		<< (fin2.is_open()?"true\n" : "false\n");
      std::cout << "In TouchFilesToSave : bytecount = "
		<< bytecount << "\n";
      fin2.close();
      system("rm tmpbytecount");
      if (bytecount==0){
	empty=true;
      }
      else{
	//   std::cout << "The file : " << thefilename.str() << " is open.\n";
	thefilename.str("");
	thefilename.clear();
	filecount++;
	thefilename << base_dir 
	  //    << "n"
	  //	    << j
	  //	    << "/"
		    << TheFiles[i].filename
		    << filecount;
	toopen.close();
	toopen.open(thefilename.str().c_str());
      }
    }
    if (TheFiles[i].saved){
      std::cout << "Inside TouchFilesToSave(blah)\n";
      std::cout << "Setting the filecount to " << filecount << "\n";
      TheFiles[i].filecount = filecount;
      
      std::cout << "The file : " << thefilename.str() << " is not open.\n";
    }
    if(TheFiles[i].saved){
      //      command.append(base_dir);
      command.append(thefilename.str());
      system(command.c_str());
    }    
  } 

  */ 
}



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


void UpdateFileCount(ToSave *TheFiles, int numfiles,
		     std::string DataCollectedBaseDir){



	std::string tmpbytecountfilename = DataCollectedBaseDir;//has the trailing backslash attached
	tmpbytecountfilename.append("tmpbytecount");
	std::string rmsystemcall = "rm ";
	rmsystemcall.append(tmpbytecountfilename);
	
	
  for (int i = 0; i < numfiles;++i){
    std::string thefilename = DataCollectedBaseDir;
    if (TheFiles[i].saved){
      thefilename.append(TheFiles[i].filename);
      std::stringstream ss;
      ss << TheFiles[i].filecount;
      thefilename.append(ss.str());
      // test if file can be open to begin with ..
      std::ifstream fin(thefilename.c_str());

      if(fin.is_open()){

		fin.close();
		std::string wccommand = "wc -c ";
		wccommand.append(thefilename);
		wccommand.append(" > ");
		wccommand.append(tmpbytecountfilename);
		system(wccommand.c_str());
		// open the tmpbytecount
		fin.open(tmpbytecountfilename.c_str());
		int bytecount;
		fin >> bytecount;
		if (bytecount > 67108864){//64*int(pow(double(2),double(20))) = 67108864
			
			TheFiles[i].filecount+=1;
		}

		fin.close();
//		system(rmsystemcall.c_str());// i decided against the extra system call.  just overwrite the file every time.
      }
    }


  }//re: for loop
  return;
}
