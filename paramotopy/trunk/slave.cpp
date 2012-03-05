
#include <mpi.h>
#include "step1.h"
#include "step2.h"
#include "mtrand.h"
#include "random.h"
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
#include "slave.h"



extern "C" {
	int bertini_main(int argC, char *args[]);
}






//////////////////////////////////
//
//         slave function
//
/////////////////////////////











void slave(std::vector<std::string> tmpfolderlocs, 
		   ToSave *TheFiles, 
		   int numfiles,
		   std::vector<std::string> ParamNames, 
		   std::string base_dir,
		   int numfilesatatime,
		   std::string called_dir,
		   std::string templocation,
		   int newfilethreshold){
	
	
	std::string up = "../../../..";
	std::string currentSeedstring;
	int blaint, currentSeed = 0;
	int linenumber;
	int numparam = ParamNames.size();
	int myid;

	
	
	MPI_Comm_rank(MPI_COMM_WORLD,&myid);
	MPI_Status status;
	
	void *v;
    int flag;
    int norunflag;
	MPI_Comm_get_attr( MPI_COMM_WORLD, MPI_TAG_UB, &v, &flag );
	norunflag = *(int*)v - 1;
	
	double datareceived[numfilesatatime*(2*numparam+1)]; //for catching from the mpi
	std::stringstream myss;
	char *args_parse[11];
	args_parse[0] = "bertini";
	args_parse[1] = "input";
	args_parse[2] = "start";
	args_parse[3] = "0";
	args_parse[4] = "0";
	args_parse[5] = "0";
	args_parse[6] = "0";
	args_parse[7] = "0";
	args_parse[8] = "0";
	args_parse[9] = "0";
	args_parse[10] = "2";
	
	char *args_noparse[12];
	args_noparse[0] = "bertini";
	args_noparse[1] = "input";
	args_noparse[2] = "start";
	args_noparse[3] = "0";
	args_noparse[4] = "0";
	args_noparse[5] = "0";
	args_noparse[6] = "0";
	args_noparse[7] = "0";
	args_noparse[8] = "0";
	args_noparse[9] = "0";
	args_noparse[10] = "3";
	
	
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
		runningfile[i].reserve(524288);
	}
//	int writethreshold = 1; //how often to check file sizes...
	
	
	myss << called_dir << "/" << base_dir << "/step2/DataCollected/c"
	<< myid << "/";
	std::string DataCollectedbase_dir = myss.str();//specific to this worker, as based on myid
	std::vector<std::pair<double,double> >  AllParams;
	AllParams.resize(numparam);//preallocate
	myss.clear();
	myss.str("");
	
	
	
#ifdef timingstep2
	double t_start, t_receive = 0, t_send = 0, t_read = 0, t_write = 0, t1, t_bertini = 0; //for timing the whatnot , t2, t3, t4, t_end, t_initial=0
	int readcounter = 0, writecounter = 0, sendcounter = 0, receivecounter = 0, bertinicounter = 0;
	t_start = omp_get_wtime();
	std::ofstream timingout;
	std::string timingname = called_dir;
	timingname.append("/");
	timingname.append(base_dir);
	myss << myid;
	timingname.append("/timing/slavetiming");
	timingname.append(myss.str());
	myss.clear();
	myss.str("");
#endif 
	
	
	
	std::string initrunfolder;
	myss << templocation << "/" << base_dir <<  "/step2/tmp/init" << myid;
	myss >> initrunfolder;
	myss.clear();
	myss.str("");
	mkdirunix(initrunfolder.c_str());
	
	std::string workingfolder;
	myss << templocation << "/" << base_dir <<  "/step2/tmp/" << myid;
	myss >> workingfolder;
	myss.clear();
	myss.str("");
	mkdirunix(workingfolder.c_str());
//	tmpfolderlocs[myid-1].c_str()
	
	int vectorlengths[2] = {0};
	
	///////
	//
	//      get the start and config files from master
	//
	///////////
#ifdef verbosestep2
	std::cout << "telling workers the number of chars to expect.\n";
#endif
#ifdef timingstep2
	t1 = omp_get_wtime();
#endif	
	MPI_Bcast(&vectorlengths, 2, MPI_INT, 0, MPI_COMM_WORLD);
#ifdef timingstep2
	t_send += omp_get_wtime()-t1;
	sendcounter++;
