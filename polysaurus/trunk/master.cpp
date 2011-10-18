

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
#include "step2readandwrite.h"

#include "master.h"






void master(std::vector<std::string> dir,
			std::string filename,
			int numfilesatatime,
			int saveprogresseverysomany){//added mcfin,filename, etc 3/7/11 db
	
	
	
	
	
	//some data members used in this function
	int numsubfolders = dir.size();
	int numprocs, rank;
	//	int workerlastrecieved = 0;
	MPI_Status status;
	std::ifstream fin;
	std::ofstream fout;
	int numtodo;
	std::string base_dir = "bfiles_";
	base_dir.append(filename);	
	std::string finishedfile = base_dir;
	finishedfile.append("/finished");
	std::string lastoutfilename0 = base_dir;
	lastoutfilename0.append("/step2/lastnumsent0");	
	std::string lastoutfilename1 = base_dir;
	lastoutfilename1.append("/step2/lastnumsent1");	
	
	
	
#ifdef timingstep2
	double t_start, t_initial, t_receive=0, t_send=0, t_write=0, t1;//, t2, t3, t4, t_end; //for timing the whatnot
	int writecounter = 0, sendcounter = 0, receivecounter = 0;
	t_start = omp_get_wtime();
	std::ofstream timingout;
	std::string timingname = base_dir;
	timingname.append("/timing/mastertiming");
#endif 
	
	/* Find out how many processes there are in the default
     communicator */
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	
	
	
	
	
	
	
	
	
	//begin parse
	int numfunct;
	int numvar;
	int numparam;
	int numconsts;
	std::vector<std::string> FunctVector; 
	std::vector<std::string> VarGroupVector; 
	std::vector<std::string> ParamVector; 
	std::vector<std::string> ParamStrings;
	std::vector<std::string> Consts;
	std::vector<std::string> ConstantStrings;
	std::vector< std::vector< std::pair<double,double> > > Values;
	std::vector< std::pair<double,double> > RandomValues; 
	bool userdefined;
	std::vector< int > NumMeshPoints;
	fin.open(filename.c_str());
	//reparse.  waaah.
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
			  Values,//captures both user-defined and program-generated
			  RandomValues,//we will overwrite, because will be wrong.  load from a file in the bfiles folder
			  userdefined,
			  fin,//yep
			  NumMeshPoints,//a vector of integers, indicating the number of points for each parameter
			  filename);
	fin.close();
	//end parse
	
#ifdef verbosestep2
	std::cout << "parsed\n";	
#endif
	
	std::vector< int > lastnumsent;
	GetLastNumSent(base_dir,
				   lastnumsent,
				   numprocs);
	
	//get random values.  probably destroyed during parsing process.  gotta do this after parsing.
	GetRandomValues(base_dir,
					RandomValues);
	
	
	
	
	//open mc file for writing out to
	std::string mcfname = base_dir;
	mcfname.append("/mc");
	std::ofstream mcfout;
	if (lastnumsent.size()>1) {
		mcfout.open(mcfname.c_str(), std::ios::out | std::ios::app);	
	}
	else {
		mcfout.open(mcfname.c_str());	
	}
	//end open mc file
	
	std::vector< std::string > startvector;
	std::vector< std::string > configvector;
	GetStartConfig(base_dir,
				   startvector,
				   configvector);
	
	
	
	
	
	
	
	std::vector< std::pair<double,double> > TmpValues;
	TmpValues.resize(numparam);
	std::vector< int > indexvector;//for forming a subscript from an index, for !userdefined case
	indexvector.resize(numparam);
	
	std::vector< int > KVector;//same

	double ParamSends[(numprocs-1)*numfilesatatime*(2*numparam+1)];//for sending to workers, to print into datacollected files
	int I, J, index;
	double a, b;//real and imaginary parts of the parameter values.
	std::stringstream ssdeleteme;
	int localcounter;//how many times through the loop
	int terminationint;//how many solves to do total.
	if (!userdefined) {
		KVector.push_back(1);
		for (int i=1; i<numparam; ++i) {
			KVector.push_back(KVector[i-1]*NumMeshPoints[i-1]);
		}
		terminationint = KVector[numparam-1]*NumMeshPoints[numparam-1];
	}
	else {
		terminationint = Values.size();
	}
	
	
	
	
	
	/////////////////////////
	//
	//  end initial setup for master
	//
	//////////////////////
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
#ifdef verbosestep2
	std::cout << "starting initial stuff\n";
