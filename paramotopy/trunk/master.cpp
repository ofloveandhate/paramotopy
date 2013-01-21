
#include "master.hpp"







void master(std::string filename,
			int numfilesatatime,
			int saveprogresseverysomany,
			std::string called_dir,
			int steptwomode,
			ProgSettings & paramotopy_settings,
			runinfo & paramotopy_info,
			timer & process_timer)
{
	
	
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
	
	
	std::string finishedfile = paramotopy_info.location;
	finishedfile.append("/step2finished");
	std::string lastoutfilename0 = paramotopy_info.location;
	lastoutfilename0.append("/step2/lastnumsent0");
	std::string lastoutfilename1 = paramotopy_info.location;
	lastoutfilename1.append("/step2/lastnumsent1");
	
	
	
	
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
	int smallestnumsent = 0;  

	
	
	std::vector< int > KVector;// for the index making function
	std::ofstream mc_out_stream;
	std::ifstream mc_in_stream;
	
	
	getTermination_OpenMC(mc_in_stream,mc_out_stream,terminationint,KVector,paramotopy_info,paramotopy_settings);
	
		
	
	
	
	
#ifdef verbosestep2
	process_timer.press_start("read");
#endif
	// read in the start file
	std::string start;
	GetStart(paramotopy_info.location,
			 start,  //   <--   reads into memory here
			 paramotopy_settings.settings["MainSettings"]["startfilename"].value());
#ifdef verbosestep2
	process_timer.add_time("read");
#endif
	
	///////////////////
	//
	//       get the start and config files, bcast them
	//
	///////////////////
	

	
#ifdef verbosestep2
	std::cout << "master making input file" << std::endl;
#endif
	

    int vectorlengths[2] = {0};
	vectorlengths[0] = start.size() + 1; // + 1 to account for null character
	
	
	std::string inputstring;

	// for the step 2 memory setup for bertini, we need random values

	std::vector<std::pair<double, double> > tmprandomvalues = paramotopy_info.MakeRandomValues(rand()); 

	
	switch (paramotopy_info.steptwomode) {
		case 2:
			inputstring = WriteStep2(tmprandomvalues,
									 paramotopy_settings,
									 paramotopy_info); // found in the step2_funcs.* files
			
			break;
		case 3:
			inputstring = WriteFailStep2(tmprandomvalues,
										 paramotopy_settings,
										 paramotopy_info); // found in the step2_funcs.* files
			
			break;
		default:
			std::cerr << "bad steptwomode: " << paramotopy_info.steptwomode << " -- exiting!" << std::endl;
			exit(-202);
			break;
	}
	
	vectorlengths[1] = inputstring.size() + 1; // + 1 to account for null character
#ifdef verbosestep2
	std::cout << "the input file is:\n " << inputstring << "\ndone with input file" << std::endl;
#endif

	
	
	
#ifdef verbosestep2
	std::cout << myid << " master is telling workers the number of chars to expect." << std::endl;
#endif
#ifdef timingstep2
	process_timer.press_start("send");
#endif
	
	for (int ii = 1; ii < numprocs; ++ii){
		
		MPI_Send(&vectorlengths[0], 2, MPI_INT, ii, 12, MPI_COMM_WORLD);
	}
#ifdef timingstep2
	process_timer.add_time("send",numprocs-1);
#endif
	
	
#ifdef verbosestep2
	std::cout << "sending start, config, and input." << std::endl;
#endif
	
	
	
	char *start_send = new char[vectorlengths[0]];
	for (int ii = 0; ii < start.size(); ++ii){
		start_send[ii] = start[ii];
	}
	start_send[start.size()] = '\0';
	
#ifdef timingstep2
	process_timer.press_start("send");
#endif
	for (int ii = 1; ii < numprocs; ++ii){
		MPI_Send(&start_send[0], vectorlengths[0], MPI_CHAR, ii, 13, MPI_COMM_WORLD);
	}
#ifdef timingstep2
	process_timer.add_time("send",numprocs-1);
#endif
	
	
	

	char *input_send = new char[vectorlengths[1]];
	for (int ii = 0; ii < inputstring.size(); ++ii){
		input_send[ii] = inputstring[ii];
	}
	input_send[inputstring.size()] = '\0';
	
#ifdef timingstep2
	process_timer.press_start("send");
#endif
	for (int ii = 1; ii < numprocs; ++ii){
		MPI_Send(&input_send[0], vectorlengths[1], MPI_CHAR, ii, 14, MPI_COMM_WORLD);
	}
#ifdef timingstep2
	process_timer.add_time("send",numprocs-1);  //one for each worker
#endif
	

	
	
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
	
	
	
	
	

	std::vector< std::pair<double,double> > TmpValues;
	TmpValues.resize(paramotopy_info.numparam);
	
	std::vector< int > indexvector;//for forming a subscript from an index, for !userdefined case
	indexvector.resize(paramotopy_info.numparam);
	
	
	double* ParamSends = new double[(numprocs-1)*numfilesatatime*(2*paramotopy_info.numparam+1)];//for sending to workers, to print into datacollected files
	int I, J, index;
	double a, b;//real and imaginary parts of the parameter values.
	std::stringstream ssdeleteme;
	int localcounter;//how many times through the loop
	
	
	
	
	//
	//        If userdefined, read the initial parameter values into memory...
	//

	
	if (paramotopy_info.userdefined) {
		paramotopy_info.Values.clear();
		std::string temp;
		
		
		for (int jj=0; (jj < (numprocs-1)*numfilesatatime) && ( (jj+smallestnumsent) < terminationint ) ; ++jj) {
			
			getline(mc_in_stream,temp);
			std::vector< std::pair<double, double> > CurrentValue;
			std::stringstream ss;
			ss << temp;
			for (int i = 0; i < paramotopy_info.numparam;++i){
				double creal;
				double cimaginary;
				ss >> creal;
				ss >> cimaginary;
				CurrentValue.push_back(std::pair<double,double>(creal,cimaginary));
			}
			
			paramotopy_info.Values.push_back(CurrentValue);
		}
	} // if userdefined
	
	
	
	
	
	
	
	////////////////////////////////
	//
	//        seed the lastnumsent vector
	//
	/////////////////////////////
	lastnumsent.clear();
	for (int ii=1; ii<numprocs; ++ii) {
		lastnumsent.push_back(smallestnumsent + (ii-1)*numfilesatatime);
#ifdef verbosestep2
		std::cout << "lastnumsent[" << ii-1 << "] = " << lastnumsent[ii-1] << std::endl;
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
		
		
		numtoprocess_first[proc_count-1] = std::min( numfilesatatime, std::max(terminationint-lastnumsent[proc_count-1],0) ); // this is a local number, used to determine how much to seed to this worker, for the first round.
		for (int i = lastnumsent[proc_count-1]; (i < lastnumsent[proc_count-1]+numfilesatatime) && (i<terminationint  ); ++i){
			
			
			//determine the parameter values for this file
			if (paramotopy_info.userdefined){
				//already made the values above
				TmpValues = paramotopy_info.Values[loopcounter];  // dont worry, loopcounter is incremented...
				for (int jj=0; jj<paramotopy_info.numparam; ++jj) {
					ParamSends[loopcounter*(2*paramotopy_info.numparam+1)+2*jj] = TmpValues[jj].first;//
					ParamSends[loopcounter*(2*paramotopy_info.numparam+1)+2*jj+1] = TmpValues[jj].second;//
				}
				ParamSends[loopcounter*(2*paramotopy_info.numparam+1)+2*paramotopy_info.numparam] = i;//the line number in mc file
			}
			else {//not user defined
				//form index for matrix of parameter values
				index = i;
				for (int jj=paramotopy_info.numparam-1; jj>-1; jj--) {
					I = (index)%KVector[jj];
					J = (index - I)/KVector[jj];
					indexvector[jj] = J;
					index = I;
				}
				
				//get param values
				for (int jj=0; jj<paramotopy_info.numparam; ++jj) {
					a = paramotopy_info.Values[jj][ indexvector[jj] ].first;
					b = paramotopy_info.Values[jj][ indexvector[jj] ].second;
					TmpValues[jj].first = a;
					TmpValues[jj].second = b;
					ParamSends[loopcounter*(2*paramotopy_info.numparam+1) + 2*jj] = a;//for sending to worker, to write to data file
					ParamSends[loopcounter*(2*paramotopy_info.numparam+1) + 2*jj+1] = b;
				}
				ParamSends[loopcounter*(2*paramotopy_info.numparam+1) + 2*paramotopy_info.numparam] = i;//the line number in the mc file
			}
			
			
#ifdef timingstep2
			process_timer.press_start("write");
#endif
			//write the mc line, but only if on an automated mesh
			if (!paramotopy_info.userdefined) {
				for (int jj=0; jj<paramotopy_info.numparam; ++jj) {
					mc_out_stream << TmpValues[jj].first << " " << TmpValues[jj].second << " ";
				}
				mc_out_stream << std::endl;
			}
	
#ifdef timingstep2
			process_timer.add_time("write");
#endif
			
			//increment
			loopcounter++;    //for keeping track of folder to write to
		}//re: for int i=lastnumsent[proc_count]
		
	}//re: for int proc_count=1:numprocs
	
	biggestnumsent = smallestnumsent + (numprocs-1)*numfilesatatime - 1;
	

	
	
	
	
	
	
	///////////////////////////
	//
	//         Initially Seed the slaves; send one unit of work to each slave.
	//
	///////////////////////////////////
	
	
	for (rank = 1; rank < numprocs; ++rank) {  //no rank 0, because 0 is head node
		double* tempsends = new double[numfilesatatime*(2*paramotopy_info.numparam+1)];
		memset(tempsends, 0, numfilesatatime*(2*paramotopy_info.numparam+1)*sizeof(double) );
		
		localcounter=0;
		if (numtoprocess_first[rank-1]!=0) {
			for (int i=lastnumsent[rank-1]; i<lastnumsent[rank-1] + numtoprocess_first[rank-1];++i){
				for (int jj=0; jj<2*paramotopy_info.numparam+1; ++jj) {
					tempsends[localcounter*(2*paramotopy_info.numparam+1)+jj] = ParamSends[i*(2*paramotopy_info.numparam+1)+jj];
				} // end for
				localcounter++;
			} // end for
		} // end if
		
		int sendymcsendsend;
		if (numtoprocess_first[rank-1] == 0) {
			sendymcsendsend = norunflag;
		} // end if
		else {
			sendymcsendsend = numtoprocess_first[rank-1];
		} // end else
		
		
		
#ifdef timingstep2
		process_timer.press_start("send");
#endif
		
		
		// this send is for all of the initial work to be sent to the
		// worker nodes (i.e. parampoints 0 thru numprocs-2)
		
		MPI_Send(&tempsends[0],      /* message buffer */
				 numfilesatatime*(2*paramotopy_info.numparam+1),                 /* number of data items */
				 MPI_DOUBLE,           /* data item is an integer */
				 rank,              /* destination process rank */
				 sendymcsendsend,           /* user chosen message tag */
				 MPI_COMM_WORLD);   /* default communicator */
#ifdef timingstep2
		process_timer.add_time("send");
#endif
		delete[] tempsends;
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
	// here!!
	int countingup = biggestnumsent+1; //essentially the last linenumber of the mc file.
	
	while (countingup < terminationint) {
		
		smallestnumsent = countingup;
		//figure out how many files to do
		numtodo = numfilesatatime;
		if (numtodo+countingup>=terminationint) {
			numtodo = terminationint - countingup;
		}
		
		
		
#ifdef timingstep2
		process_timer.press_start("receive");
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
		process_timer.add_time("receive");
#endif

		
		
		
		// make tempsends in the step2 loop
		double* tempsends = new double[numfilesatatime*(2*paramotopy_info.numparam+1)];
		memset(tempsends, 0, numfilesatatime*(2*paramotopy_info.numparam+1)*sizeof(double) );
		
		localcounter=0;
		for (int i = (status.MPI_SOURCE-1)*numfilesatatime; i < (status.MPI_SOURCE-1)*numfilesatatime+numtodo; ++i){
			
			
			if (paramotopy_info.userdefined) {
#ifdef timingstep2
				process_timer.press_start("read");
#endif
				FormNextValues_mc(numfilesatatime,paramotopy_info.numparam,localcounter,countingup,mc_in_stream,tempsends);
#ifdef timingstep2
				process_timer.add_time("read");
#endif
			}
			else {
				FormNextValues(numfilesatatime,paramotopy_info.numparam,localcounter,paramotopy_info.Values,countingup,KVector,tempsends);


				if (paramotopy_settings.settings["MainSettings"]["writemeshtomc"].intvalue==1){

#ifdef timingstep2
					process_timer.press_start("write");
#endif			//write the mc line if not userdefined

					for (int jj=0; jj<paramotopy_info.numparam; ++jj) {
						mc_out_stream << tempsends[localcounter*(2*paramotopy_info.numparam+1)+2*jj] << " " << tempsends[localcounter*(2*paramotopy_info.numparam+1)+2*jj+1] << " ";
					}
					mc_out_stream << std::endl;
#ifdef timingstep2
					process_timer.add_time("write");
#endif
				}
			}
			


			//increment
			localcounter++;
			countingup++;
		} //re: make tempsends in the step2 loop
		
		
		
#ifdef timingstep2
		process_timer.press_start("send");
#endif
		/* Send the slave a new work unit */
		MPI_Send(&tempsends[0],             /* message buffer */
				 numfilesatatime*(2*paramotopy_info.numparam+1),                 /* how many data items */
				 MPI_DOUBLE,           /* data item is an integer */
				 status.MPI_SOURCE, /* to who we just received from */
				 numtodo,           /* user chosen message tag */
				 MPI_COMM_WORLD);   /* default communicator */
#ifdef timingstep2
		process_timer.add_time("send");
#endif
		
		
		
		lastnumsent[status.MPI_SOURCE] = smallestnumsent;//int(tempsends[(2*numparam)]);
		
		{
			
			std::stringstream tempss;
			for (int i=1; i<numprocs; ++i) {
				tempss << lastnumsent[i] << "\n";
			}
			
#ifdef timingstep2
			process_timer.press_start("write");
#endif
			std::ofstream lastout;
			if (lastoutcounter%(2*saveprogresseverysomany)==0) {
				lastout.open(lastoutfilename1.c_str());
			}
			else {
				lastout.open(lastoutfilename0.c_str());
			}

			lastout << tempss.str();
			lastout.close();
			lastout.clear();
#ifdef timingstep2
			process_timer.add_time("write");
#endif
			
			
		}
		
		lastoutcounter++;//essentially counts this loop.  will take mod 10, for the time being, and if 0, will write lastout
		delete[] tempsends;
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
		process_timer.press_start("receive");
#endif
		MPI_Recv(&filereceived,
				 1,
				 MPI_INT,
				 MPI_ANY_SOURCE,
				 MPI_ANY_TAG,
				 MPI_COMM_WORLD,
				 &status);
#ifdef timingstep2
		process_timer.add_time("receive");
#endif
		
		
		
#ifdef verbosestep2
		std::cout << "finally received from " << status.MPI_SOURCE << "\n";
		std::cout << "Sending kill tag to rank " << int(status.MPI_SOURCE) << "\n";
#endif
		
		
#ifdef timingstep2
		process_timer.press_start("send");
#endif
		MPI_Send(&arbitrarydouble, 1, MPI_DOUBLE, int(status.MPI_SOURCE), 0, MPI_COMM_WORLD);//send everybody the kill signal.
#ifdef timingstep2
		process_timer.add_time("send");
#endif
		
	}
	
	
#ifdef timingstep2
	process_timer.press_start("write");
#endif
	
	if (paramotopy_settings.settings["MainSettings"]["writemeshtomc"].intvalue==1){
		mc_out_stream.close();
	}
		
	mc_in_stream.close();
	fout.open(finishedfile.c_str());
	fout << 1;
	fout.close();
	
#ifdef timingstep2
	process_timer.add_time("write");
#endif
	
	
	delete[] ParamSends;
	delete[] input_send;
	delete[] start_send;
	
	
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
	for (int jj=numparam-1; jj>-1; --jj) {
		I = (index)%KVector[jj];
		J = (index - I)/KVector[jj];
		indexvector[jj] = J;
		index = I;
	}
	
	//assign the data
	for (int jj=0; jj<numparam; ++jj) {
		tempsends[localcounter*(2*numparam+1)+2*jj] = Values[jj][ indexvector[jj] ].first;//for sending to workers, to write into data files.
		tempsends[localcounter*(2*numparam+1)+2*jj+1] = Values[jj][ indexvector[jj] ].second;
		//					a = Values[jj][ indexvector[jj] ].first;
		//					b = Values[jj][ indexvector[jj] ].second;
		//					tempsends[localcounter*(2*numparam+1)+2*jj] = a;//for sending to workers, to write into data files.
		//					tempsends[localcounter*(2*numparam+1)+2*jj+1] = b;
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
	for (int jj=0; jj<numparam; ++jj) {
		tempsends[localcounter*(2*numparam+1)+2*jj] = CValues[jj].first;//
		tempsends[localcounter*(2*numparam+1)+2*jj+1] = CValues[jj].second;//
	}
	tempsends[localcounter*(2*numparam+1)+2*numparam] = countingup;//the line number in mc file
	
	
	
	return;
	
}


