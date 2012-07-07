 #include "menu_cases.h"
//
////  make new random values.  user is guided through the steps by the program.  matt niemerg wrote this.
//std::vector< std::pair<double,double> > random_case(std::vector< std::pair<double,double> > & RandomValues,
//													std::vector<std::string> ParamStrings){
//	
//
//}  //   re: random_case
//
//
//
//
//void save_random_case(std::vector< std::pair<double,double> > RandomValues,
//					  int numparam){
//	
//
//}//  re: save_random_case
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//void load_random_case(std::ifstream & fin3, 
//					  std::vector< std::pair<double,double> > & RandomValues,
//					  std::vector<std::string> ParamStrings){
//	
//
//}







void steptwo_case(ProgSettings paramotopy_settings,
				  runinfo paramotopy_info){
	
	
	paramotopy_settings.WriteConfigStepTwo();		
	// open the bfiles_filename/mc file that contains the monte carlo points
	// in order in accordance to the order of the paramstrings in ParamStrings
	
	
	std::ifstream fin;
	std::ofstream fout;
	

	
	// make the tmp files directory/folder
	std::string filenamestep2;
	if (paramotopy_settings.settings["MainSettings"]["devshm"].intvalue==1) {
		filenamestep2= "/dev/shm";
	}
	else {
		filenamestep2 = stackoverflow_getcwd();
	}
	filenamestep2.append("/bfiles_");
	filenamestep2.append(paramotopy_info.inputfilename);
	filenamestep2.append("/step2/tmp/");
	mkdirunix(filenamestep2.c_str());
	
	
	
	std::string curline;	
	
	
	//make the DataCollected directory.
	std::string DataCollectedBaseDir=paramotopy_info.base_dir;
	DataCollectedBaseDir.append("/step2/DataCollected/");
	mkdirunix(DataCollectedBaseDir.c_str());
	
	//touch files to save, in folders to save data.
	
	if (paramotopy_settings.settings["MainSettings"]["parallel"].intvalue == 0){
		SetFileCount(paramotopy_settings,DataCollectedBaseDir);
		TouchFilesToSave(paramotopy_settings,DataCollectedBaseDir);
	}
	else{
		for (int i = 1; i < paramotopy_settings.settings["MainSettings"]["numprocs"].intvalue ;++i){
			std::stringstream CurDataCollectedBaseDir;
			CurDataCollectedBaseDir <<  DataCollectedBaseDir
			<< "c" << i << "/";
			mkdirunix(CurDataCollectedBaseDir.str().c_str());
			SetFileCount(paramotopy_settings,CurDataCollectedBaseDir.str());
			TouchFilesToSave(paramotopy_settings,
							 CurDataCollectedBaseDir.str());
		}
	}
	
	//write a file containing the random values.
	std::string randpointfilename;
	randpointfilename = paramotopy_info.base_dir;
	randpointfilename.append("/randstart");
	
	fout.open(randpointfilename.c_str());
	
	for (int i = 0; i < paramotopy_info.numparam; ++i){
		fout << paramotopy_info.RandomValues[i].first << " "
		<< paramotopy_info.RandomValues[i].second << "\n";
		
	}
	fout.close();
	
	
	
	
	
	//actually run that shit
	if (paramotopy_settings.settings["MainSettings"]["parallel"].intvalue == 0) {//this section needs a lot of work
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
		//////////////////////////////////////////////pickuphere
		std::stringstream mpicommand;
		
		mpicommand << paramotopy_settings.settings["MainSettings"]["architecture"].value() << " -n ";

		mpicommand << paramotopy_settings.settings["MainSettings"]["numprocs"].value() << " ";
		mpicommand << paramotopy_settings.settings["MainSettings"]["step2location"].value() << "/step2 ";	    
		mpicommand << paramotopy_info.inputfilename << " ";
		
		int numfilestosave=0;
		settingmap::iterator iter;
		std::stringstream commandss;
		
		for (iter=paramotopy_settings.settings["SaveFiles"].begin() ; iter!=paramotopy_settings.settings["SaveFiles"].end()  ;++iter){
			if ((*iter).second.intvalue==1) {//supposed to retrieve the integer value for the file setting
				numfilestosave++;
				commandss << (*iter).first << " " << (*iter).second.filenumber << " ";// << filename incrementedfilenumber ;  incrementedfilenumber set in touchfiletosave()
			}
		}

		mpicommand << numfilestosave << " ";
		mpicommand << commandss.str();
		
		mpicommand << paramotopy_settings.settings["MainSettings"]["numfilesatatime"].value() << " ";
		mpicommand << paramotopy_info.ParameterNames.size() << " ";

		for (int i = 0; i < int(paramotopy_info.ParameterNames.size());++i){
			mpicommand << paramotopy_info.ParameterNames[i] << " ";
		}
	
		mpicommand << paramotopy_settings.settings["MainSettings"]["saveprogresseverysomany"].value() << " ";
		mpicommand << paramotopy_settings.settings["MainSettings"]["newfilethreshold"].value() << " ";
		mpicommand << paramotopy_settings.settings["MainSettings"]["devshm"].value() << " ";
		
		if (paramotopy_settings.settings["MainSettings"]["stifle"].intvalue==1){
			mpicommand << " > /dev/null ";	
		}
		

		
		std::cout << "\n\n\n\n\n\n\n\n" << mpicommand.str() << "\n\n\n\n\n\n\n\n\n";
		system(mpicommand.str().c_str());  //make the system call to bertini
		
	} // end parallel case
	
	
	
	
}//re: step2case