#endif
	
	//////////////////////////////
	//
	//        initial writes and sends
	//
	//////////////////////////
	
	
	
	
	
	
	if (lastnumsent.size()==1) {
		for (int i=1; i<numprocs; ++i) {
			lastnumsent.push_back((i-1)*numfilesatatime);
		}
	}
	
	
	
	
	
	
	std::vector<std::pair<double, double> > tmprandomvalues = MakeRandomValues(numparam);
	for (int ii=1; ii<numprocs; ++ii) {
		std::stringstream myss;
		myss << base_dir << "/step2/tmp/init" << ii;
		std::string initrunfolder = "";
		myss >> initrunfolder;
		myss.clear();
		myss.str("");
		mkdirunix(initrunfolder.c_str());
		std::string initruninput = initrunfolder;
		initruninput.append("/input");
		
		
		
		std::cout << initruninput << "\n";
		
		fout.open(initruninput.c_str());	  
		WriteStep2(configvector,
				   fout,
				   tmprandomvalues, //  <------- this, in this call, has been replaced from TmpValues to tmprandomvalues.
				   FunctVector, 
				   VarGroupVector,
				   ParamVector,
				   ParamStrings,
				   Consts,
				   ConstantStrings,
				   RandomValues,
				   numfunct,
				   numvar,
				   numparam,
				   numconsts);
		fout.close();
		
		
		//write the start files.  only need to do this once.
		std::string initrunstart = initrunfolder;
		initrunstart.append("/start");
//		std::cout << initrunstart << "\n";
		fout.open(initrunstart.c_str());
		for (int j=0; j<startvector.size(); ++j) {
			fout << startvector[j];
			if (startvector[j].length() > 0) {
				fout << ";";
			}
			
			fout  << "\n";
		}
		fout.close();		
	}
	
	
	
	
	
	
#ifdef timingstep2
	t_initial = omp_get_wtime() - t_start;	
#endif	
	
	//to let workers know its ok to do the 2.1 solve
	int arbitraryint =0 ;