#endif
	
	char start_char[vectorlengths[0]];
	char input_char[vectorlengths[1]];
#ifdef verbosestep2
	std::cout << "sending start and config.\n";
#endif
#ifdef timingstep2
	t1 = omp_get_wtime();
#endif	
	MPI_Bcast(&start_char, vectorlengths[0], MPI_CHAR, 0, MPI_COMM_WORLD);
	MPI_Bcast(&input_char, vectorlengths[1], MPI_CHAR, 0, MPI_COMM_WORLD);	
#ifdef timingstep2
	t_send += omp_get_wtime()-t1;
	sendcounter++;
	sendcounter++;
#endif
	///////
	//
	//      now have the start and config files.  need to write to file.
	//
	///////////
	
	//input
	std::ofstream fout;
	std::string initruninput = initrunfolder;
	initruninput.append("/input");
	fout.open(initruninput.c_str());	  
	fout << input_char;
	fout.close();	
	
	
	//start
	std::string initrunstart = initrunfolder;
	initrunstart.append("/start");
	fout.open(initrunstart.c_str());
	fout << start_char;
	fout.close();
	
	

	
	
	
	//////////////////////////
	//
	//     done writing the config, start, and input files
	//
	///////////////////////
	
	
	
	
	int arbitraryint =0 ;
#ifdef timingstep2
	t1 = omp_get_wtime();
#endif
	MPI_Bcast(&arbitraryint, 1, MPI_INT, 0, MPI_COMM_WORLD);// recieve a bcast from head telling its ok to proceed
#ifdef timingstep2
	t_receive+= omp_get_wtime() - t1;
	receivecounter++;
#endif
	
	blaint = chdir(initrunfolder.c_str());
#ifdef timingstep2
	t1 = omp_get_wtime();
#endif
	currentSeed = bertini_main(11,args_parse);
#ifdef timingstep2
	t_bertini += omp_get_wtime() - t1;
	bertinicounter++;
#endif
	
	
	
#ifdef timingstep2
	t1 = omp_get_wtime();
#endif
	ReadDotOut(Numoutvector, 
			   arroutvector,
			   degoutvector,
			   namesoutvector,
			   configvector,
			   funcinputvector);
#ifdef timingstep2
	t_read += omp_get_wtime() - t1;
	readcounter++; //increment the counter
#endif	
	
	
//	blaint = chdir(up.c_str());
	
	//		std::cout << "done reading \n";
	myss << currentSeed;
	myss >> currentSeedstring;
	args_noparse[11] = const_cast<char *>(currentSeedstring.c_str());
	myss.clear();
	myss.str("");
	
	
	arbitraryint=0;
#ifdef timingstep2
	t1 = omp_get_wtime();
#endif
	MPI_Bcast(&arbitraryint, 1, MPI_INT, 0, MPI_COMM_WORLD); //agree w headnode that done w initial solve.
#ifdef timingstep2
	t_receive+= omp_get_wtime() - t1;
	receivecounter++;
