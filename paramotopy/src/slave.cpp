#include "slave.hpp"









//////////////////////////////////
//
//         slave function
//
/////////////////////////////











void slave(ToSave *TheFiles,
					 int numfiles,
					 std::vector<std::string> ParamNames,
					 std::string base_dir,
					 int numfilesatatime,
					 std::string called_dir,
					 std::string tmpfolder,
					 int newfilethreshold,
					 std::string location,
					 int buffersize,
					 ProgSettings & paramotopy_settings,
					 runinfo & paramotopy_info,
					 timer & process_timer)
{
  int sstep2 = paramotopy_settings.settings["mode"]["standardstep2"].intvalue;
  bool standardstep2;
  if (sstep2 == 0){
    standardstep2 = false;
  }
  else{
    standardstep2 = true;
  }
	std::stringstream myss;
	int blaint;
	unsigned int currentSeed;
	int MPType;
	int linenumber;
	int numparam = ParamNames.size();
	int myid;
	
	int numprocs;
	
	MPI_Comm_rank(MPI_COMM_WORLD,&myid);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Status status;
	


	std::ofstream bertini_input_file;

	
	void *v;
	int flag;
	int norunflag;
	MPI_Comm_get_attr( MPI_COMM_WORLD, MPI_TAG_UB, &v, &flag );
	norunflag = *(int*)v - 1;
	
	double* datareceived = new double[numfilesatatime*(2*numparam+1)]; //for catching from the mpi
	
	
	
	//setup the vector of filesizes
	std::vector<int> filesizes;
	filesizes.resize(numfiles);
	for (int i=0; i<numfiles; ++i) {
		filesizes[i] = 0;
	}
	
	std::vector<std::string> Numoutvector;
	std::vector<std::string> arroutvector;
	std::vector<std::string> degoutvector;
	std::vector<std::string> namesoutvector;
	std::vector<std::string> configvector;
	std::vector<std::string> funcinputvector;
	std::vector<std::string> preproc_datavector;


	std::vector<std::string> runningfile; //for storing the data between writes.
	runningfile.resize(numfiles);
	for (int i=0; i<numfiles; ++i) {
		runningfile[i].reserve(2*buffersize);
	}

	
	//make data file location
	myss << called_dir << "/" << location << "/step2/DataCollected/c" << myid << "/";
	std::string DataCollectedbase_dir = myss.str();//specific to this worker, as based on myid
	myss.clear();
	myss.str("");
	
	
	std::vector<std::pair<double,double> >  AllParams;
	AllParams.resize(numparam);//preallocate
	
	
	
	
	

	

	
	std::string workingfolder;
	myss << tmpfolder << "/work" << myid;
	myss >> workingfolder;
	myss.clear(); myss.str("");
	
	boost::filesystem::remove_all(workingfolder);
	mkdirunix(workingfolder.c_str());
	
	int vectorlengths[2] = {0};
	
	///////
	//
	//      get the start and config files from master
	//
	///////////
#ifdef verbosestep2
	std::cout << "receiving from master the number of chars to expect.\n";
#endif
#ifdef timingstep2
	process_timer.press_start("receive");
#endif
	
	MPI_Recv(&vectorlengths[0], 2, MPI_INT, 0, 12, MPI_COMM_WORLD, &status);
	
	
#ifdef timingstep2
	process_timer.add_time("receive");
#endif
	
	char* start_char = new char[vectorlengths[0]];
	
	
#ifdef verbosestep2
	std::cout << myid << ", getting start file.\n";
#endif
#ifdef timingstep2
	process_timer.press_start("receive");
#endif
	MPI_Recv(&start_char[0], vectorlengths[0], MPI_CHAR, 0, 13, MPI_COMM_WORLD, &status);
	
#ifdef timingstep2
	process_timer.add_time("receive");
#endif
	
	
	

	
	///////
	//
	//      now have the start and config files.  need to write to file.
	//
	///////////
	
	
	
	
	
	
	
	
	
	

	std::ofstream fout;
	
	
	
	char* input_char = new char[vectorlengths[1]];

#ifdef verbosestep2
	std::cout << myid << ", getting input file.\n";
#endif
#ifdef timingstep2
	process_timer.press_start("receive");
#endif
	
	MPI_Recv(&input_char[0], vectorlengths[1], MPI_CHAR, 0, 14, MPI_COMM_WORLD, &status);
	
#ifdef timingstep2
	process_timer.add_time("receive");
#endif
	
	
	
	blaint = chdir(workingfolder.c_str());
	
	

	fout.open("start");
	if (!fout.is_open()) {
		std::cerr << "failed to properly open " << "start" << " to open start file, worker " << myid << std::endl;
		exit(719);
	}
	fout << start_char;
	fout.close(); fout.clear();
	
	
	

	fout.open("input");
	if (!fout.is_open()) {
		std::cerr << "failed to properly open " << "input" << " to open input file, worker " << myid << std::endl;
		exit(720);
	}
	fout << input_char;
	fout.close(); fout.clear();
	
	
#ifdef timingstep2
	process_timer.press_start("bertini");
#endif
	parse_input_file_bertini(currentSeed, MPType);
#ifdef timingstep2
	process_timer.add_time("bertini");
#endif
	

	
	//////////////////////////
	//
	//     done writing the config, start, and input files
	//
	///////////////////////
	
	
	
#ifdef timingstep2
	process_timer.press_start("read");
#endif


	ReadDotOut(Numoutvector,
		   arroutvector,
		   degoutvector,
		   namesoutvector,
		   configvector,
		   funcinputvector,
		   preproc_datavector);


#ifdef timingstep2
	process_timer.add_time("read",6);
#endif
	
	
	

	
	

	 
	
	long loopcounter = 0;  //is for checking whether we are on the first loop
	std::string target_file;
	
	
	while (1) {
		
		/* Receive parameter points and line numbers from the master */
#ifdef timingstep2
		process_timer.press_start("receive");
#endif
		
		MPI_Recv(&datareceived[0], numfilesatatime*(2*numparam+1), MPI_DOUBLE, 0, MPI_ANY_TAG,
				 MPI_COMM_WORLD, &status);

#ifdef timingstep2
		process_timer.add_time("receive");
#endif
		
		
		
		
		/* Check the tag of the received message. */
		if (status.MPI_TAG == 0) {
#ifdef verbosestep2
			std::cout << "received kill tag to rank " << myid << "\nkilling now\n";
#endif
			
			for (int j = 0; j < numfiles;++j){
				
				target_file = MakeTargetFilename(DataCollectedbase_dir,TheFiles,j);
				
				
#ifdef verbosestep2
				std::cout << "final writing data to " << target_file << " from worker " << myid << "\n";
#endif
				
#ifdef timingstep2
				process_timer.press_start("write");
#endif
				WriteData(runningfile[j],
						  target_file,
						  ParamNames);
				runningfile[j].clear();
#ifdef timingstep2
				process_timer.add_time("write");
#endif
			}
			break; //exits the while loop
		}
		
		
		
		
		
		if (int(status.MPI_TAG) != norunflag) {
			long long localcounter = 0;
			for (int k = 0; k < int(status.MPI_TAG); ++k){
				
				
				// unpack some data for passing around; perhaps should rewrite stuff to make this unnecessary
				for (int mm=0; mm<numparam; ++mm) {
					AllParams[mm].first  = datareceived[localcounter*(2*numparam+1)+2*mm];
					AllParams[mm].second = datareceived[localcounter*(2*numparam+1)+2*mm+1];
				}
				
				linenumber = int(datareceived[localcounter*(2*numparam+1)+2*numparam]);
				
			
#ifdef timingstep2
				process_timer.press_start("write");
#endif
				if (loopcounter==0){    //<numfilesatatime) {
				  fout.open("start");
				  fout << start_char;
				  fout.close();
				  
				  WriteDotOut(arroutvector,
					      degoutvector,
					      namesoutvector,
					      configvector,
					      funcinputvector,				  
					      preproc_datavector);
				
#ifdef timingstep2
				process_timer.add_time("write",5);
#endif	
				loopcounter = 1;
			}
#ifdef timingstep2
			process_timer.press_start("write");
#endif
			
			/** this guy needs to be changed for dealing with
			    nonstandard parameter runs.
			*/
			
			WriteNumDotOut(Numoutvector,
				       AllParams,
				       numparam,
				       standardstep2);
#ifdef timingstep2
			process_timer.add_time("write");
#endif
			
			
			
			// Call Bertini
#ifdef timingstep2
			process_timer.press_start("bertini");
#endif
			
#ifndef nosolve
				run_zero_dim_main(MPType, currentSeed);
#endif
			
#ifdef timingstep2
			process_timer.add_time("bertini");
#endif
			
			
#ifndef nosolve
			SlaveCollectAndWriteData(numfiles,
						 runningfile,
						 linenumber,
						 TheFiles,
						 ParamNames,
						 AllParams,
						 buffersize,
						 DataCollectedbase_dir,
						 filesizes, newfilethreshold, myid,
						 process_timer);
#endif
			
			++localcounter;
			
			
			
		}//re: for (int k = 0; k < int(status.MPI_TAG);++k)
	}
	
	
	
#ifdef timingstep2
	process_timer.press_start("send");
#endif
#ifdef verbosestep2
	std::cout << "sending done with line " << linenumber << " on worker " << myid << std::endl;
#endif
	MPI_Send(&linenumber, 1, MPI_INT, 0, int(status.MPI_TAG), MPI_COMM_WORLD);
#ifdef timingstep2
	process_timer.add_time("send");
#endif
	
}//re: while (1)

blaint = chdir(called_dir.c_str()); //used to move back to the starting directory to write the timing files.  now stay down there for loop, move here.   no sense in moving, when each worker is just writing its own files.
	//todo: make this chdir test for success.
	
	
	
	
	//delete[] input_char;
	delete[] start_char;
	delete[] datareceived;
	return;
}//re:slave










