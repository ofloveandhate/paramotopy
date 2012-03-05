

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







void master(std::vector<std::string> tmpfolderlocs,
			std::string filename,
			int numfilesatatime,
			int saveprogresseverysomany,
			std::string called_dir,
			std::string templocation){
	
	
#ifdef verbosestep2
	std::cout << "starting initial stuff master\n";
#endif
	
	
	//some data members used in this function

	int numprocs, rank;
	int terminationint;//how many solves to do total.
	

	
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
	
	void *v;
    int flag;
    int norunflag;
	MPI_Comm_get_attr( MPI_COMM_WORLD, MPI_TAG_UB, &v, &flag );
	norunflag = *(int*)v - 1;	
	

	
	
	
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
			  Values,//captures both user-defined and program-generated.  since userdefined could be a big file, it gets read as needed down below.
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
	int smallestnumsent = GetLastNumSent(base_dir,  //reads from two files.
								  lastnumsent, // assigns this vector here
								  numprocs);
#ifdef verbosestep2
	std::cout << smallestnumsent << "\n";
#endif
	
	
	//get random values from file in base_dir.  probably destroyed during parsing process.  gotta do this after parsing.
	GetRandomValues(base_dir,
					RandomValues);
	
	
	std::string mcfname = base_dir;
	mcfname.append("/mc");
	std::vector< int > KVector;// for the index making function
	std::ofstream mc_out_stream;
	std::ifstream mc_in_stream;
	
	
	if (!userdefined) {
		KVector.push_back(1); 
		for (int i=1; i<numparam; ++i) {
			KVector.push_back(KVector[i-1]*NumMeshPoints[i-1]);
		}
		terminationint = KVector[numparam-1]*NumMeshPoints[numparam-1];


		//open mc file for writing out to
		if (smallestnumsent==0) {  // fresh run
			mc_out_stream.open(mcfname.c_str(), std::ios::out | std::ios::app);	
		}
		else {
			mc_out_stream.open(mcfname.c_str());	
		}
		//end open mc file
	}
	else {
		
		int num_lines_mcfile = GetMcNumLines(base_dir,numparam); // verified correct for both newline terminated and not newline terminated.  dab
		terminationint = num_lines_mcfile;

		mc_in_stream.open(mcfname.c_str(), std::ios::in);
		
	}

	
	
	

	
	
	
	
	///////////////////
	//
	//       get the start and config files, bcast them
	//
	///////////////////
//	std::vector< std::string > startvector;
//	std::vector< std::string > configvector;
	std::string start;
	std::string config;
	
	// read in the start and config files
	GetStartConfig(base_dir,
				   start,
				   config);
	
	
	// for the step 2.1 solve, we need random values
	std::vector<std::pair<double, double> > tmprandomvalues = MakeRandomValues(numparam);
	std::string inputstring = WriteStep2(config,
										 tmprandomvalues, //  <------- this, in this call, has been replaced from TmpValues to tmprandomvalues.
										 FunctVector,     // 
										 VarGroupVector,  //    write for the initial 2.1 solve 
										 ParamVector,
										 ParamStrings,
										 Consts,
										 ConstantStrings,
										 RandomValues,
										 numfunct,
										 numvar,
										 numparam,
										 numconsts);	
	
	int vectorlengths[2] = {0};
	vectorlengths[0] = start.size();
	vectorlengths[1] = inputstring.size();



	
	
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
	
	
#ifdef verbosestep2
	std::cout << "sending start, config, and input.\n";
#endif
#ifdef timingstep2
	t1 = omp_get_wtime();
#endif	
	char *start_send = (char*) start.c_str();
	char *input_send = (char*) inputstring.c_str();

	MPI_Bcast(&start_send[0], start.size(), MPI_CHAR, 0, MPI_COMM_WORLD);
	MPI_Bcast(&input_send[0], inputstring.size(), MPI_CHAR, 0, MPI_COMM_WORLD);
#ifdef timingstep2
	t_send += omp_get_wtime()-t1;
	sendcounter++;
	sendcounter++;
#endif
	
	
	
	
	std::vector< std::pair<double,double> > TmpValues;
	TmpValues.resize(numparam);
	
	std::vector< int > indexvector;//for forming a subscript from an index, for !userdefined case
	indexvector.resize(numparam);
	

	double ParamSends[(numprocs-1)*numfilesatatime*(2*numparam+1)];//for sending to workers, to print into datacollected files
	int I, J, index;
	double a, b;//real and imaginary parts of the parameter values.
	std::stringstream ssdeleteme;
	int localcounter;//how many times through the loop

	
	
	

	
	
	
	/////////////////////////
	//
	//    end setup for master
	//
	//////////////////////
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	

	
	//////////////////////////////
	//
	//        initial writes and sends
	//
	//////////////////////////
	
	
	
	
	
#ifdef timingstep2
	t_initial = omp_get_wtime() - t_start;	
#endif
	
	
	
	//to let workers know its ok to do the 2.1 solve
	int arbitraryint =0 ;
	
#ifdef verbosestep2
	std::cout << "letting workers know its ok to start w init.\n";
#endif
#ifdef timingstep2
	t1 = omp_get_wtime();
#endif	
	MPI_Bcast(&arbitraryint, 1, MPI_INT, 0, MPI_COMM_WORLD);
#ifdef timingstep2
	t_send += omp_get_wtime()-t1;
	sendcounter++;
#endif
	
	
	
	
	//////////////////////////////
	//
	//        If userdefined, read the initial parameter values into memory...
	//
	////////////////////////////////
	
	
	if (userdefined) {
		Values.clear();
		std::string temp;
		

		if (smallestnumsent>0) {
			for (int i=0; i < smallestnumsent ; ++i) {
				getline(mc_in_stream,temp);         // burn the already processed lines
			}
		}
		
		
		for (int j=0; (j < (numprocs-1)*numfilesatatime) && ( (j+smallestnumsent) < terminationint ) ; ++j) {
			
			getline(mc_in_stream,temp);
			std::vector< std::pair<double, double> > CValue;
			std::stringstream ss;
			ss << temp;
			for (int i = 0; i < numparam;++i){
				double creal;
				double cimaginary;
				ss >> creal;
				ss >> cimaginary;
				CValue.push_back(std::pair<double,double>(creal,cimaginary));
			}
			
			Values.push_back(CValue);
		}
	} // if userdefined
	
	
	
	
	
	
	
	////////////////////////////////
	//
	//        seed the lastnumsent vector
	//
	/////////////////////////////
	lastnumsent.clear();
	for (int i=1; i<numprocs; ++i) {
		lastnumsent.push_back(smallestnumsent + (i-1)*numfilesatatime);
#ifdef verbosestep2
		std::cout << "lastnumsent[" << i-1 << "] = " << lastnumsent[i-1] << "\n";
#endif
	}


	
	///////////////
	//    
	//    create the necessary files for the workers
	//
	///////////////
	std::vector< int > numtoprocess_first;
	numtoprocess_first.resize(numprocs);
	int biggestnumsent;
	int loopcounter = 0;
	for (int proc_count=1;proc_count<(numprocs);++proc_count){
		
				
//				////////////////////////////////////////
//				//          write the start files.  only need to do this once per worker.
//				////////////////////////////////////////
//				
//				std::string curbase_dir=tmpfolderlocs[proc_count-1];
//				curbase_dir.append("/start");
//		#ifdef timingstep2
//				t1 = omp_get_wtime();
//		#endif	
//				std::cout << curbase_dir << "\n";
//				fout.open(curbase_dir.c_str());
//				for (int j=0; j< int(startvector.size()); ++j) {
//					fout << startvector[j];
//					if (startvector[j].length() > 0) {
//						fout << ";";
//					}
//					
//					fout  << "\n";
//				}
//				fout.close();
//		#ifdef timingstep2
//				t_write+= omp_get_wtime() - t1;
//				writecounter++;
//		#endif
				
				

				/////////////////////////
				//     make initial data to send to the workers
				////////////////////////////
				
				
		numtoprocess_first[proc_count-1] = std::min( numfilesatatime, std::max(terminationint-lastnumsent[proc_count-1],0) );
				for (int i = lastnumsent[proc_count-1]; (i < lastnumsent[proc_count-1]+numfilesatatime) && (i<terminationint  ); ++i){
					
					
					//determine the parameter values for this file
					if (userdefined){
						//already made the values above
						TmpValues = Values[loopcounter];  // dont worry, loopcounter is incremented...
						for (int j=0; j<numparam; ++j) {
							ParamSends[loopcounter*(2*numparam+1)+2*j] = TmpValues[j].first;//
							ParamSends[loopcounter*(2*numparam+1)+2*j+1] = TmpValues[j].second;//
						}
						ParamSends[loopcounter*(2*numparam+1)+2*numparam] = i;//the line number in mc file
					}
					else {//not user defined
						//form index for matrix of parameter values
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
					//write the mc line, but only if on an automated mesh
					if (!userdefined) {
						for (int j=0; j<numparam; ++j) {
							mc_out_stream << TmpValues[j].first << " " << TmpValues[j].second << " ";
						}
						mc_out_stream << "\n";
					}
					
					
		#ifdef timingstep2
					t_write+= omp_get_wtime() - t1;
					writecounter++;
		#endif
					
					//increment
					loopcounter++;//for keeping track of folder to write to			
				}		//re: for int i=lastnumsent[proc_count]	  
		
	}//re: for int proc_count=1:numprocs
	
	biggestnumsent = smallestnumsent + (numprocs-1)*numfilesatatime - 1;
	
	//to let the workers know its ok to move on, and to let head node know workers are done with initial solve
	MPI_Bcast(&biggestnumsent, 1, MPI_INT, 0, MPI_COMM_WORLD);
#ifdef verbosestep2
	std::cout << "bcast indicating all workers done w init\n";
#endif
	
	
	
	
	
	
	
	///////////////////////////
	//
	//         Initially Seed the slaves; send one unit of work to each slave. 
	//
	///////////////////////////////////
	
	
	for (rank = 1; rank < numprocs; ++rank) {  //no rank 0, because 0 is head node
		double tempsends[numfilesatatime*(2*numparam+1)];
		memset(tempsends, 0, numfilesatatime*(2*numparam+1)*sizeof(double) );
		
		localcounter=0;
		if (numtoprocess_first[rank-1]!=0) {
			for (int i=lastnumsent[rank-1]; i<lastnumsent[rank-1] + numtoprocess_first[rank-1];++i){      //old:  (rank-1)*numfilesatatime; i<rank*numfilesatatime; ++i) {
				for (int j=0; j<2*numparam+1; ++j) {
					tempsends[localcounter*(2*numparam+1)+j] = ParamSends[i*(2*numparam+1)+j];
				}	  
				localcounter++;
			}
		}

		int sendymcsendsend;
		if (numtoprocess_first[rank-1] == 0) {
			sendymcsendsend = norunflag;
		}
		else {
			sendymcsendsend = numtoprocess_first[rank-1];
		}

		
		
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
				 sendymcsendsend,           /* user chosen message tag */
				 MPI_COMM_WORLD);   /* default communicator */
#ifdef timingstep2
		t_send += omp_get_wtime()-t1;
		sendcounter++;
#endif
	}//re: initial sends
	
	
	
	std::cout << smallestnumsent << " " << biggestnumsent << "\n";
	
	
	
	
	
	
	
	
	
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
		
		smallestnumsent = countingup;
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
		
		
		
		// make tempsends in the step2 loop
		double tempsends[numfilesatatime*(2*numparam+1)];
		memset(tempsends, 0, numfilesatatime*(2*numparam+1)*sizeof(double) );
		
		localcounter=0;
	    for (int i = (status.MPI_SOURCE-1)*numfilesatatime; i < (status.MPI_SOURCE-1)*numfilesatatime+numtodo; ++i){
			if (userdefined) {
				FormNextValues_mc(numfilesatatime,numparam,localcounter,countingup,mc_in_stream,tempsends);
			}
			else {
				FormNextValues(numfilesatatime,numparam,localcounter,Values,countingup,KVector,tempsends);
			}			
			
#ifdef timingstep2
			t1 = omp_get_wtime();
#endif			//write the mc line if not userdefined
			if (!userdefined) {
				for (int j=0; j<numparam; ++j) {
					mc_out_stream << tempsends[localcounter*(2*numparam+1)+2*j] << " " << tempsends[localcounter*(2*numparam+1)+2*j+1] << " ";
				}
				mc_out_stream << "\n";
			}
#ifdef timingstep2
			t_write+= omp_get_wtime()-t1;
			writecounter++;
#endif
			//increment
			localcounter++;
			countingup++;
	    } //re: make tempsends in the step2 loop
		
		
		
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
		
		
		
		lastnumsent[status.MPI_SOURCE] = smallestnumsent;//int(tempsends[(2*numparam)]);
		
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
			
			std::stringstream tempss;
			for (int i=1; i<numprocs; ++i) {
				tempss << lastnumsent[i] << "\n";
			}		
			lastout << tempss.str();
			tempss.clear();
			tempss.str("");
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
		MPI_Send(&arbitrarydouble, 1, MPI_DOUBLE, rank, 0, MPI_COMM_WORLD);//send everybody the kill signal.
#ifdef timingstep2
		t_send += omp_get_wtime()-t1;
		sendcounter++;
#endif
		
	}
	
	
