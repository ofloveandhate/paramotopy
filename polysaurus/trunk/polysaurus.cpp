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
#include <mpi.h>
#include <unistd.h>


enum OPTIONS {Start, Input, SetRandom, Step1, Step2,CollectData ,DetParallel, Quit};




int main(int argC, char *args[]){
  
//	get_curr_dir_name();
	int prefversion = 11; //please increase this every time you modify the preferences function(s).  added 11.04.21 dab


	
	
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
  MTRand drand(time(0));

	
	std::string path;
	system("pwd > path.out");
	fin.open("path.out");
	fin >> path;
	
	std::cout << path << "\n";
	fin.close();
	
	
  int myid,num_processes, headnode=0;



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
  
  
  std::vector< std::vector< std::pair<float,float> > > Values;
  
  // random initial values
  // -- vector size is the number of parameters
  // -- pair is the real (first) and imaginary (second)
  std::vector< std::pair<float,float> > RandomValues;  
  std::vector< std::pair< std::pair< float, float >, 
    std::pair< float, float > > >
    RandomRanges; 
  
  bool userdefined;
  std::string mytemp;
  std::string randpointfilename;
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
	    << "Welcome to the Polysaurus main program.\n\n";

	int numfilespossible = 8;
	std::vector< bool >  FilePrefVector;
	FilePrefVector.resize(numfilespossible);

	
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
	TheFiles[7].saved=false;
	TheFiles[7].filecount=0;
	
	
	
	int architecture = 0;
	int numprocs = 0;
	int namelen, saveprogresseverysomany;	
	bool parallel = true;
	std::string machinefile = "";
	
	
	
	bool rerun = false;//for parallel preferences

	int currprefversion;
	currprefversion = GetPrefVersion();
	if (currprefversion<prefversion) {
		rerun = true;
	}
	
	int usemachine = 0;
	int numfilesatatime = 0;
	parallel = DeterminePreferences(architecture, usemachine, machinefile, numprocs, rerun, numfilesatatime,FilePrefVector,saveprogresseverysomany);
	while (numfilesatatime == 0 ||  (numprocs==0)){ //(parallel && (machinefile=="")) ||  antiquated option dab 4/25/11
		rerun = true;
		parallel = DeterminePreferences(architecture, usemachine, machinefile, numprocs, rerun, numfilesatatime,FilePrefVector,saveprogresseverysomany);
		rerun = false;
	}
	SetPrefVersion(prefversion);
	
	WriteShell1(architecture, usemachine);
	WriteShell1Parallel(architecture, usemachine);	
	// Write  the copy shell script, running script
//	WriteShell2();
	WriteShell3(architecture);
	

	
	

	

	bool finopened = false;//for parsing input file.
	bool suppliedfilename = true;

	
	
	
	if (argC==1) {
		suppliedfilename = false;
		std::cout << "\n\nBefore you begin any real work, you must choose an input file to parse.\n";
	}





	bool finished = false;//for detecting already completed runs
	bool alreadytried = false;//for testing file openness if user supplied command-line filename


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
				alreadytried=true;
			}
		}
		finopened = false;//reset
		//end open file 
		
      ParseData(numfunct,
		numvar,
		numparam,
		numconsts,
		FunctVector,
		VarGroupVector,
		ParamVector,
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
      
      std::cout << "Done parsing.";
      parsed=true;
      PrintRandom(RandomValues,ParamStrings);

		
		//used to write mc file here.  now is done on the fly during step2.
		
		
		
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
			for (int i=1; i<numprocs; ++i) {
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
			for (int i=1; i<numprocs; ++i) {
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
		if ( ((lastnumsent1.size()==numprocs) || (lastnumsent0.size()==numprocs))  && !finished ) {
			

			
			
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
					float crandreal;
					float crandimaginary;
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
    int ccount=0;
    int intChoice;
	  
	  if (finished) {
	    intChoice=9;
	  }
	  else {
		std::cout << "\n\nHere are your choices : \n"
	      << "1) Parse an appropriate input file. \n"
	      << "2) Randomize start points. \n"
	      << "3) Save random start points. \n"
	      << "4) Load random start points. \n"
	      << "5) Write Step 1.\n"
	      << "6) Run Step 1.\n"
	      << "7) Run Step 2.\n"
		  << "8) Determine Preferences for this machine.\n"
	      << "9) Quit the program.\n\n"
	      << "Enter the integer value of your choice : ";
		std::cin >> intChoice;
		std::cout << "\n\n";
	  }
    
    while (intChoice < 1 || intChoice > 9){
      
      std::cout << "\n\nHere are your choices : \n"
		<< "1) Parse an appropriate input file. \n"
		<< "2) Randomize start points. \n"
		<< "3) Save random start points. \n"
		<< "4) Load random start points. \n"
		<< "5) Write Step 1.\n"
		<< "6) Run Step 1.\n"
		<< "7) Run Step 2.\n"
		<< "8) Determine Preferences for this machine.\n"
		<< "9) Quit the program.\n\n"
		<< "Enter the integer value of your choice : "; 
      
      std::cin >> intChoice;
      std::cout << "\n\n";	
    }
    
    
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
//      if(!userdefined){
//			// rewrite the mesh values into a Monte Carlo - like file
//			// in the folder of bfiles_filename
//			
//			mcfname = base_dir;
//			mkdirunix(mcfname.c_str());
//			mcfname.append("/mc");
//			std::cout << "mcfname = " << mcfname << "\n";
//			std::string cline = "";
//			std::string rm = "rm ";
//			rm.append(mcfname);
//			system(rm.c_str());
//			WriteMeshToMonteCarlo(0, Values, mcfname,cline);
//      }
      
      // copy the montecarlo file to bfiles_filename/mc
      
      
	
      PrintRandom(RandomValues,ParamStrings);
      break;
    case 2:
      
      int randchoice;
      std::cout << "1) Default range [0 to 1)\n"
		<< "2) User-specified range\n"
		<< "Enter in if you would like to use the standard "
		<< "default range for random floats or if you would "
		<< "like to specify the range : ";
      std::cin >> randchoice;
      while (randchoice != 1 && randchoice != 2){
			std::cout << "Please enter 1 for the default range or 2 for"
				  << " a user-specified range : ";
			std::cin >> randchoice;
      }
      if (randchoice == 1){
			RandomValues = MakeRandomValues(ParamStrings.size());
      }
      if (randchoice == 2){
		
			  int paramrandchoice = 1;	
			  while (paramrandchoice != 0){ 
			  std::cout << "0 - Done specifying random values\n";
			  for (int i = 0; i < ParamStrings.size();++i){
				std::cout << i+1 << " - " << ParamStrings[i]
					  << "\n";
		  }
		  std::cout << "Enter the parameter you want to rerandomize : ";
		  std::cin >> paramrandchoice;
		  while (paramrandchoice < 0 || paramrandchoice > ParamStrings.size()){
			
			std::cout << "0 - Done specifying random values\n";
			for (int i = 0; i < ParamStrings.size();++i){
				  std::cout << i+1 << " - " << ParamStrings[i]
					<< "\n";
			}
			std::cout << "Enter the parameter you want to rerandomize : ";
			std::cin >> paramrandchoice;
		  }
		  if (paramrandchoice !=0){
			std::cout << "Enter a low range followed "
				  << "by a high range for the"
				  << " real and imaginary parts "
				  << "of the chosen parameter."
				  << "\n";
			
			float creallow;
			float crealhigh;
			float cimaginarylow;
			float cimaginaryhigh;
			std::cout << "\n" << "Real Low : ";
			std::cin >> creallow;
			std::cout << "Real High : ";
			std::cin >> crealhigh;
			std::cout << "Imaginary Low : ";
			std::cin >> cimaginarylow;
			std::cout << "Imaginary High : ";
			std::cin >> cimaginaryhigh;	  
			float crandreal = drand();
			float crandimaginary = drand();
			crandreal*=(crealhigh-creallow);
			crandimaginary*=(cimaginaryhigh-cimaginarylow);
			crandreal+=creallow;
			crandimaginary+=cimaginarylow;
			RandomValues[paramrandchoice-1].first=crandreal;
			RandomValues[paramrandchoice-1].second=crandimaginary;
		  }
		}
      }
      PrintRandom(RandomValues,ParamStrings);//in random.cpp
      break;
      
    case 3: // Save Random points
      
			  
			  std::cout << "Enter the filename you want to save the"
				<< " random start points to : ";
			  std::cin >> randpointfilename;
			  
			fout.open(randpointfilename.c_str());
			
			for (int i = 0; i < numparam; ++i){
				  fout << RandomValues[i].first << " "
					   << RandomValues[i].second << "\n";
			}
			fout.close();
			
			break;
					
					
			
			
      case 4: // Load Random points
			
			std::cout << "Enter the filename of the random points you"
				  << " want to load : ";
			std::cin >> randfilename;
			
			fin3.open(randfilename.c_str());
			
			  
			ccount=0;
			std::cout << "\n\n";
			while(getline(fin3,mytemp)){
			  std::stringstream myss;
			  myss << mytemp;
			  float crandreal;
			  float crandimaginary;
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
			fin3.close();
			break; 
			
			
			
			
			
			
			
			
			
			
    case 5:         // Write Step1
      currentChoice=Step1;
	
      
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
      
      std::cout << "base_dir = " <<  base_dir << "\n";
			if (parallel) {
				CallBertiniStep1Parallel(base_dir,machinefile,numprocs);
			}
			else {
				CallBertiniStep1(base_dir);
			}

      
      break;
      
			
			
			
			
			
			
			
			
			
			
			////////////////////////
			//
			//
			//     begin choice 7, step2
			//
			//
			////////////////////////////
			
			
			
			
			
			
			
    case 7:{
      
		
	
		
		for (int i =0; i<numfilespossible; ++i) {
			TheFiles[i].saved = FilePrefVector[i];
		}      
      
      // open the bfiles_filename/mc file that contains the monte carlo points
      // in order in accordance to the order of the paramstrings in ParamStrings
      

		
      fin.close();//just in case??  db
		
		base_dir = "bfiles_";
		base_dir.append(filename);
		
		
		
	// make the tmp files directory/folder
		std::string curline;	
		std::string filenamestep2="bfiles_";
		filenamestep2.append(filename);
		filenamestep2.append("/step2/tmp/");
	    mkdirunix(filenamestep2.c_str());
		
		
		

		
		//make the DataCollected directory.
      std::string DataCollectedBaseDir=base_dir;
      DataCollectedBaseDir.append("/step2/DataCollected/");
      mkdirunix(DataCollectedBaseDir.c_str());
		
		//touch files to save, in folders to save data.
      
      if (!parallel){
		SetFileCount(TheFiles,numfilespossible,DataCollectedBaseDir);
		TouchFilesToSave(TheFiles,numfilespossible,DataCollectedBaseDir);
      }
      else{
		for (int i = 1; i < numprocs;++i){
		  std::stringstream CurDataCollectedBaseDir;
		  CurDataCollectedBaseDir <<  DataCollectedBaseDir
					  << "c" << i << "/";
		  mkdirunix(CurDataCollectedBaseDir.str().c_str());
		  SetFileCount(TheFiles,numfilespossible,CurDataCollectedBaseDir.str());
		  TouchFilesToSave(TheFiles,numfilespossible,
				   CurDataCollectedBaseDir.str());
//			std::cout << "touching files in folder: " << CurDataCollectedBaseDir.str() << "\n";
		}
      }


		
		
		
		
		std::string randpointfilename;
		randpointfilename = base_dir;
		randpointfilename.append("/randstart");
		
		fout.open(randpointfilename.c_str());
		
		for (int i = 0; i < numparam; ++i){
			fout << RandomValues[i].first << " "
			<< RandomValues[i].second << "\n";
			
		}
		fout.close();
		
		
		
		
		
		//actually run that shit
		
		
		if (!parallel) {//this section needs a lot of work
//			std::string filenamestep2="bfiles_";
//			filenamestep2.append(filename);
//			filenamestep2.append("/step2/tmp/");
//			
//			std::ifstream finconfig2("config2");
//			while (!finconfig2.eof()) {
//				getline(finconfig2,copyme);
//				configvector.push_back(copyme);
//			}
//			finconfig2.close();
//				
//			int i=0;
////			bool firsttime = true;	  
//			while(i<Values.size()){
//			  
//			  
//				  std::vector<std::string> bertinitmpdir;
//				  std::vector< std::string > configvector;
//				  std::string copyme;
//				  
//
//
//				  
//					std::stringstream ss;
//					ss <<filenamestep2;      
//					ss << i;
//					std::string curbasedir=ss.str();
//					curbasedir.append("/");
//					ss << "/input";
//					fout.open(ss.str().c_str());	  
//					
//					WriteStep2(configvector,
//						   fout,
//						   Values[i],
//						   FunctVector, 
//						   VarGroupVector,
//						   ParamVector,
//						   ParamStrings,
//						   Consts,
//						   ConstantStrings,
//						   RandomValues,
//						   numfunct,
//						   numvar,
//						   numparam,
//						   numconsts);
//					fout.close();
//					// Copy the nonsingular_solutions file from
//					// the step1 run to bfiles_filename/curbasedir/start
//			//	    std::cout << "curbasedir = " << curbasedir << "\n";
//			//	    std::cout << "base_dir = " << base_dir << "\n";
//					CallCopyStartStep2(base_dir,curbasedir);
//					
//					
//					// Run Step2 on the current input file
//					WriteShell3();
//					//   std::cout << "i = " << i << " and pushing onto bertinitmpdir\n";
//					//	    std::cout << "numsubfolders = " << numsubfolders
//					//		      << ", eof = " << ( fin.eof()? "true\n":"false\n");
//					//	    std::cout << "loop condition to continue going = "
//					//	      << (i+1 < numsubfolders && !fin.eof()?"true\n":"false\n");
//					bertinitmpdir.push_back(curbasedir);
//			  
//			}//re:while loop
//
//			  UpdateFileCount(TheFiles, numfilespossible,DataCollectedBaseDir);
//
//			  for (int i = 0; i  < bertinitmpdir.size();++i){
//				bool append;
//				CallBertiniStep2(bertinitmpdir[i]);	  
//				for (int j = 0; j < numfilespossible;++j){
//				  if (TheFiles[j].saved){
//						// Store into appropriate files
//						std::string target_file_base_dir = base_dir;
//						target_file_base_dir.append("/step2/DataCollected/");
//						std::string target_file =
//						  MakeTargetFilename(target_file_base_dir,
//									 TheFiles,
//									 j);
//
//						std::string orig_file = bertinitmpdir[i];
//						orig_file.append(TheFiles[j].filename);
//						WriteData(runid, 
//							  orig_file, 
//							  target_file,
//							  //	  append,
//							  ParamStrings,
//							  CValuesVect[i]);	    		
//
//				  } // end if saved
//				}// end j
//				
//				std::ofstream lastrunfile;
//				std::string lastrunfilename;
//				lastrunfilename="bfiles_";
//				lastrunfilename.append(filename);
//				lastrunfilename.append("/step2/tmp/lastrun");
//				
//				lastrunfile.open(lastrunfilename.c_str());
//				lastrunfile << runid;
//				lastrunfile.close();
//				++runid;	    
//			  }// end i	    
//			  
			}// end not parallel
			
			else{//parallel case...

//			  if (CValuesVect.size()){
//
//				// update filecount if necessary ...
//				for (int i = 1; i < numprocs;++i){
//				  std::stringstream CurDataCollectedBaseDir;
//				  CurDataCollectedBaseDir <<  DataCollectedBaseDir
//							  << "c" << i << "/";
//				  mkdirunix(CurDataCollectedBaseDir.str().c_str());
//				  UpdateFileCount(TheFiles, numfilespossible,
//						  CurDataCollectedBaseDir.str());
//				}//re: making folders for data collection in DataCollected.
				std::string mpicommand;
				if (architecture==0) {

					mpicommand = "mpirun ";
					if (usemachine==1){
						mpicommand.append("-machinefile ");
						mpicommand.append(machinefile);
					}
					mpicommand.append(" -np ");
				}
				else {
					mpicommand = "aprun -n ";
				}

				std::stringstream ssnumproc;
				ssnumproc << numprocs;
				mpicommand.append(ssnumproc.str());
				mpicommand.append(" ./mystep2 ");	    
				mpicommand.append(filename);
				mpicommand.append(" ");
				int numfilestosave=0;
				for (int i = 0; i < numfilespossible;++i){
				  if (TheFiles[i].saved){
					  ++numfilestosave;
				  }
				}
				ssnumproc.str("");
				ssnumproc.clear();
				ssnumproc << numfilestosave;
				mpicommand.append(ssnumproc.str());
				mpicommand.append(" ");

				for (int i = 0; i < numfilespossible;++i){
				  if (TheFiles[i].saved){
					mpicommand.append(TheFiles[i].filename);
					mpicommand.append(" ");
					std::stringstream r;
					r << TheFiles[i].filecount;
					mpicommand.append(r.str());
					mpicommand.append(" ");
				  }
				}
				
				
				std::stringstream ss42;//nice variable name matt
				ss42 << numfilesatatime;
				ss42 << " ";
				mpicommand.append(ss42.str());
				

				ss42.str("");
				ss42.clear();
				ss42 << ParamStrings.size();
				ss42 << " ";
				mpicommand.append(ss42.str());
				for (int i = 0; i < ParamStrings.size();++i){
				  mpicommand.append(ParamStrings[i]);
				  mpicommand.append(" ");
				}
				

				ss42.str("");
				ss42.clear();
				
				
				ss42 << saveprogresseverysomany;
				ss42 << " ";
				

				mpicommand.append(ss42.str());
				
				std::cout << "\n\n\n\n\n\n\n\n" << mpicommand << "\n\n\n\n\n\n\n\n\n";
				system(mpicommand.c_str());

			  } // end parallel case
			   

			  

      
      break;
    }//end choice 7: step2
			
			
			
			
			
		/////////////////////////
			//
			//
			//     end choice 7
			//
			//
		//////////////////////////
			
			
			
			
			
			
			
			
			
			
			
			
			
	case 8:
		currentChoice=DetParallel;
			rerun = true;
			FilePrefVector.clear();
			parallel =  DeterminePreferences(architecture, usemachine, machinefile,numprocs,rerun, numfilesatatime,FilePrefVector,saveprogresseverysomany);
			SetPrefVersion(prefversion);
			rerun = false;
			
			WriteShell1(architecture, usemachine);
			WriteShell1Parallel(architecture, usemachine);	
			// Write  the copy shell script, running script
//			WriteShell2();
			WriteShell3(architecture);
			break;
			
    case 9:
      currentChoice=Quit;	    
      std::cout << "Quitting\n\n";
    }//re: switch (intChoice)
    
  }//re: while(currentChoice!=Quit)
  
  return 0;
}//re: main function