//	std::cout << "letting workers know its ok to start w init.\n";
	MPI_Bcast(&arbitraryint, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	
	
	
	
	
	///////////////
	//
	//    create the input files for the initial seed of workers
	//
	///////////////
	int biggestnumsent = 0;
	int loopcounter = 0;
	for (int proc_count=1;proc_count<(numprocs);++proc_count){
		
		
		std::cout << (proc_count-1) << "\n";
		//write the start files.  only need to do this once.			
		std::string curbase_dir=dir[proc_count-1];
		curbase_dir.append("/start");
#ifdef timingstep2
		t1 = omp_get_wtime();
#endif	
		std::cout << curbase_dir << "\n";
		fout.open(curbase_dir.c_str());
		for (int j=0; j<startvector.size(); ++j) {
			fout << startvector[j];
			if (startvector[j].length() > 0) {
				fout << ";";
			}
			
			fout  << "\n";
		}
		fout.close();
#ifdef timingstep2
		t_write+= omp_get_wtime() - t1;
		writecounter++;
#endif
		
		
		std::cout << "lastnumsent[proccount] " << lastnumsent[proc_count] << "\n";
		
		localcounter=0; //how many times have done the next inner loop
		for (int i = lastnumsent[proc_count]; (i < lastnumsent[proc_count]+numfilesatatime) && (i<terminationint  ); ++i){
			
			
			//determine the parameter values for this file
			if (userdefined){
				TmpValues = Values[i];
				for (int j=0; j<numparam; ++j) {
					ParamSends[loopcounter*(2*numparam+1)+2*j] = Values[i][j].first;//
					ParamSends[loopcounter*(2*numparam+1)+2*j+1] = Values[i][j].second;//
				}
				ParamSends[loopcounter*(2*numparam+1)+2*numparam] = i;//the line number in mc file
			}
			
			else {//not user defined
				//form index
				index = i;
				for (int j=numparam-1; j>-1; j--) {
					I = (index)%KVector[j];
					J = (index - I)/KVector[j];
					indexvector[j] = J;
					index = I;
				}
				
				//get param values
				for (int j=0; j<numparam; ++j) {
					a = Values[j][ indexvector[j] ].first;
					b = Values[j][ indexvector[j] ].second;
					TmpValues[j].first = a;
					TmpValues[j].second = b;
					ParamSends[loopcounter*(2*numparam+1) + 2*j] = a;//for sending to worker, to write to data file
					ParamSends[loopcounter*(2*numparam+1) + 2*j+1] = b;
				}
				ParamSends[loopcounter*(2*numparam+1) + 2*numparam] = i;//the line number in the mc file
			}
			
			
#ifdef timingstep2
			t1 = omp_get_wtime();
#endif			
			//write the mc line
			for (int j=0; j<numparam; ++j) {
				mcfout << TmpValues[j].first << " " << TmpValues[j].second << " ";
			}
			mcfout << "\n";
			
#ifdef timingstep2
			t_write+= omp_get_wtime() - t1;
			writecounter++;
#endif
			
			//increment
			localcounter++;
			loopcounter++;//for keeping track of folder to write to
			if (i>biggestnumsent){
				biggestnumsent=i;
			}
		}		//re: for int i=lastnumsent[proc_count]	  
	}//re: for int proc_count=1:numprocs
	
	
	//to let the workers know its ok to move on, and to let head node know workers are done with initial solve
	MPI_Bcast(&biggestnumsent, 1, MPI_INT, 0, MPI_COMM_WORLD);
//	std::cout << "bcast indicating all workers done w init\n";
	
	
	double tempsends[numfilesatatime*(2*numparam+1)];
	
	/* Seed the slaves; send one unit of work to each slave. */
	for (rank = 1; rank < numprocs; ++rank) {  //no rank 0, because 0 is head node
		
		localcounter=0;
		for (int i=(rank-1)*numfilesatatime; i<rank*numfilesatatime; ++i) {
			for (int j=0; j<2*numparam+1; ++j) {
				tempsends[localcounter*(2*numparam+1)+j] = ParamSends[i*(2*numparam+1)+j];
			}	  
			localcounter++;
		}
		lastnumsent[rank] = int(tempsends[2*numparam]);
		
		
		
#ifdef verbosestep2
		std::cout << "to process : " << rank << "with numfilesatatime " << numfilesatatime 
		<< "\n";
#endif
		
		
#ifdef timingstep2
		t1 = omp_get_wtime();
#endif
		MPI_Send(&tempsends,      /* message buffer */
				 numfilesatatime*(2*numparam+1),                 /* one data item */
				 MPI_DOUBLE,           /* data item is an integer */
				 rank,              /* destination process rank */
				 numfilesatatime,           /* user chosen message tag */
				 MPI_COMM_WORLD);   /* default communicator */
#ifdef timingstep2
		t_send += omp_get_wtime()-t1;
		sendcounter++;
#endif
	}//re: initial sends
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	//////////////////////////////
	//
	//        loop over rest of points.
	//
	//////////////////////////
	
	
	
	
	/* Loop over getting new work requests 
     until there is no more work
     to be done */
	int filereceived = 0;
	int lastoutcounter=1;//for writing the lastnumsent file.  every so many, we write the file.
	int countingup = biggestnumsent+1; //essentially the last linenumber of the mc file.
	
	while (countingup < terminationint) {
		
		//figure out how many files to do
		numtodo = numfilesatatime;
		if (numtodo+countingup>=terminationint) {
			numtodo = terminationint - countingup;
		}
		
		
		
#ifdef timingstep2
		t1 = omp_get_wtime();
#endif
		/* Receive results from a slave */
		MPI_Recv(&filereceived,       /* message buffer */
				 1,                 /* one data item */
				 MPI_INT,           /* of type int */
				 MPI_ANY_SOURCE,    /* receive from any sender */
				 MPI_ANY_TAG,       /* any type of message */
				 MPI_COMM_WORLD,    /* default communicator */
				 &status);          /* info about the received message */
#ifdef timingstep2
		t_receive+= omp_get_wtime() - t1;
		receivecounter++;
#endif
		
		
#ifdef verbosestep2
		std::cout << "received from : " << status.MPI_SOURCE << "\n";
#endif
		
		
		
		
	    
		localcounter=0;
	    for (int i = (status.MPI_SOURCE-1)*numfilesatatime; i < (status.MPI_SOURCE-1)*numfilesatatime+numtodo; ++i){
			
			FormNextValues(numfilesatatime, userdefined,numparam,localcounter,Values,countingup,KVector,tempsends);
						
			
#ifdef timingstep2
			t1 = omp_get_wtime();
#endif			//write the mc line
			for (int j=0; j<numparam; ++j) {
				mcfout << tempsends[localcounter*(2*numparam+1)+2*j] << " " << tempsends[localcounter*(2*numparam+1)+2*j+1] << " ";
			}
			mcfout << "\n";
#ifdef timingstep2
			t_write+= omp_get_wtime()-t1;
			writecounter++;
#endif
			//increment
			localcounter++;
			countingup++;
	    } //re: write files in the step2 loop
		
		
		
#ifdef timingstep2
		t1 = omp_get_wtime();
#endif
		/* Send the slave a new work unit */
		MPI_Send(&tempsends,             /* message buffer */
				 numfilesatatime*(2*numparam+1),                 /* how many data items */
				 MPI_DOUBLE,           /* data item is an integer */
				 status.MPI_SOURCE, /* to who we just received from */
				 numtodo,           /* user chosen message tag */
				 MPI_COMM_WORLD);   /* default communicator */
#ifdef timingstep2
		t_send+= omp_get_wtime()-t1;
		sendcounter++;
#endif
		
		
		
		lastnumsent[status.MPI_SOURCE] = int(tempsends[(2*numparam)]);
		
		if ((lastoutcounter%saveprogresseverysomany)==0) {
#ifdef timingstep2
			t1 = omp_get_wtime();
#endif
			std::ofstream lastout;
			if (lastoutcounter%(2*saveprogresseverysomany)==0) {
				lastout.open(lastoutfilename1.c_str());
			}
			else {
				lastout.open(lastoutfilename0.c_str());
			}
			
			
			for (int i=1; i<numprocs; ++i) {
				lastout << lastnumsent[i] << "\n";
			}		
			lastout.close();
			
#ifdef timingstep2
			t_write += omp_get_wtime() - t1;
			writecounter++;
#endif
		}
		
		
		lastoutcounter++;//essentially counts this loop.  will take mod 10, for the time being, and if 0, will write lastout
	} //re: while loop
	
	////////////
	//
	//     end while
	//
	/////////////
	
	
	
	
	
	
	
	
	
	/* There's no more work to be done, so receive all the outstanding
     results from the slaves. */
	double arbitrarydouble = -1;//this way the number of things sent matches the number expected to recieve.
	
	
	for (rank = 1; rank < numprocs; ++rank) {
		
#ifdef timingstep2
		t1 = omp_get_wtime();
#endif
		
		MPI_Recv(&filereceived, 
				 1, 
				 MPI_INT, 
				 MPI_ANY_SOURCE,
				 MPI_ANY_TAG, 
				 MPI_COMM_WORLD, 
				 &status);
#ifdef timingstep2
		t_receive += omp_get_wtime() - t1;
		receivecounter++;
#endif
		
#ifdef verbosestep2
		std::cout << "finally received from " << status.MPI_SOURCE << "\n";
		std::cout << "Sending kill tag to rank " << rank << "\n";
#endif
#ifdef timingstep2
		t1 = omp_get_wtime();
#endif
		MPI_Send(&arbitrarydouble, 1, MPI_DOUBLE, rank, 0, MPI_COMM_WORLD);//added arbitraryfloat to make number send match number received.  db.
#ifdef timingstep2
		t_send += omp_get_wtime()-t1;
		sendcounter++;
#endif
		
	}
	
	
#ifdef timingstep2
	t1 = omp_get_wtime();
#endif
	
	mcfout.close();
	fout.open(finishedfile.c_str());
	fout << 1;
	fout.close();
	
#ifdef timingstep2
	t_write += omp_get_wtime() - t1;
	writecounter++;
#endif
	
	
#ifdef timingstep2
	timingout.open(timingname.c_str(),std::ios::out);
	timingout <<  "initialize: " << t_initial << "\n"
	<< "write time: " << t_write << "\n"
	<< "send time: " << t_send << "\n"
	<< "receive time: " << t_receive << "\n"
	<< "total time: " << omp_get_wtime() - t_start << "\n";
	timingout.close();
#endif
	
	
}//re:master



