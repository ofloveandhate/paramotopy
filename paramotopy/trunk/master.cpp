
#include "master.h"







void master(std::vector<std::string> tmpfolderlocs,
			std::string filename,
			int numfilesatatime,
			int saveprogresseverysomany,
			std::string called_dir,
			std::string templocation,
			std::string location){
	
	std::string homedir = getenv("HOME");
	std::string settingsfilename = homedir;
	settingsfilename.append("/.paramotopy/paramotopyprefs.xml");
	ProgSettings paramotopy_settings(settingsfilename);
	paramotopy_settings.load();
	
	
#ifdef verbosestep2
	std::cout << "starting initial stuff master\n";
#endif
	
	
	//some data members used in this function

	int numprocs, rank;
	int terminationint;//how many solves to do total.
	
	int myid;
	MPI_Comm_rank(MPI_COMM_WORLD,&myid);
	MPI_Status status;
	std::ifstream fin;
	std::ofstream fout;
	int numtodo;

	runinfo paramotopy_info;  //holds all the info for the run 
	
	paramotopy_info.GetInputFileName(filename); //dear god please don't fail to find the file.
	//std::cout << "got file name\n";
	paramotopy_info.ParseData(location);
	//std::cout << "parsed input file\n";
	paramotopy_info.location = location;
	
	std::string finishedfile = paramotopy_info.location;
	finishedfile.append("/step2finished");
	std::string lastoutfilename0 = paramotopy_info.location;
	lastoutfilename0.append("/step2/lastnumsent0");	
	std::string lastoutfilename1 = paramotopy_info.location;
	lastoutfilename1.append("/step2/lastnumsent1");	
	
	
	
#ifdef timingstep2
	double t_start, t_initial, t_receive=0, t_send=0, t_write=0, t1;//, t2, t3, t4, t_end; //for timing the whatnot
	int writecounter = 0, sendcounter = 0, receivecounter = 0;
	t_start = omp_get_wtime();
	std::ofstream timingout;
	std::string timingname = paramotopy_info.location;
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
	
#ifdef verbosestep2
	std::cout << "parsed\n";	
#endif
	
	
	
	std::vector< int > lastnumsent;
	int smallestnumsent = 0;  //changed to 0, because recovery is broken right now.
	//GetLastNumSent(paramotopy_info.location,  //reads from two files.
								  //lastnumsent, // assigns this vector here
								  //numprocs);
#ifdef verbosestep2
	std::cout << smallestnumsent << "\n";
#endif
	
	
	//get random values from file in base_dir.  probably destroyed during parsing process.  gotta do this after parsing.
	GetRandomValues(paramotopy_info.location,
					paramotopy_info.RandomValues);
	
	
	std::string mcfname = paramotopy_info.location;
	mcfname.append("/mc");
	std::vector< int > KVector;// for the index making function
	std::ofstream mc_out_stream;
	std::ifstream mc_in_stream;
	
	
	if (!paramotopy_info.userdefined) {
		KVector.push_back(1); 
		for (int i=1; i<paramotopy_info.numparam; ++i) {
			KVector.push_back(KVector[i-1]*paramotopy_info.NumMeshPoints[i-1]);
		}
		terminationint = KVector[paramotopy_info.numparam-1]*paramotopy_info.NumMeshPoints[paramotopy_info.numparam-1];


		//open mc file for writing out to
		if (smallestnumsent==0) {  // fresh run
			mc_out_stream.open(mcfname.c_str(), std::ios::out | std::ios::app);	
		}
		else {
			mc_out_stream.open(mcfname.c_str());	
		}
		//end open mc file
		if (!mc_out_stream.is_open()){
			std::cerr << "failed to open the parameter value out file: " << mcfname << "\n";
		}
	}
	else {
		
		terminationint = GetMcNumLines(paramotopy_info.location,paramotopy_info.numparam); // verified correct for both newline terminated and not newline terminated.  dab


		
		mc_in_stream.open(mcfname.c_str(), std::ios::in);
		if (!mc_in_stream.is_open()){
			std::cerr << "critical error: failed to open mc file to read parameter values.  filename: " << mcfname << std::endl;
			exit(10);
		}
	}

	
	
	

	

	
	
	///////////////////
	//
	//       get the start and config files, bcast them
	//
	///////////////////

	std::string start;
	
	
#ifdef verbosestep2
	std::cout << "master making input file" << std::endl;
#endif
	
//	// read in the start file
	GetStart(paramotopy_info.location,
				   start);

	
	// for the step 2.1 solve, we need random values
	std::vector<std::pair<double, double> > tmprandomvalues = paramotopy_info.MakeRandomValues(42);  //the 42 here is irrelevant. it is merely a flag to indicate to use a particular version of the MakeRandomValues function.
	std::string inputstring = WriteStep2(tmprandomvalues,
										 paramotopy_settings,
										 paramotopy_info);
	
	
	int vectorlengths[2] = {0};
	vectorlengths[0] = start.size();
	vectorlengths[1] = inputstring.size();

#ifdef verbosestep2
	std::cout << "the input file is:\n " << inputstring << "\ndone with input file" << std::endl;

	std::cout << myid << " master is telling workers the number of chars to expect." << std::endl;
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
	std::cout << "sending start, config, and input." << std::endl;
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
	TmpValues.resize(paramotopy_info.numparam);
	
	std::vector< int > indexvector;//for forming a subscript from an index, for !userdefined case
	indexvector.resize(paramotopy_info.numparam);
	

	double ParamSends[(numprocs-1)*numfilesatatime*(2*paramotopy_info.numparam+1)];//for sending to workers, to print into datacollected files
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
	std::cout << "letting workers know its ok to start w init." << std::endl;
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
	
	
	if (paramotopy_info.userdefined) {
		paramotopy_info.Values.clear();
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
			for (int i = 0; i < paramotopy_info.numparam;++i){
				double creal;
				double cimaginary;
				ss >> creal;
				ss >> cimaginary;
				CValue.push_back(std::pair<double,double>(creal,cimaginary));
			}
			
			paramotopy_info.Values.push_back(CValue);
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
		std::cout << "lastnumsent[" << i-1 << "] = " << lastnumsent[i-1] << std::endl;
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
		
		

				/////////////////////////
				//     make initial data to send to the workers
				////////////////////////////
				
				
				numtoprocess_first[proc_count-1] = std::min( numfilesatatime, std::max(terminationint-lastnumsent[proc_count-1],0) );
				for (int i = lastnumsent[proc_count-1]; (i < lastnumsent[proc_count-1]+numfilesatatime) && (i<terminationint  ); ++i){
					
					
					//determine the parameter values for this file
					if (paramotopy_info.userdefined){
						//already made the values above
						TmpValues = paramotopy_info.Values[loopcounter];  // dont worry, loopcounter is incremented...
						for (int j=0; j<paramotopy_info.numparam; ++j) {
							ParamSends[loopcounter*(2*paramotopy_info.numparam+1)+2*j] = TmpValues[j].first;//
							ParamSends[loopcounter*(2*paramotopy_info.numparam+1)+2*j+1] = TmpValues[j].second;//
						}
						ParamSends[loopcounter*(2*paramotopy_info.numparam+1)+2*paramotopy_info.numparam] = i;//the line number in mc file
					}
					else {//not user defined
						//form index for matrix of parameter values
						index = i;
						for (int j=paramotopy_info.numparam-1; j>-1; j--) {
							I = (index)%KVector[j];
							J = (index - I)/KVector[j];
							indexvector[j] = J;
							index = I;
						}
						
						//get param values
						for (int j=0; j<paramotopy_info.numparam; ++j) {
							a = paramotopy_info.Values[j][ indexvector[j] ].first;
							b = paramotopy_info.Values[j][ indexvector[j] ].second;
							TmpValues[j].first = a;
							TmpValues[j].second = b;
							ParamSends[loopcounter*(2*paramotopy_info.numparam+1) + 2*j] = a;//for sending to worker, to write to data file
							ParamSends[loopcounter*(2*paramotopy_info.numparam+1) + 2*j+1] = b;
						}
						ParamSends[loopcounter*(2*paramotopy_info.numparam+1) + 2*paramotopy_info.numparam] = i;//the line number in the mc file
					}
					
					
		#ifdef timingstep2
					t1 = omp_get_wtime();
		#endif			
					//write the mc line, but only if on an automated mesh
					if (!paramotopy_info.userdefined) {
						for (int j=0; j<paramotopy_info.numparam; ++j) {
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
	std::cout << "bcast indicating all workers done w init" << std::endl;
#endif
	
	
	
	
	
	
	
	///////////////////////////
	//
	//         Initially Seed the slaves; send one unit of work to each slave. 
	//
	///////////////////////////////////
	
	
	for (rank = 1; rank < numprocs; ++rank) {  //no rank 0, because 0 is head node
		double tempsends[numfilesatatime*(2*paramotopy_info.numparam+1)];
		memset(tempsends, 0, numfilesatatime*(2*paramotopy_info.numparam+1)*sizeof(double) );
		
		localcounter=0;
		if (numtoprocess_first[rank-1]!=0) {
			for (int i=lastnumsent[rank-1]; i<lastnumsent[rank-1] + numtoprocess_first[rank-1];++i){      //old:  (rank-1)*numfilesatatime; i<rank*numfilesatatime; ++i) {
				for (int j=0; j<2*paramotopy_info.numparam+1; ++j) {
					tempsends[localcounter*(2*paramotopy_info.numparam+1)+j] = ParamSends[i*(2*paramotopy_info.numparam+1)+j];
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

		
		
		
		
#ifdef timingstep2
		t1 = omp_get_wtime();
#endif
		MPI_Send(&tempsends,      /* message buffer */
				 numfilesatatime*(2*paramotopy_info.numparam+1),                 /* number of data items */
				 MPI_DOUBLE,           /* data item is an integer */
				 rank,              /* destination process rank */
				 sendymcsendsend,           /* user chosen message tag */
				 MPI_COMM_WORLD);   /* default communicator */
#ifdef timingstep2
		t_send += omp_get_wtime()-t1;
		sendcounter++;
#endif
	}//re: initial sends
	
	
#ifdef verbosestep2
	std::cout << "smallest index seeded: " << smallestnumsent << "\n"
			  << "largest index seeded:  " << biggestnumsent << "\n";
#endif
	
	
	
	
	
	
	
	
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
		double tempsends[numfilesatatime*(2*paramotopy_info.numparam+1)];
		memset(tempsends, 0, numfilesatatime*(2*paramotopy_info.numparam+1)*sizeof(double) );
		
		localcounter=0;
	    for (int i = (status.MPI_SOURCE-1)*numfilesatatime; i < (status.MPI_SOURCE-1)*numfilesatatime+numtodo; ++i){
			if (paramotopy_info.userdefined) {
				FormNextValues_mc(numfilesatatime,paramotopy_info.numparam,localcounter,countingup,mc_in_stream,tempsends);
			}
			else {
				FormNextValues(numfilesatatime,paramotopy_info.numparam,localcounter,paramotopy_info.Values,countingup,KVector,tempsends);
			}			
			
#ifdef timingstep2
			t1 = omp_get_wtime();
#endif			//write the mc line if not userdefined
			if (!paramotopy_info.userdefined) {
				for (int j=0; j<paramotopy_info.numparam; ++j) {
					mc_out_stream << tempsends[localcounter*(2*paramotopy_info.numparam+1)+2*j] << " " << tempsends[localcounter*(2*paramotopy_info.numparam+1)+2*j+1] << " ";
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
				 numfilesatatime*(2*paramotopy_info.numparam+1),                 /* how many data items */
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
	if (!timingout.is_open()) {
		std::cerr << "master timing out file " << timingname << "failed to open" << std::endl;
	}
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




// the master is responsible for generating the data points to work on, and distributing them to the workers.  

// FormNextValues generates parameter points from a mesh-style set
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


//FormNextValues_mc generates points to send, from a user-defined set
void FormNextValues_mc(int numfilesatatime,   //how many points in parameter space to set up
						int numparam,  //the number of parameters in the run
						int localcounter,	//another integer counter
						int countingup,  //an integer counter
					   std::ifstream & mc_in_stream, //the input file stream we will read from
						double tempsends[]){ //tempsends holds the values to send

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
		CValues.push_back(std::pair<double, double>(creal,cimaginary));//really should eliminate CValues
	}

	//set the values in the array
	for (int j=0; j<numparam; ++j) {
			tempsends[localcounter*(2*numparam+1)+2*j] = CValues[j].first;//
			tempsends[localcounter*(2*numparam+1)+2*j+1] = CValues[j].second;//
		}
		tempsends[localcounter*(2*numparam+1)+2*numparam] = countingup;//the line number in mc file


	
	return;
	
}