#ifdef timingstep2
	t1 = omp_get_wtime();
#endif
	
	mc_out_stream.close();
	mc_in_stream.close();
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






/////////////////////////////
//
//   some subfunctions
//
////////////////////////





void FormNextValues(int numfilesatatime,
					int numparam,
					int localcounter,	
					std::vector< std::vector< std::pair<double,double> > > Values,
					int countingup,
					std::vector< int > KVector,
					double tempsends[]){
	
	int index, I,J;
	std::vector< int > indexvector;//for forming a subscript from an index, for !userdefined case
	indexvector.resize(numparam);
	


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

	return;
}



void FormNextValues_mc(int numfilesatatime,
						int numparam,
						int localcounter,	
						int countingup,
					   std::ifstream & mc_in_stream,
						double tempsends[]){

	std::string temp;
	getline(mc_in_stream,temp);
	std::vector< std::pair<double, double> > CValues;
	std::stringstream ss;
	ss << temp;
	for (int i = 0; i < numparam;++i){
		double creal;
		double cimaginary;
		ss >> creal;
		ss >> cimaginary;
		CValues.push_back(std::pair<double, double>(creal,cimaginary));
	}

	for (int j=0; j<numparam; ++j) {
			tempsends[localcounter*(2*numparam+1)+2*j] = CValues[j].first;//
			tempsends[localcounter*(2*numparam+1)+2*j+1] = CValues[j].second;//
		}
		tempsends[localcounter*(2*numparam+1)+2*numparam] = countingup;//the line number in mc file


	
	return;
	
}