void FormNextValues(int numfilesatatime,
						int userdefined,
						int numparam,
						int localcounter,	
						std::vector< std::vector< std::pair<double,double> > > Values,
						int countingup,
					  std::vector< int > KVector,
					  double tempsends[]){
	int index, I,J;
	std::vector< int > indexvector;//for forming a subscript from an index, for !userdefined case
	indexvector.resize(numparam);
	
	if (userdefined){
		for (int j=0; j<numparam; ++j) {
			tempsends[localcounter*(2*numparam+1)+2*j] = Values[countingup][j].first;//
			tempsends[localcounter*(2*numparam+1)+2*j+1] = Values[countingup][j].second;//
		}
		tempsends[localcounter*(2*numparam+1)+2*numparam] = countingup;//the line number in mc file
	}
	else {
		//get subscripts (zero-based) for the index, countingup
		index = countingup;
		for (int j=numparam-1; j>-1; --j) {
			I = (index)%KVector[j];
			J = (index - I)/KVector[j];
			indexvector[j] = J;
			index = I;
		}
		
		//assign the data
		for (int j=0; j<numparam; ++j) {
			tempsends[localcounter*(2*numparam+1)+2*j] = Values[j][ indexvector[j] ].first;//for sending to workers, to write into data files.
			tempsends[localcounter*(2*numparam+1)+2*j+1] = Values[j][ indexvector[j] ].second;	
			//					a = Values[j][ indexvector[j] ].first;
			//					b = Values[j][ indexvector[j] ].second;
			//					tempsends[localcounter*(2*numparam+1)+2*j] = a;//for sending to workers, to write into data files.
			//					tempsends[localcounter*(2*numparam+1)+2*j+1] = b;
		}
		tempsends[localcounter*(2*numparam+1)+2*numparam] = countingup;//the line number in mc file
	}
	
	return;
	
}


