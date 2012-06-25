#include "menu_cases.h"

//  make new random values.  user is guided through the steps by the program.  matt niemerg wrote this.
std::vector< std::pair<double,double> > random_case(std::vector< std::pair<double,double> > & RandomValues,
													std::vector<std::string> ParamStrings){
	
	MTRand drand(time(0));
	int randchoice;
	std::cout << "1) Default range [0 to 1)\n"
	<< "2) User-specified range\n"
	<< "Enter in if you would like to use the standard \n"
	<< "default range for random floats or if you would \n"
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
			for (int i = 0; i < int(ParamStrings.size());++i){
				std::cout << i+1 << " - " << ParamStrings[i]
				<< "\n";
			}
			std::cout << "Enter the parameter you want to rerandomize : ";
			std::cin >> paramrandchoice;
			while (paramrandchoice < 0 || paramrandchoice > int(ParamStrings.size())){
				
				std::cout << "0 - Done specifying random values\n";
				for (int i = 0; i < int(ParamStrings.size());++i){
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
				
				double creallow;
				double crealhigh;
				double cimaginarylow;
				double cimaginaryhigh;
				std::cout << "\n" << "Real Low : ";
				std::cin >> creallow;
				std::cout << "Real High : ";
				std::cin >> crealhigh;
				std::cout << "Imaginary Low : ";
				std::cin >> cimaginarylow;
				std::cout << "Imaginary High : ";
				std::cin >> cimaginaryhigh;	  
				double crandreal = drand();
				double crandimaginary = drand();
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
	
	
	
	
	return RandomValues;
}  //   re: random_case




void save_random_case(std::vector< std::pair<double,double> > RandomValues,
					  std::ofstream & fout,
					  int numparam){
	std::string randpointfilename;
	std::cout << "Enter the filename you want to save the random start points to : ";
	std::cin >> randpointfilename;
	
	fout.open(randpointfilename.c_str());
	
	for (int i = 0; i < numparam; ++i){
		fout << RandomValues[i].first << " "
		<< RandomValues[i].second << "\n";
	}
	fout.close();
	
	return;
}//  re: save_random_case














void load_random_case(std::ifstream & fin3, 
					  std::vector< std::pair<double,double> > & RandomValues,
					  std::vector<std::string> ParamStrings){
	
	std::string randfilename;
	int cancelthis = 0;
	
	fin3.close();
	while ( !fin3.is_open() ) {
		std::cout << "Enter the filename of the random points you"
		<< " want to load (% to cancel): ";
		std::cin >> randfilename;
		
		size_t found=randfilename.find('%');
		if (found!=std::string::npos) {
			if ( int(found) == 0) {
				std::cout << "canceling load.\n" << std::endl;
				cancelthis = 1;
				break;
			}
			
		}
		
		
		fin3.open(randfilename.c_str());
	}
	
	if (!cancelthis) {
		
		std::string mytemp;
		int ccount=0;
		std::cout << "\n\n";
		while(getline(fin3,mytemp)){
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
		fin3.close();
	}
	PrintRandom(RandomValues,ParamStrings);//in random.cpp
	
	return;
}







void step2_case(int numfilespossible,
				ToSave *TheFiles,
				std::string filename,
				bool parallel,
				preferences *Prefs,
				int numparam, 
				std::vector< std::pair<double,double> > RandomValues,
				std::vector<std::string> ParamStrings){
	
	
	
	
	// open the bfiles_filename/mc file that contains the monte carlo points
	// in order in accordance to the order of the paramstrings in ParamStrings
	
	
	std::ifstream fin;
	std::ofstream fout;
	
	std::string base_dir = make_base_dir_name(filename);
	
	
	// make the tmp files directory/folder
	std::string filenamestep2;
	if (Prefs[0].devshm==1) {
		filenamestep2= "/dev/shm";
	}
	else {
		filenamestep2 = stackoverflow_getcwd();
	}
	filenamestep2.append("/bfiles_");
	filenamestep2.append(filename);
	filenamestep2.append("/step2/tmp/");
	mkdirunix(filenamestep2.c_str());
	
	
	
	std::string curline;	
	
	
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
		for (int i = 1; i < Prefs[0].numprocs;++i){
			std::stringstream CurDataCollectedBaseDir;
			CurDataCollectedBaseDir <<  DataCollectedBaseDir
			<< "c" << i << "/";
			mkdirunix(CurDataCollectedBaseDir.str().c_str());
			SetFileCount(TheFiles,numfilespossible,CurDataCollectedBaseDir.str());
			TouchFilesToSave(TheFiles,numfilespossible,
							 CurDataCollectedBaseDir.str());
			
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
		
		std::string mpicommand;
		if (Prefs[0].architecture==0) {
			
			mpicommand = "mpiexec ";
			if (Prefs[0].usemachine==1){
				mpicommand.append("-machinefile ");
				mpicommand.append(Prefs[0].machinefile);
			}
			mpicommand.append(" -np ");
		}
		else {
			mpicommand = "aprun -n ";
		}
		
		std::stringstream ssnumproc;
		ssnumproc << Prefs[0].numprocs;
		mpicommand.append(ssnumproc.str());
		mpicommand.append(" ");
		mpicommand.append(Prefs[0].step2location);
		mpicommand.append("/step2 ");	    
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
		ss42 << Prefs[0].numfilesatatime;
		ss42 << " ";
		mpicommand.append(ss42.str());
		
		
		ss42.str("");
		ss42.clear();
		ss42 << ParamStrings.size();
		ss42 << " ";
		mpicommand.append(ss42.str());
		for (int i = 0; i < int(ParamStrings.size());++i){
			mpicommand.append(ParamStrings[i]);
			mpicommand.append(" ");
		}
		
		
		ss42.str("");
		ss42.clear();
		
		
		ss42 << Prefs[0].saveprogresseverysomany;
		ss42 << " ";
		
		ss42 << Prefs[0].newfilethreshold;
		ss42 << " ";
		
		ss42 << Prefs[0].devshm;
		ss42 << " ";
		
		if (Prefs[0].stifle==1){
			ss42 << " > /dev/null ";	
		}
		
		mpicommand.append(ss42.str());
		
		std::cout << "\n\n\n\n\n\n\n\n" << mpicommand << "\n\n\n\n\n\n\n\n\n";
		system(mpicommand.c_str());
		
	} // end parallel case
	
	
	
	
}//re: step2case


