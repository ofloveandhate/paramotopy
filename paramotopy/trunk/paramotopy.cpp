#include <iostream>
#include <ios>
#include <string>
#include <fstream>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <list>
#include <vector>
#include <map>
#include <sstream>
#include <cmath>
#include "mtrand.h"
#include "step1.h"
#include "random.h"
#include "step2.h"
#include "para_aux_funcs.h"
#include <mpi.h>
#include <unistd.h>


enum OPTIONS {Start, Input, SetRandom, SaveRandom, LoadRandom, WriteStepOne, RunStepOne, Step2,CollectData ,DetPrefs, Quit};




int main(int argC, char *args[]){
  
//	get_curr_dir_name();
	int prefversion = 12; //please increase this every time you modify the preferences function(s).  added 11.04.21 dab


	
	
  // Data members used throughout the program  

  OPTIONS currentChoice = Start;
  std::string filename;
  std::string base_dir ="bfiles_";
  bool parsed = false;
  std::ifstream fin;
  std::ifstream finconfig;
  std::ofstream fout;
  int numfunct;
  int numvar;
  int numparam;
  int numconsts;
//  MTRand drand(time(0));




  std::vector<std::string> FunctVector; 
  std::vector<std::string> VarGroupVector; 
  std::vector<std::string> ParamVector; 
  std::vector<std::string> ParamStrings;
  std::vector<std::string> Consts;
  std::vector<std::string> ConstantStrings;
  // values of points -- two different ways storing the data
  // whether or not we are user-defined or not
  // if doing a mesh, the data is contained as follows
  // -- first vector size is the number of parameters
  // -- second vector size the number of mesh points for the given param
  // -- pair is the real (first) and imaginary (second)
  
  // if not doing a mesh, the data is contained as follows
  // -- first vector size is number of sample n-ples)
  // -- second vector size is n
  // -- pair is the real (first) and imaginary (second)
  
  
  std::vector< std::vector< std::pair<double,double> > > Values;
  
  // random initial values
  // -- vector size is the number of parameters
  // -- pair is the real (first) and imaginary (second)
  std::vector< std::pair<double,double> > RandomValues;  
  std::vector< std::pair< std::pair< double, double >, std::pair< double, double > > > RandomRanges; 
  
  bool userdefined;
  std::string mytemp;
  std::ifstream fin3;
  std::ofstream foutdir;
  std::string tmpbase;
  std::string tmpstr;

  std::string dirfilename;
  std::string mcfname;  
  std::vector< int > NumMeshPoints;//added Nov16,2010 DAB



  std::stringstream myssmc;
  myssmc << " ";


	
  std::cout << "\n*******************************\n"
	    << "Welcome to the Paramotopy main program.\n\n";

	int numfilespossible = 8; // 
	std::vector< bool >  FilePrefVector;
	FilePrefVector.resize(numfilespossible-1);

	
	ToSave *TheFiles = new ToSave[numfilespossible];
	TheFiles[0].filename="real_solutions";
	TheFiles[0].saved=false;
	TheFiles[0].filecount=0;
	TheFiles[1].filename="nonsingular_solutions";
	TheFiles[1].saved=false;
	TheFiles[1].filecount=0;
	TheFiles[2].filename="singular_solutions";
	TheFiles[2].saved=false;
	TheFiles[2].filecount=0;
	TheFiles[3].filename="raw_data";
	TheFiles[3].saved=false;
	TheFiles[3].filecount=0;
	TheFiles[4].filename="raw_solutions";
	TheFiles[4].saved=false;
	TheFiles[4].filecount=0;
	TheFiles[5].filename="main_data";
	TheFiles[5].saved=false;
	TheFiles[5].filecount=0;
	TheFiles[6].filename="midpath_data";
	TheFiles[6].saved=false;
	TheFiles[6].filecount=0;
	TheFiles[7].filename="failed_paths";
	TheFiles[7].saved=true;
	TheFiles[7].filecount=0;
	
	preferences *Prefs = new preferences[1];
		Prefs[0].machinefile = "";
		Prefs[0].architecture = 0;
		Prefs[0].numprocs = -1;
		Prefs[0].usemachine = 0;
		Prefs[0].numfilesatatime = -1;
		Prefs[0].saveprogresseverysomany = 0;

	bool parallel = true;
	bool rerun = false;//for parallel preferences

	int currprefversion;
	currprefversion = GetPrefVersion();
	if (currprefversion!=prefversion) {
		rerun = true;
	}

	parallel = DeterminePreferences(Prefs, rerun, FilePrefVector);
	while (Prefs[0].numfilesatatime <= 0 ||  (Prefs[0].numprocs<=0)){
		rerun = true;
		parallel = DeterminePreferences(Prefs, rerun, FilePrefVector);
		rerun = false;
	}
	SetPrefVersion(prefversion);
	
	
	
	WriteShell1(Prefs[0].architecture, Prefs[0].usemachine);
	WriteShell1Parallel(Prefs[0].architecture, Prefs[0].usemachine);	

	

	
	

	

	bool finopened = false;//for parsing input file.
	bool suppliedfilename = true;
	bool finished = false;//for detecting already completed runs
	bool alreadytried = false;//for testing file openness if user supplied command-line filename

	
	if (argC==1) {
		suppliedfilename = false;
		std::cout << "\n\nBefore you begin any real work, you must choose an input file to parse.\n";
	}


  while(currentChoice!=Quit){
    
    if (!parsed){
		
		//open file
		while (!finopened) {
			if ( (!suppliedfilename) || alreadytried) {
				std::cout << "Enter the input file's name : ";
				std::cin >> filename;
			}
			else{
				std::stringstream commandss;
				for (int i=0;i<argC;++i){
					commandss << args[i] << " ";	
				}
				std::string garbage;
				commandss >> garbage;
				commandss >> filename;
			}
			
			
			  base_dir="bfiles_";
			  base_dir.append(filename);
			  fin.open(filename.c_str());
			if (fin.is_open()){
				finopened = true;
			}
			else {
				std::cout << "could not open a file of that name... try again.\n";
				alreadytried=true;
			}
		}
		finopened = false;//reset
		//end open file 
		
      ParseData(numfunct,numvar,numparam,numconsts,FunctVector,VarGroupVector,ParamVector,ParamStrings,Consts,ConstantStrings,Values,RandomValues,userdefined,fin,NumMeshPoints,filename);
      fin.close();
      
      std::cout << "Done parsing.";
      parsed=true;
      PrintRandom(RandomValues,ParamStrings);

		

		
		
		//check out previous runs...
		std::string blank;
		std::stringstream commandss;

		std::string lastoutfilename0 = base_dir;
		lastoutfilename0.append("/step2/lastnumsent0");	  
		// query whether had run or not yet. 
		fin.open(lastoutfilename0.c_str());
		std::vector< int > lastnumsent0;
		lastnumsent0.push_back(0);
		int tmpint;
		if (fin.is_open()){
			int lastoutcounter=1;
			for (int i=1; i<Prefs[0].numprocs; ++i) {
				getline(fin,blank);
				if (blank=="") {
					break;
				}
				commandss << blank;
				commandss >> tmpint;
				lastnumsent0.push_back(tmpint);
				lastoutcounter++;
				commandss.clear();
				commandss.str("");
			}
			
		}
		fin.close();
		
		std::string lastoutfilename1 = base_dir;
		lastoutfilename1.append("/step2/lastnumsent1");	  
		// query whether had run or not yet. 
		fin.open(lastoutfilename1.c_str());
		std::vector< int > lastnumsent1;
		lastnumsent1.push_back(0);
		if (fin.is_open()){
			int lastoutcounter=1;
			for (int i=1; i<Prefs[0].numprocs; ++i) {
				getline(fin,blank);
				if (blank=="") {
					break;
				}
				commandss << blank;
				commandss >> tmpint;
				lastnumsent1.push_back(tmpint);
				lastoutcounter++;
				commandss.clear();
				commandss.str("");
			}
			
		}
		fin.close();	
		

		
		
		std::string finishedfile = "bfiles_";
		finishedfile.append(filename);
		finishedfile.append("/finished");
		fin.open(finishedfile.c_str());
		if (fin.is_open()){
			finished = true;
		}
		fin.close();
		if (finished) {
			std::cout << "\n\n\nthis run appears done.  proceed anyway?  1 yes, 0 no.\n: ";
			int tmpint;
			
			std::cin >> tmpint;
			if (tmpint==0) {
				finished=true;
				currentChoice=Quit;
				
			}
			else {
				finished=false;
			}
		}		
		
		
		
		
		//if a previous run is recoverable...
		if ( (( int(lastnumsent1.size()) ==Prefs[0].numprocs) || ( int(lastnumsent0.size())==Prefs[0].numprocs))  && !finished ) {
			

			
			
			std::cout << "detected a previous run, which is recoverable.\nload previous random values?\n1 yes, 0 no.\n: ";
			int loadiemcloadster = -1;
			while ((loadiemcloadster<0) || (loadiemcloadster>1)) {
				std::cin >> loadiemcloadster;
			}
			
			if (loadiemcloadster==1) {
				std::string randfilename=base_dir;
				randfilename.append("/randstart");
				
				fin.open(randfilename.c_str());

				int ccount=0;
				std::cout << "\n\n";
				while(getline(fin,mytemp)){
					std::stringstream myss;
					myss << mytemp;
					double crandreal;
					double crandimaginary;
					myss >> crandreal;
					myss >> crandimaginary;
					RandomValues[ccount].first = crandreal;
					RandomValues[ccount].second = crandimaginary;
					std::cout << ParamStrings[ccount]
					<< " = "
					<< RandomValues[ccount].first 
					<< " + "
					<< RandomValues[ccount].second
					<< "*I\n";
					++ccount;
				}
				std::cout << "\n";
				fin.close();
			}
		}
		//done reloading previous random values
		
		
		
    }
  
    

    std::string randfilename;

	int intChoice=0;
	  
	  if (finished) {
	    intChoice=9;
	  }
	  else {
		  intChoice = GetUserChoice(intChoice);
	  }
    
	  intChoice = GetUserChoice(intChoice);


    switch (intChoice){
    case 1 :
			
      currentChoice = Input;
			finopened = false;
			while (!finopened) {
				
			
			    std::cout << "Enter in the input file's name : ";
			    std::cin >> filename;
			    base_dir="bfiles_";
			    base_dir.append(filename);
			    fin.close();
			    fin.open(filename.c_str());
				if (fin.is_open()) {
					finopened = true;
				}
			}
			finopened = false;
      ParseData(numfunct,numvar,
		numparam,numconsts,FunctVector,
		VarGroupVector,ParamVector,
		ParamStrings,
		Consts,
		ConstantStrings,
		Values,
		RandomValues,
		userdefined,
		fin,
		NumMeshPoints,
		filename);
      fin.close();
      parsed=true;
	
      PrintRandom(RandomValues,ParamStrings);
      break;
    case 2:
			
			currentChoice = SetRandom;
			RandomValues = random_case(RandomValues, ParamStrings);			
			break;
      
			
			
    case 3: // Save Random points
			
			currentChoice = SaveRandom;
			save_random_case(RandomValues, fout, numparam);
			break;
					
					
			
			
	case 4: // Load Random points
			
			currentChoice = LoadRandom;
			load_random_case(fin3, RandomValues,ParamStrings);
			break; 
		
			
    case 5:         // Write Step1
      currentChoice=WriteStepOne;
	
      
      WriteStep1(filename, 
		 "config1", 
		 FunctVector,
		 ParamStrings, 
		 VarGroupVector, 
		 Consts, 
		 ConstantStrings,
		 RandomValues);
      

      
      std::cout << "Writing Step 1 done ... \n";
      

      break;
			
			
			
			
			
			
			
			
			
			
			
			
			
    case 6: 
		currentChoice = RunStepOne;
		WriteStep1(filename, 
					   "config1", 
					   FunctVector,
					   ParamStrings, 
					   VarGroupVector, 
					   Consts, 
					   ConstantStrings,
					   RandomValues);
			
			
			
			if (parallel) {
				CallBertiniStep1Parallel(base_dir,Prefs[0].machinefile,Prefs[0].numprocs);
			}
			else {
				CallBertiniStep1(base_dir);
			}

      
      break;
      
			
			
			
			
    case 7:
      
			////////////////////////
			//
			//
			//     begin choice 7, step2
			//
			//
			////////////////////////////
			
			
			
				
		currentChoice = Step2;
		for (int i =0; i<numfilespossible-1; ++i) {
			TheFiles[i].saved = FilePrefVector[i];
		}     
		step2_case(numfilespossible,
				   TheFiles,
				   filename,
				   parallel,
				   Prefs,
				   numparam,
				   RandomValues,
				   ParamStrings);
		break;
		
		
					
		/////////////////////////
			//
			//
			//     end choice 7
			//
			//
		//////////////////////////
			
			
		
    //end choice 7: step2
			
			
			
			

			
			
			
	case 8:
		currentChoice=DetPrefs;
			rerun = true;
			FilePrefVector.clear();
			parallel =  DeterminePreferences(Prefs, rerun, FilePrefVector);
			SetPrefVersion(prefversion);
			rerun = false;
			
			WriteShell1(Prefs[0].architecture, Prefs[0].usemachine);
			WriteShell1Parallel(Prefs[0].architecture, Prefs[0].usemachine);	
			// Write the shell script

			//WriteShell3(architecture);
			break;
			
    case 9:
      currentChoice=Quit;	    
      std::cout << "Quitting\n\n";
    }//re: switch (intChoice)
    
  }//re: while(currentChoice!=Quit)
  
  return 0;
}//re: main function