#endif

	
	
	long loopcounter = 0;  //is for counting how many solves this worker has performed, for data collection file incrementing
	blaint = chdir(workingfolder.c_str());  //used to move down on each iteration of loop.  now just stay in after initially moving into working folder.
	
	
	std::cout << myid << " " << stackoverflow_getcwd() << "\n";

	
	std::string target_file;
	while (1) {
		
		/* Receive a message from the master */
#ifdef timingstep2
		t1 = omp_get_wtime();
#endif
		MPI_Recv(&datareceived, numfilesatatime*(2*numparam+1), MPI_DOUBLE, 0, MPI_ANY_TAG,
				 MPI_COMM_WORLD, &status);
#ifdef timingstep2
		t_receive+= omp_get_wtime() - t1;
		receivecounter++;
#endif
		
		
		
		
		/* Check the tag of the received message. */
		if (status.MPI_TAG == 0) {
#ifdef verbosestep2
			std::cout << "received kill tag to rank " << myid << "\nkilling now\n";
#endif
			
			for (int j = 0; j < numfiles;++j){
				
				target_file = MakeTargetFilename(DataCollectedbase_dir,TheFiles,j);	
				std::cout << myid << " " << target_file << "\n";
				
#ifdef timingstep2
				t1= omp_get_wtime();
#endif
				WriteData(runningfile[j], 
						  target_file,
						  ParamNames);
				runningfile[j].clear();// = "";				
#ifdef timingstep2
				t_write += omp_get_wtime() - t1;
				writecounter++;
#endif
			}
			break; //exits the while loop
		}
		
		
		
		
		
		if (int(status.MPI_TAG) != norunflag) {
		long localcounter = 0;
		for (int k = 0; k < int(status.MPI_TAG); ++k){
			
			
			// unpack some data for passing around; perhaps should rewrite stuff to make this unnecessary
			for (int mm=0; mm<numparam; ++mm) {
				AllParams[mm].first = datareceived[localcounter*(2*numparam+1)+2*mm];
				AllParams[mm].second = datareceived[localcounter*(2*numparam+1)+2*mm+1];
			}
			linenumber = int(datareceived[localcounter*(2*numparam+1)+2*numparam]);
			
			
			// Call Bertini
#ifdef timingstep2
			t1= omp_get_wtime();
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
			}
			WriteNumDotOut(Numoutvector,
						   AllParams,
						   numparam);
#ifdef timingstep2
			t_write += omp_get_wtime() - t1;
			writecounter++; //increment the counter
#endif

			
			
			
#ifdef timingstep2
			t1 = omp_get_wtime();
#endif
			blaint = bertini_main(12,args_noparse);
#ifdef timingstep2
			t_bertini += omp_get_wtime() - t1;
			bertinicounter++;
#endif
			
			
			
			
			// Collect the Data
			for (int j = 0; j < numfiles;++j){
				
				
				//get data from file
#ifdef timingstep2
				t1= omp_get_wtime();
#endif
				runningfile[j].append(AppendData(linenumber,
												 TheFiles[j].filename,
												 ParamNames,
												 AllParams));
#ifdef timingstep2
				t_read += omp_get_wtime() - t1;
				readcounter++; //increment the counter
#endif				
				

				//if big enough
				if (int(runningfile[j].size()) > 65536){
					std::string target_file = MakeTargetFilename(DataCollectedbase_dir,TheFiles,j);				
					filesizes[j] += int(runningfile[j].size());
#ifdef verbosestep2
					std::cout << target_file << " " << TheFiles[j].filename << "\n";
					std::cout << "allparams: " << AllParams[numparam-1].first << "\n";
					std::cout << "writing data to " << target_file << " from worker " << myid << " with folder" << k <<"\n";
#endif
#ifdef timingstep2
					t1= omp_get_wtime();
#endif
					WriteData(runningfile[j], 
							  target_file,
							  ParamNames);

#ifdef timingstep2
					t_write += omp_get_wtime() - t1;
					writecounter++; //increment the counter
#endif
					runningfile[j].clear();//reset the string
					if (filesizes[j] > newfilethreshold) { //update the file count
						TheFiles[j].filecount+=1;
						filesizes[j] = 0;
					}
				}
				

			}
    		
			
#ifdef verbosestep2
			std::cout << "Finished doing work on iteration : " << k << " on rank " << myid << "\n";
#endif
			
			
			++localcounter;
			++loopcounter;

			
			
		}//re: for (int k = 0; k < int(status.MPI_TAG);++k)
		}
#ifdef verbosestep2
		std::cout << "\n\n" << myid << " " << linenumber << " \n";
#endif
		
		
#ifdef timingstep2
		t1 = omp_get_wtime();
#endif
		MPI_Send(&linenumber, 1, MPI_INT, 0, int(status.MPI_TAG), MPI_COMM_WORLD);
#ifdef timingstep2
		t_send += omp_get_wtime() - t1;
		sendcounter++;
#endif
		
	}//re: while (1)
	
	blaint = chdir(up.c_str()); //used to move inside loop.  now stay down there for loop, move here.   no sense in moving, when each worker is just writing its own files.
	
#ifdef timingstep2
	timingout.open(timingname.c_str(),std::ios::out);
	timingout << myid << "\n"
	<< "bertini: " << t_bertini << " " << bertinicounter << "\n"
	<< "write: " << t_write+t_read << " " << writecounter+readcounter << "\n"
	<< "send: " << t_send << " " << sendcounter << "\n"
	<< "receive: " << t_receive << " " << receivecounter << "\n"
	<< "total: " << omp_get_wtime() - t_start << "\n";
	timingout.close();
#endif	
	return;
}//re:slave





