
#include <mpi.h>
#include "step1_funcs.hpp"
#include "step2_funcs.hpp"
#include "mtrand.hpp"
#include "random.hpp"
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "slave.hpp"


//from the bertini library.
extern "C" {
	int bertini_main(int argC, char *args[]);
}






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
	
    
	std::stringstream myss;
	std::string currentSeedstring;
	int blaint, currentSeed = 0;
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
	
	
	//initialize argument string for bertini, for the 2.1 solve (setting up the data structure).
	char *args_parse[11];
	args_parse[0] = const_cast<char *>("bertini");
	args_parse[1] = const_cast<char *>("input");
	args_parse[2] = const_cast<char *>("start");
	args_parse[3] = const_cast<char *>("0");
	args_parse[4] = const_cast<char *>("0");
	args_parse[5] = const_cast<char *>("0");
	args_parse[6] = const_cast<char *>("0");
	args_parse[7] = const_cast<char *>("0");
	args_parse[8] = const_cast<char *>("0");
	args_parse[9] = const_cast<char *>("0");
	args_parse[10] = const_cast<char *>("2");
	
	
	//initialize for 2.2 solves, that is no parsing, just interpretation of preexisting .out files.
	char *args_noparse[12];
	args_noparse[0] = const_cast<char *>("bertini");
	args_noparse[1] = const_cast<char *>("input");
	args_noparse[2] = const_cast<char *>("start");
	args_noparse[3] = const_cast<char *>("0");
	args_noparse[4] = const_cast<char *>("0");
	args_noparse[5] = const_cast<char *>("0");
	args_noparse[6] = const_cast<char *>("0");
	args_noparse[7] = const_cast<char *>("0");
	args_noparse[8] = const_cast<char *>("0");
	args_noparse[9] = const_cast<char *>("0");
	args_noparse[10] = const_cast<char *>("3");
	//do not set the 11th argument here.
	
	
	
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
	
	std::vector<std::string> runningfile; //for storing the data between writes.
	runningfile.resize(numfiles);
	for (int i=0; i<numfiles; ++i) {
		runningfile[i].reserve(2*buffersize);
	}
	//	int writethreshold = 1; //how often to check file sizes...
	
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
	myss.clear();
	myss.str("");
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
	
	
	
	
	
	
	
	
	
	
	//////////////////////////
	//
	//     done writing the config, start, and input files
	//
	///////////////////////
	std::ofstream fout;
	

	
	//make the initial folder
	std::string initrunfolder;
	myss << tmpfolder << "/init" << myid;
	myss >> initrunfolder;
	myss.clear();
	myss.str("");
	boost::filesystem::remove_all(initrunfolder);
	mkdirunix(initrunfolder.c_str());
	
	//start
	std::string initrunstart = initrunfolder;
	initrunstart.append("/start");
	fout.open(initrunstart.c_str());
	
	if (!fout.is_open()) {
		std::cerr << "failed to properly open " << initrunstart << " to open start file, worker " << myid << std::endl;
		exit(719);
	}
	
	fout << start_char;
	fout.close();
	fout.clear();
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
	
	
	//input

	std::string initruninput = initrunfolder;
	initruninput.append("/input");
	fout.open(initruninput.c_str());
	if (!fout.is_open()) {
		std::cerr << "failed to properly open " << initruninput << " to open input file, worker " << myid << std::endl;
		exit(720);
	}
	
	fout << input_char;
	fout.close();
	fout.clear();
	

	
	blaint = chdir(initrunfolder.c_str());
	
	
#ifdef timingstep2
	process_timer.press_start("bertini");
#endif
	currentSeed = bertini_main(11,args_parse);
#ifdef timingstep2
	process_timer.add_time("bertini");
#endif
	
	myss << currentSeed;
	myss >> currentSeedstring;
	args_noparse[11] = const_cast<char *>(currentSeedstring.c_str());
	myss.clear();
	myss.str("");
	
	
#ifdef timingstep2
	process_timer.press_start("read");
#endif
	ReadDotOut(Numoutvector,
			   arroutvector,
			   degoutvector,
			   namesoutvector,
			   configvector,
			   funcinputvector);
#ifdef timingstep2
	process_timer.add_time("read",6);
#endif
	
	
	

	
	
	long loopcounter = 0;  //is for checking whether we are on the first loop
	
	blaint = chdir(workingfolder.c_str());  //formerly, used to move down on each iteration of loop.  now just stay in after initially moving into working folder.
	
	//todo: check if this chdir was successful
	
	
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
								funcinputvector);
#ifdef timingstep2
					process_timer.add_time("write",5);
#endif	
					loopcounter = 1;
				}
#ifdef timingstep2
				process_timer.press_start("write");
#endif
				WriteNumDotOut(Numoutvector,
							   AllParams,
							   numparam);
#ifdef timingstep2
				process_timer.add_time("write");
#endif
				
				
				
				// Call Bertini
#ifdef timingstep2
				process_timer.press_start("bertini");
#endif
				
#ifndef nosolve
				blaint = bertini_main(12,args_noparse);
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














