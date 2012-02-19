
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











void slave(std::vector<std::string> dir, 
		   ToSave *TheFiles, 
		   int numfiles,
		   std::vector<std::string> ParamNames, 
		   std::string base_dir,
		   int numfilesatatime){
	
	
	std::string up = "../../../..";
	std::string currentSeedstring;
	int blaint, currentSeed = 0;
	int linenumber;
	int numparam = ParamNames.size();
	int myid;
	MPI_Comm_rank(MPI_COMM_WORLD,&myid);
	MPI_Status status;
	
	double datareceived[numfilesatatime*(2*numparam+1)]; //for catching from the mpi
	std::stringstream myss;
	char *args_parse[3];
	args_parse[0] = "bertini";
	args_parse[1] = "input";
	args_parse[2] = "0";
	
	char *args_noparse[4];
	args_noparse[0] = "bertini";
	args_noparse[1] = "input";
	args_noparse[2] = "1";
	
	
	
	std::vector<std::string> Numoutvector; 
	std::vector<std::string> arroutvector;
	std::vector<std::string> degoutvector;
	std::vector<std::string> namesoutvector;
	std::vector<std::string> configvector;
	std::vector<std::string> funcinputvector;
	
	
	
	
	
	myss << base_dir << "/step2/DataCollected/c"
	<< myid << "/";
	std::string DataCollectedbase_dir = myss.str();//specific to this worker, as based on myid
	std::vector<std::pair<double,double> >  AllParams;
	AllParams.resize(numparam);//preallocate
	myss.clear();
	myss.str("");
	
	
	
#ifdef timingstep2
	double t_start, t_receive=0, t_send=0, t_write=0, t1, t_bertini=0; //for timing the whatnot , t2, t3, t4, t_end, t_initial=0
	int writecounter = 0, sendcounter = 0, receivecounter = 0, bertinicounter = 0;
	t_start = omp_get_wtime();
	std::ofstream timingout;
	std::string timingname = base_dir;
	myss << myid;
	timingname.append("/timing/slavetiming");
	timingname.append(myss.str());
	myss.clear();
	myss.str("");
#endif 
	
	
	
	std::string initrunfolder;
	myss << base_dir <<  "/step2/tmp/init" << myid;
	myss >> initrunfolder;
	myss.clear();
	myss.str("");
	
	
	int arbitraryint =0 ;
	MPI_Bcast(&arbitraryint, 1, MPI_INT, 0, MPI_COMM_WORLD);// recieve a bcast from head telling its ok to proceed
	
	//		std::cout << "w" << myid << " " << initrunfolder << "\n";
	blaint = chdir(initrunfolder.c_str());
	
#ifdef timingstep2
	t1 = omp_get_wtime();
#endif
	
	currentSeed = bertini_main(3,args_parse);
	//	std::cout << myid << "bertin\n";
#ifdef timingstep2
	t_bertini += omp_get_wtime() - t1;
	bertinicounter++;
#endif
	
	std::cout << currentSeed<<"\n";
	//		std::cout << "done with the first pass, worker " << myid << "\n";
	ReadDotOut(Numoutvector, 
			   arroutvector,
			   degoutvector,
			   namesoutvector,
			   configvector,
			   funcinputvector);
	blaint = chdir(up.c_str());
	
	//		std::cout << "done reading \n";
	myss << currentSeed;
	myss >> currentSeedstring;
	args_noparse[3] = const_cast<char *>(currentSeedstring.c_str());
	myss.clear();
	myss.str("");
	
	
	arbitraryint=0;
	MPI_Bcast(&arbitraryint, 1, MPI_INT, 0, MPI_COMM_WORLD); //agree w headnode that done w initial solve.
	
	
	
	
	int loopcounter = 0;  //is for counting how many solves this worker has performed, for data collection file incrementing
	blaint = chdir(dir[myid-1].c_str());  //used to move down on each iteration of loop.  now just stay in after initially moving into working folder.
	
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
			break; //exits the while loop
		}
		
		
		
		
		int localcounter = 0;
		for (int k = (myid-1)*numfilesatatime; k < (myid-1)*numfilesatatime+int(status.MPI_TAG); ++k){
			
			
			// unpack some data for passing around; perhaps should rewrite stuff to make this unnecessary
			for (int mm=0; mm<numparam; ++mm) {
				AllParams[mm].first = datareceived[localcounter*(2*numparam+1)+2*mm];
				AllParams[mm].second = datareceived[localcounter*(2*numparam+1)+2*mm+1];
			}
			linenumber = int(datareceived[localcounter*(2*numparam+1)+2*numparam]);
			
			
			// Call Bertini
			//		blaint = chdir(dir[k].c_str());  //used to move down.  now just stay in after initially moving into working folder.
			if (loopcounter<numfilesatatime) {
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
			t1 = omp_get_wtime();
#endif
			blaint = bertini_main(4,args_noparse);
#ifdef timingstep2
			t_bertini += omp_get_wtime() - t1;
			bertinicounter++;
#endif
			//		blaint = chdir(up.c_str()); //used to move up.  now stay down there.   no sense in moving, when each worker is just writing its own files.
			
			
			
			
			// Collect the Data
			for (int j = 0; j < numfiles;++j){
				
				std::string target_file =
				MakeTargetFilename(DataCollectedbase_dir,
								   TheFiles,
								   j);
				//			  std::string orig_file = dir[k];
				std::string orig_file = "";  //original file is in current directory now.  -dan brake
				
				orig_file.append(TheFiles[j].filename);
				
#ifdef verbosestep2
				std::cout << target_file << " " << orig_file << "\n";
				std::cout << "allparams: " << AllParams[numparam-1].first << "\n";
				std::cout << "writing data to " << target_file << " from worker " << myid << " with folder" << k <<"\n";
#endif
				
				
				
				//write data to file
#ifdef timingstep2
				t1= omp_get_wtime();
#endif
				WriteData(linenumber, 
						  orig_file, 
						  target_file,
						  ParamNames,
						  AllParams);	
#ifdef timingstep2
				t_write += omp_get_wtime() - t1;
				writecounter++;
#endif
				
			}
    		
			
#ifdef verbosestep2
			std::cout << "Finished doing work on iteration : " << k << " on rank " << myid << "\n";
#endif
			
			
			++localcounter;
			++loopcounter;
			if (loopcounter==512){//every 512 loops, check file sizes
				UpdateFileCount(TheFiles,numfiles,DataCollectedbase_dir);
				loopcounter=0;
			}
			
			
		}//re: for (int k = 0; k < int(status.MPI_TAG);++k)
		
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
	
	blaint = chdir(up.c_str()); //used to move up inside loop.  now stay down there for loop, move up here.   no sense in moving, when each worker is just writing its own files.
	
#ifdef timingstep2
	timingout.open(timingname.c_str(),std::ios::out);
	timingout << myid << "\n"
	<< "bertini: " << t_bertini << " " << bertinicounter << "\n"
	<< "write: " << t_write << " " << writecounter << "\n"
	<< "send: " << t_send << " " << sendcounter << "\n"
	<< "receive: " << t_receive << " " << receivecounter << "\n"
	<< "total: " << omp_get_wtime() - t_start << "\n";
	timingout.close();
#endif	
	return;
}//re:slave






