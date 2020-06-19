
#include "controller.hpp"






void controller_process::controller_main(ProgSettings input_settings,
								 runinfo input_p_info,
								 timer & process_timer)

{
	this->paramotopy_settings = input_settings;
	this->paramotopy_info = input_p_info;
	
	
	//some data members used in this function
	
	controller_process::SetTmpFolder();
	
	controller_process::controllerSetup();
	
	controller_process::SetUpFolders();
	
	controller_process::GetTerminationInt();
	
	controller_process::OpenMC();
	
	controller_process::SendStart(process_timer);
	
	controller_process::SendInput(process_timer);
	
	controller_process::SeedSwitch(process_timer);
	
	controller_process::LoopSwitch(process_timer);
	
	controller_process::CleanupSwitch(process_timer);
	
	
	
}//re:controller


///////////////////
///////////////
///////////
//////////  END controller MAIN FUNCTION
///////////
//////////////
////////////////
///////////////////














void controller_process::controllerSetup()
{
	
	this->numparam = paramotopy_info.numparam;
	this->numfilesatatime = paramotopy_settings.settings["parallelism"]["numfilesatatime"].intvalue;
	this->numfilesatatime_orig = paramotopy_settings.settings["parallelism"]["numfilesatatime"].intvalue;
	
	this->filename = paramotopy_info.inputfilename;
	this->standardstep2 = paramotopy_settings.settings["mode"]["standardstep2"].intvalue;
	
	
	this->lastoutfilename = paramotopy_info.location;
	this->lastoutfilename /= "step2/lastnumsent";
	
}







/////////////////////////////
//
//   some subfunctions
//
////////////////////////




// the controller is responsible for generating the data points to work on, and distributing them to the workers.



void controller_process::NextRandomValue(int pointcounter, // localcounter tells which indices in tempsends
									 double tempsends[]) // holds the created data
{
	MTRand drand;
	
	//assign the data
	for (int ii=0; ii<numparam; ++ii) {
		double rand_real = drand(), rand_imag = drand();
		
		rand_real*=(paramotopy_info.BoundsRight[ii].first
					- paramotopy_info.BoundsLeft[ii].first);
		rand_real+=paramotopy_info.BoundsLeft[ii].first;
		
		rand_imag*=(paramotopy_info.BoundsRight[ii].second
					- paramotopy_info.BoundsLeft[ii].second);
		rand_imag+=paramotopy_info.BoundsLeft[ii].second;
		
		
		tempsends[pointcounter*(2*numparam+1)+2*ii] = rand_real;//for sending to workers, to write into data files.
		tempsends[pointcounter*(2*numparam+1)+2*ii+1] = rand_imag;
	}
	tempsends[pointcounter*(2*numparam+1)+2*numparam] = current_absolute_index;//the line number in mc file
	
	return;
	
}


// FormNextValues generates parameter points from a mesh-style set
void controller_process::FormNextValues(int pointcounter,
									std::vector< std::vector< std::pair<double,double> > > Values,
									double tempsends[]){
	
	int index, I,J;
	std::vector< int > indexvector;//for forming a subscript from an index, for !userdefined case
	indexvector.resize(numparam);
	
	
	
	//get subscripts (zero-based) for the index, current_absolute_index
	index = current_absolute_index;
	for (int jj=numparam-1; jj>-1; --jj) {
		I = (index)%index_conversion_vector[jj];
		J = (index - I)/index_conversion_vector[jj];
		indexvector[jj] = J;
		index = I;
	}
	
	//assign the data
	for (int jj=0; jj<numparam; ++jj) {
		tempsends[pointcounter*(2*numparam+1)+2*jj] = Values[jj][ indexvector[jj] ].first;//for sending to workers, to write into data files.
		tempsends[pointcounter*(2*numparam+1)+2*jj+1] = Values[jj][ indexvector[jj] ].second;
	}
	tempsends[pointcounter*(2*numparam+1)+2*numparam] = current_absolute_index;//the line number in mc file
	
	return;
}





//FormNextValues_mc generates points to send, from a user-defined set
void controller_process::FormNextValues_mc(int pointcounter,	//another integer counter
									   double tempsends[]){ //tempsends holds the values to send
	
	
	std::string temp;
	getline(this->mc_in_stream,temp);
	std::vector< std::pair<double, double> > CValues;
	std::stringstream ss;
	ss << temp;
	
	for (int jj = 0; jj < numparam;++jj){
		ss >> tempsends[pointcounter*(2*numparam+1)+2*jj];
		ss >> tempsends[pointcounter*(2*numparam+1)+2*jj+1];
	}
	
	
	
	tempsends[pointcounter*(2*numparam+1)+2*numparam] = current_absolute_index;//the line number in mc file
	
	
	
	
	return;
	
}


















void controller_process::SeedSwitch(timer & process_timer){
	
	
	
	switch (paramotopy_settings.settings["mode"]["main_mode"].intvalue) {
		case 0:
			controller_process::SeedBasic(process_timer);
			break;
			
		case 1:
			controller_process::SeedSearch(process_timer);
			break;
			
			
		default:
			
			std::cerr << "invalid main_mode setting" << std::endl;
			MPI_Abort(MPI_COMM_WORLD, 54);
			break;
	}
	
}//re: seed process switch



void controller_process::SeedBasic(timer & process_timer){
	
	
	this->current_absolute_index = 0;
	
	double* tempsends = new double[(numprocs-1)*numfilesatatime*(2*paramotopy_info.numparam+1)];//for sending to workers, to print into	datacollected files
	
	
	/////////////////////////
	//     make initial data to send to the workers
	////////////////////////////
	
	
	
	for (int proc_count=1;proc_count<(numprocs);++proc_count){
		int numtodo = std::min( numfilesatatime, std::max(terminationint-current_absolute_index,0) );
		
		
		if (numtodo>0) {
			
			lastnumsent.push_back(current_absolute_index);
			
			for (int ii = 0; ii <numtodo; ++ii){
				//determine the parameter values for this file
				
				if (paramotopy_info.userdefined){
					controller_process::FormNextValues_mc(ii,
													  tempsends);
				}
				else {//not user defined
					controller_process::FormNextValues(ii,
												   paramotopy_info.Values,
												   tempsends);
					
					if (paramotopy_settings.settings["files"]["writemeshtomc"].intvalue==1){
#ifdef timingstep2
						process_timer.press_start("write");
#endif
						for (int jj=0; jj<paramotopy_info.numparam; ++jj) {
							mc_out_stream << tempsends[2*jj]
							<< " " << tempsends[2*jj+1] << " ";
						}
						mc_out_stream << std::endl;
#ifdef timingstep2
						process_timer.add_time("write");
#endif
					}// if
				} // else
				
				//increment
				current_absolute_index++;    //for keeping track of folder to write to
				
			}//re: for int ii=lastnumsent[proc_count]
			
#ifdef timingstep2
			process_timer.press_start("send");
#endif
			/* Send the worker a new work unit */
			MPI_Send(&numtodo,            /* message buffer */
					 1,										/* how many data items */
					 MPI_INT,							/* data item is an integer */
					 proc_count,					/* the destination */
					 NUM_PACKETS,           /* user chosen message tag */
					 MPI_COMM_WORLD);			/* default communicator */
#ifdef timingstep2
			process_timer.add_time("send");
#endif
			
			
			
#ifdef timingstep2
			process_timer.press_start("send");
#endif
			/* Send the worker a new work unit */
			MPI_Send(&tempsends[0],             /* message buffer */
					 numtodo*(2*paramotopy_info.numparam+1),                 /* how many data items */
					 MPI_DOUBLE,           /* data item is an integer */
					 proc_count, /* to who we just received from */
					 DATA_DOUBLE,           /* user chosen message tag */
					 MPI_COMM_WORLD);   /* default communicator */
#ifdef timingstep2
			process_timer.add_time("send");
#endif
			
			
			//now send them here
			
			active_workers[proc_count] = true;
			num_active_workers++;
			
		}//re: if numtodo>0
		else{
			lastnumsent.push_back(-1);
			//just send a kill tag
			controller_process::TerminateInactiveWorker(proc_count,process_timer);
		}
	}//re: for int proc_count=1:numprocs
	
	
	
}


void controller_process::SeedSearch(timer & process_timer){
	
	
	
	
	this->current_absolute_index = 0;
	
	double* tempsends = new double[(numprocs-1)*numfilesatatime*(2*paramotopy_info.numparam+1)];//for sending to workers, to print into	datacollected files
	
	
	/////////////////////////
	//     make initial data to send to the workers
	////////////////////////////
	
	if (paramotopy_info.userdefined){
		std::cerr << "invalid userdefined mode inside search mode" << std::endl;
		MPI_Abort(MPI_COMM_WORLD,438);
	}
	
	
	if (paramotopy_settings.settings["mode"]["search_desirednumber"].intvalue <= (numprocs-1)) {
		numfilesatatime = 1;
	}
	else
	{
		double fractpart, intpart;
		fractpart = modf ( double(paramotopy_settings.settings["mode"]["search_desirednumber"].intvalue)/double((numprocs-1)), &intpart);
		
		
		numfilesatatime = int(std::max(int(intpart), numfilesatatime));
	}
	
	
	for (int proc_count=1;proc_count<(numprocs);++proc_count){
		
		int numtodo = std::min( numfilesatatime, std::max(terminationint-current_absolute_index,0) );
		
		
		if (numtodo>0) {
			
			lastnumsent.push_back(current_absolute_index);
			
			for (int ii = 0; ii <numtodo; ++ii){
				
				
				controller_process::NextRandomValue(ii, // current index in tempsends
												tempsends); // data to fill
				
				if (paramotopy_settings.settings["files"]["writemeshtomc"].intvalue==1){
#ifdef timingstep2
					process_timer.press_start("write");
#endif
					for (int jj=0; jj<paramotopy_info.numparam; ++jj) {
						mc_out_stream << tempsends[2*jj+ii*(2*paramotopy_info.numparam+1)]
						<< " " << tempsends[2*jj+1 +ii*(2*paramotopy_info.numparam+1)] << " ";
					}
					mc_out_stream << std::endl;
#ifdef timingstep2
					process_timer.add_time("write");
#endif
				}// if
				
				//increment
				current_absolute_index++;    //for keeping track of folder to write to
				
			}//re: for int ii=lastnumsent[proc_count]
			
#ifdef timingstep2
			process_timer.press_start("send");
#endif
			/* Send the worker a new work unit */
			MPI_Send(&numtodo,            /* message buffer */
					 1,										/* how many data items */
					 MPI_INT,							/* data item is an integer */
					 proc_count,					/* the destination */
					 NUM_PACKETS,           /* user chosen message tag */
					 MPI_COMM_WORLD);			/* default communicator */
#ifdef timingstep2
			process_timer.add_time("send");
#endif
			
			
			
#ifdef timingstep2
			process_timer.press_start("send");
#endif
			/* Send the worker a new work unit */
			MPI_Send(&tempsends[0],             /* message buffer */
					 numtodo*(2*paramotopy_info.numparam+1),                 /* how many data items */
					 MPI_DOUBLE,           /* data item is an integer */
					 proc_count, /* to who we just received from */
					 DATA_DOUBLE,           /* user chosen message tag */
					 MPI_COMM_WORLD);   /* default communicator */
#ifdef timingstep2
			process_timer.add_time("send");
#endif
			
			
			//now send them here
			
			active_workers[proc_count] = true;
			num_active_workers++;
			
		}//re: if numtodo>0
		else{
			lastnumsent.push_back(-1);
			//just send a kill tag
			controller_process::TerminateInactiveWorker(proc_count,process_timer);
		}
	}//re: for int proc_count=1:numprocs
	
	
	
	
	
	
}




void controller_process::LoopSwitch(timer & process_timer){
	
	
	switch (paramotopy_settings.settings["mode"]["main_mode"].intvalue) {
		case 0:
			controller_process::LoopBasic(process_timer);
			break;
			
		case 1:
			controller_process::LoopSearch(process_timer);
			break;
			
			
		default:
			break;
	}
	
	
	
}//re: while loop switch


void controller_process::LoopBasic(timer & process_timer){
	
	
	MPI_Status status;
	int numtodo;
	/* Loop over getting new work requests
	 until there is no more work
	 to be done */
	
	double* tempsends = new double[numfilesatatime*(2*paramotopy_info.numparam+1)];
	
	
	int lastlineprocessed = -1;
	
	controller_process::ReadyCheck();
	
	
	while (current_absolute_index < this->terminationint) {
		
#ifdef timingstep2
		process_timer.press_start("receive");
#endif
		/* Receive results from a worker */
		MPI_Recv(&lastlineprocessed,       /* message buffer */
				 1,                 /* one data item */
				 MPI_INT,           /* of type int */
				 MPI_ANY_SOURCE,    /* receive from any sender */
				 MPI_ANY_TAG,       /* any type of message */
				 MPI_COMM_WORLD,    /* default communicator */
				 &status);          /* info about the received message */
		
		
#ifdef timingstep2
		process_timer.add_time("receive");
#endif
		
		
		//figure out how many files to do
		numtodo = numfilesatatime;
		if (numtodo+current_absolute_index>=terminationint) {
			numtodo = terminationint - current_absolute_index;
		}
		
		
		
		//		numtodo guaranteed >0
#ifdef timingstep2
		process_timer.press_start("send");
#endif
		/* Send the worker a new work unit */
		MPI_Send(&numtodo,             /* message buffer */
				 1,                 /* how many data items */
				 MPI_INT,           /* data item is an integer */
				 status.MPI_SOURCE, /* to who we just received from */
				 NUM_PACKETS,           /* user chosen message tag */
				 MPI_COMM_WORLD);   /* default communicator */
#ifdef timingstep2
		process_timer.add_time("send");
#endif
		
		
		int smallestnumsent = current_absolute_index;
		
		// make tempsends in the step2 loop
		
		memset(tempsends, 0, numtodo*(2*paramotopy_info.numparam+1)*sizeof(double) );
		
		for (int ii = 0; ii < numtodo; ++ii){
			
			if (paramotopy_info.userdefined==1) {
#ifdef timingstep2
				process_timer.press_start("read");
#endif
				controller_process::FormNextValues_mc(ii,tempsends);
#ifdef timingstep2
				process_timer.add_time("read");
#endif
			}
			else {
				controller_process::FormNextValues(ii,
											   paramotopy_info.Values,
											   tempsends);
				
				
				if (paramotopy_settings.settings["files"]["writemeshtomc"].intvalue==1){
#ifdef timingstep2
					process_timer.press_start("write");
#endif			//write the mc line if not userdefined
					
					for (int jj=0; jj<paramotopy_info.numparam; ++jj) {
						mc_out_stream << tempsends[ii*(2*paramotopy_info.numparam+1)+2*jj] << " "
						<< tempsends[ii*(2*paramotopy_info.numparam+1)+2*jj+1] << " ";
					}
					mc_out_stream << std::endl;
#ifdef timingstep2
					process_timer.add_time("write");
#endif
				}
			}//re: else
			
			
			current_absolute_index++;  //increment
		} //re: make tempsends in the step2 loop
		
		
#ifdef timingstep2
		process_timer.press_start("send");
#endif
		/* Send the worker a new work unit */
		MPI_Send(&tempsends[0],             /* message buffer */
				 numtodo*(2*paramotopy_info.numparam+1),                 /* how many data items */
				 MPI_DOUBLE,           /* data item is an integer */
				 status.MPI_SOURCE, /* to who we just received from */
				 DATA_DOUBLE,           /* user chosen message tag */
				 MPI_COMM_WORLD);   /* default communicator */
#ifdef timingstep2
		process_timer.add_time("send");
#endif
		
		
		
		lastnumsent[status.MPI_SOURCE] = smallestnumsent;
		
		
		std::stringstream tempss;
		for (int ii=1; ii<numprocs; ++ii) {
			tempss << lastnumsent[ii] << "\n";
		}
		
#ifdef timingstep2
		process_timer.press_start("write");
#endif
		std::ofstream lastout;
		lastout.open(lastoutfilename.c_str());
		
		lastout << tempss.str();
		lastout.close();
		lastout.clear();
#ifdef timingstep2
		process_timer.add_time("write");
#endif
		
		
		
	} //re: while loop
	
	
	
	delete[] tempsends;
}//  re: loop basic








void controller_process::LoopSearch(timer & process_timer){
	
	
	
	
	
	MPI_Status status;
	int numtodo;
	/* Loop over getting new work requests
	 until there is no more work
	 to be done */
	
	double* tempsends = new double[numfilesatatime*(2*paramotopy_info.numparam+1)];
	
	
	int num_solns_this_iteration = -1;
	
	controller_process::ReadyCheck();
	
	
	int num_found_solns = 0;
	
	
	while ((num_found_solns < paramotopy_settings.settings["mode"]["search_desirednumber"].intvalue) && (current_absolute_index < terminationint) ) {
		
		
		if (numfilesatatime<numfilesatatime_orig) {
			numfilesatatime += std::min(numfilesatatime_orig-numfilesatatime_orig, 5);
		}
#ifdef timingstep2
		process_timer.press_start("receive");
#endif
		/* Receive results from a worker */
		MPI_Recv(&num_solns_this_iteration,       /* message buffer */
				 1,                 /* one data item */
				 MPI_INT,           /* of type int */
				 MPI_ANY_SOURCE,    /* receive from any sender */
				 MPI_ANY_TAG,       /* any type of message */
				 MPI_COMM_WORLD,    /* default communicator */
				 &status);          /* info about the received message */
#ifdef timingstep2
		process_timer.add_time("receive");
#endif
		
		num_found_solns += num_solns_this_iteration;
		//figure out how many files to do
		numtodo = numfilesatatime;
		if (numtodo+current_absolute_index>=terminationint) {
			numtodo = terminationint - current_absolute_index;
		}
		
		if (num_found_solns>=paramotopy_settings.settings["mode"]["search_desirednumber"].intvalue) {
			controller_process::TerminateActiveWorker(status.MPI_SOURCE, process_timer);
			break;
		}
		else{
			
			//		numtodo guaranteed >0
#ifdef timingstep2
			process_timer.press_start("send");
#endif
			/* Send the worker a new work unit */
			MPI_Send(&numtodo,             /* message buffer */
					 1,                 /* how many data items */
					 MPI_INT,           /* data item is an integer */
					 status.MPI_SOURCE, /* to who we just received from */
					 NUM_PACKETS,           /* user chosen message tag */
					 MPI_COMM_WORLD);   /* default communicator */
#ifdef timingstep2
			process_timer.add_time("send");
#endif
		}
		
		
		if (numtodo>0) {
			
			int smallestnumsent = current_absolute_index;
			
			// make tempsends in the step2 loop
			
			memset(tempsends, 0, numtodo*(2*paramotopy_info.numparam+1)*sizeof(double) );
			
			for (int ii = 0; ii < numtodo; ++ii){
				
				controller_process::NextRandomValue(ii, // current index in tempsends
												tempsends); // data to fill
				
				if (paramotopy_settings.settings["files"]["writemeshtomc"].intvalue==1){
#ifdef timingstep2
					process_timer.press_start("write");
#endif			//write the mc line if not userdefined
					
					for (int jj=0; jj<paramotopy_info.numparam; ++jj) {
						mc_out_stream << tempsends[ii*(2*paramotopy_info.numparam+1)+2*jj] << " "
						<< tempsends[ii*(2*paramotopy_info.numparam+1)+2*jj+1] << " ";
					}
					mc_out_stream << std::endl;
#ifdef timingstep2
					process_timer.add_time("write");
#endif
				}
				
				
				current_absolute_index++;  //increment
			} //re: make tempsends in the step2 loop
			
			
#ifdef timingstep2
			process_timer.press_start("send");
#endif
			/* Send the worker a new work unit */
			MPI_Send(&tempsends[0],             /* message buffer */
					 numtodo*(2*paramotopy_info.numparam+1),                 /* how many data items */
					 MPI_DOUBLE,           /* data item is an integer */
					 status.MPI_SOURCE, /* to who we just received from */
					 DATA_DOUBLE,           /* user chosen message tag */
					 MPI_COMM_WORLD);   /* default communicator */
#ifdef timingstep2
			process_timer.add_time("send");
#endif
			
			
			
			lastnumsent[status.MPI_SOURCE] = smallestnumsent;
			
			
			std::stringstream tempss;
			for (int ii=1; ii<numprocs; ++ii) {
				tempss << lastnumsent[ii] << "\n";
			}
			
#ifdef timingstep2
			process_timer.press_start("write");
#endif
			std::ofstream lastout;
			lastout.open(lastoutfilename.c_str());
			
			lastout << tempss.str();
			lastout.close();
			lastout.clear();
#ifdef timingstep2
			process_timer.add_time("write");
#endif
			
			
		}//re: if numtodo>0
		
		
	} //re: while loop
	
	
	
	delete[] tempsends;
	
	
	
	
}//re: LoopSearch()











////////////////////
//////////////
//////////
////////
//////           CLEANUP FUNCTIONS
////
///
//




void controller_process::CleanupSwitch(timer & process_timer){
	
	
	switch (paramotopy_settings.settings["mode"]["main_mode"].intvalue) {
			
		case 0:
			controller_process::CleanupBasic(process_timer);
			break;
			
		case 1: // also uses the basic cleanup method
			controller_process::CleanupSearch(process_timer);
			break;
			
		default:
			break;
	}
	
	
	int arbitraryinteger = 1;
	MPI_Bcast(&arbitraryinteger, 1, MPI_INT, 0, MPI_COMM_WORLD); // could also be MPI_Barrier()
	
	if (paramotopy_settings.settings["files"]["deletetmpfilesatend"].intvalue==1) {
		boost::filesystem::path temppath(tmpfolder);
		boost::filesystem::remove_all(tmpfolder);
	}
	
	
}//re: cleanup switch



void controller_process::CleanupBasic(timer & process_timer){
	/* There's no more work to be done, so receive all the outstanding
	 results from the workers. */
	
	
	
	int* receive_buffer = new int[1];
	MPI_Status status;
	int killcounter = 0;
	
	if (num_active_workers!=int(active_workers.size())) {
		std::cerr << "discrepancy in number of active workers" << std::endl;
		MPI_Abort(MPI_COMM_WORLD, 778);
	}
	else{

	}
	
	while (num_active_workers>0) {
		
		
#ifdef timingstep2
		process_timer.press_start("receive");
#endif
		MPI_Recv(&receive_buffer[0],
				 1,
				 MPI_INT,
				 MPI_ANY_SOURCE,
				 MPI_ANY_TAG,
				 MPI_COMM_WORLD,
				 &status);
#ifdef timingstep2
		process_timer.add_time("receive");
#endif
		
		controller_process::TerminateActiveWorker(status.MPI_SOURCE, process_timer);
		
		
		std::map< int, bool>::iterator iter;
		std::stringstream printme;
		printme << "remaining active workers:\n";
		for (iter=active_workers.begin(); iter!=active_workers.end(); ++iter) {
			printme << "worker" << iter->first << "\n";
		}
		printme << std::endl;
		
		std::cout << printme.str();
		killcounter++;
	}
	
	
#ifdef timingstep2
	process_timer.press_start("write");
#endif
	
	if (paramotopy_info.userdefined!=1 && paramotopy_settings.settings["files"]["writemeshtomc"].intvalue==1){
		mc_out_stream.clear();
		mc_out_stream.seekp(0, std::ios::beg); // return to the beginning to write the number at the top.
											   // we left plenty of space at the top for this number
		mc_out_stream << current_absolute_index;
		mc_out_stream.close();
	}
	
	if (paramotopy_info.userdefined)
		mc_in_stream.close();
	
	boost::filesystem::path finishedfile = paramotopy_info.location;
	finishedfile /= "step2finished";
	
	std::ofstream fout;
	fout.open(finishedfile.c_str());
	fout << 1;
	fout.close();
	
#ifdef timingstep2
	process_timer.add_time("write");
#endif
	
	delete[] receive_buffer;
	
}//re: cleanup basic



void controller_process::CleanupSearch(timer & process_timer){
	
	controller_process::CleanupBasic(process_timer);
	
}












void controller_process::SendStart(timer & process_timer){
	
	
	
	
#ifdef timingstep2
	process_timer.press_start("read");
#endif
	// read in the start file
	std::string start;
	GetStart(paramotopy_info.location,
			 start,  //   <--   reads into memory here
			 paramotopy_settings.settings["mode"]["startfilename"].value());
#ifdef timingstep2
	process_timer.add_time("read");
#endif
	
	int start_length = start.size() + 1; // + 1 to account for null character
	
	char *start_send = new char[start_length];
	for (int ii = 0; ii <  int(start.size()); ++ii){
		start_send[ii] = start[ii];
	}
	start_send[start.size()] = '\0';
	
	
	
	
#ifdef timingstep2
	process_timer.press_start("send");
#endif
	
	for (int ii = 1; ii < numprocs; ++ii){
		MPI_Send(&start_length, 1, MPI_INT, ii, NUM_CHARACTERS, MPI_COMM_WORLD);
	}
#ifdef timingstep2
	process_timer.add_time("send",numprocs-1);
#endif
	
	
	
	
	
#ifdef timingstep2
	process_timer.press_start("send");
#endif
	for (int ii = 1; ii < numprocs; ++ii){
		MPI_Send(&start_send[0], start_length, MPI_CHAR, ii, TEXT_FILE, MPI_COMM_WORLD);
	}
#ifdef timingstep2
	process_timer.add_time("send",numprocs-1);
#endif
	
	
	delete[] start_send;
	
	
}





void controller_process::SendInput(timer & process_timer){
	
	
	
	
	// for the step 2 memory setup for bertini, we need random values
	std::vector<std::pair<double, double> > tmprandomvalues = paramotopy_info.MakeRandomValues(rand());
	
	
	
	std::string inputstring;
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
	
	
	
#ifdef verbosestep2
	std::cout << "the input file is:\n " << inputstring << std::endl;
#endif
	
	
	int input_length = inputstring.size() + 1; // + 1 to account for null character
#ifdef timingstep2
	process_timer.press_start("send");
#endif
	for (int ii = 1; ii < numprocs; ++ii){
		MPI_Send(&input_length, 1, MPI_INT, ii, NUM_CHARACTERS, MPI_COMM_WORLD);
	}
#ifdef timingstep2
	process_timer.add_time("send",numprocs-1);
#endif
	
	
	char *input_send = new char[input_length];
	for (int ii = 0; ii < int(inputstring.size()); ++ii){
		input_send[ii] = inputstring[ii];
	}
	input_send[inputstring.size()] = '\0';
	
#ifdef timingstep2
	process_timer.press_start("send");
#endif
	for (int ii = 1; ii < numprocs; ++ii){
		MPI_Send(&input_send[0], input_length, MPI_CHAR, ii, TEXT_FILE, MPI_COMM_WORLD);
	}
#ifdef timingstep2
	process_timer.add_time("send",numprocs-1);  //one for each worker
#endif
	
	
	delete[] input_send;
}
















void controller_process::GetTerminationInt(){
	
	if (!paramotopy_info.userdefined) {
		index_conversion_vector.push_back(1);
		for (int ii=1; ii<paramotopy_info.numparam; ++ii) {
			index_conversion_vector.push_back(index_conversion_vector[ii-1]*paramotopy_info.NumMeshPoints[ii-1]);
		}

		if (paramotopy_settings.settings["mode"]["main_mode"].intvalue==0) {
			terminationint = index_conversion_vector[paramotopy_info.numparam-1]*paramotopy_info.NumMeshPoints[paramotopy_info.numparam-1];
		}
		else if (paramotopy_settings.settings["mode"]["main_mode"].intvalue==1){
			terminationint = paramotopy_settings.settings["mode"]["search_iterations"].intvalue;
		}
		
	}
	else {
		terminationint = GetMcNumLines(paramotopy_info.location,paramotopy_info.numparam); // verified correct for both newline terminated and not newline terminated.  dab
	}
	
	return;
	
};





//opens the mc_in or mc_out file stream
void controller_process::OpenMC(){
	
	
	boost::filesystem::path mcfname = paramotopy_info.location;
	mcfname /= "mc";
	
	if (!paramotopy_info.userdefined) {
		boost::filesystem::remove(mcfname);
		
		if (paramotopy_settings.settings["files"]["writemeshtomc"].intvalue==1){
			//open mc file for writing out to
			mc_out_stream.open(mcfname.c_str());
			
			//end open mc file
			if (!mc_out_stream.is_open()){
				std::cerr << "failed to open the parameter value out file: " << mcfname << std::endl;
				exit(11);
				
			}
			mc_out_stream << "                                          " << std::endl;
			//      mc_out_stream << terminationint << std::endl;
		}
		if (paramotopy_settings.settings["files"]["writemeshtomc"].intvalue==1){
			mc_out_stream.precision(16);
		}
	}
	else {
		
		mc_in_stream.open(mcfname.c_str(), std::ios::in);
		if (!mc_in_stream.is_open()){
			std::cerr << "critical error: failed to open mc file to read parameter values.  filename: " << mcfname << std::endl;
			exit(10);
		}
		std::string burnme;
		std::getline(mc_in_stream,burnme); // burn the line containing the number of lines in the file
	}
	
	return;
}



void controller_process::SetTmpFolder(){
	
	
	
	std::stringstream templocation;
	if (paramotopy_settings.settings["files"]["customtmplocation"].intvalue==1){
		templocation << paramotopy_settings.settings["files"]["tempfilelocation"].value();
	}
	else {
		templocation << called_dir;
	}
	templocation << "/bfiles_" << paramotopy_info.inputfilename << "/tmpstep2";
	
	
	tmpfolder = templocation.str();
	
}







void controller_process::SetUpFolders(){
	
	
	boost::filesystem::path DataCollectedbase_dir = paramotopy_info.location;
	DataCollectedbase_dir /= "step2/DataCollected";
	boost::filesystem::create_directories(DataCollectedbase_dir);
	
	//make text file with names of folders with data in them
	boost::filesystem::path mydirfname = paramotopy_info.location;
	mydirfname /= "folders";
	std::ofstream fout(mydirfname.c_str());
	if (!fout.is_open()){
		std::cerr << "failed to open " << mydirfname.string() << " to write folder names!\n";
	}
	else{
		for (int i = 1; i < numprocs;++i){
			std::stringstream converter;
			converter << i;
			
			boost::filesystem::path makeme =  DataCollectedbase_dir;
			makeme /= "c";
			
			makeme += converter.str();
			converter.clear();
			converter.str("");
			
			fout << makeme.string() << "\n";
			boost::filesystem::create_directories(makeme);
		}
	}
	fout.close();
	
}



void controller_process::TerminateInactiveWorker(int worker_id,
											 timer & process_timer)
{
	
	
	
	int arbitraryinteger = 0;
#ifdef timingstep2
	process_timer.press_start("send");
#endif
	/* Send the worker a new work unit */
	MPI_Send(&arbitraryinteger,            /* message buffer */
			 1,										/* how many data items */
			 MPI_INT,							/* data item is an integer */
			 worker_id,					/* the destination */
			 TERMINATE,           /* user chosen message tag */
			 MPI_COMM_WORLD);			/* default communicator */
#ifdef timingstep2
	process_timer.add_time("send");
#endif
	
	if (active_workers.find(worker_id)!=active_workers.end()) {
		std::cerr << "terminating an active worker as an inactive!\n";
		MPI_Abort(MPI_COMM_WORLD,690);
	}
	
}

void controller_process::TerminateActiveWorker(int worker_id,
										   timer & process_timer)
{
	
	
	
	int arbitraryinteger = 0;
#ifdef timingstep2
	process_timer.press_start("send");
#endif
	/* Send the worker a new work unit */
	MPI_Send(&arbitraryinteger,            /* message buffer */
			 1,										/* how many data items */
			 MPI_INT,							/* data item is an integer */
			 worker_id,					/* the destination */
			 TERMINATE,           /* user chosen message tag */
			 MPI_COMM_WORLD);			/* default communicator */
#ifdef timingstep2
	process_timer.add_time("send");
#endif
	
	if (active_workers.find(worker_id)==active_workers.end()) {
		std::cerr << "terminating an INactive worker as active!\n";
		MPI_Abort(MPI_COMM_WORLD,691);
	}
	
	active_workers.erase(worker_id);
	num_active_workers--;
}


void controller_process::ReadyCheck()
{
	
	bool its_all_good = true;
	std::vector< std::string > reported_errors;
	
	
	if (numfilesatatime<0) {
		reported_errors.push_back("unset numfilesatatime");
		its_all_good = false;
	}
	
	
	if (terminationint<0) {
		reported_errors.push_back("unset terminationint");
		its_all_good = false;
	}
	
	if (tmpfolder.string().size()==0) {
		reported_errors.push_back("no tmpfolder");
		its_all_good = false;
	}
	
	
	if (!its_all_good){
		std::cerr << "controller " << myid << " reported its not all good, with bad flags:\n";
		
		for (int ii=0; ii<int(reported_errors.size()); ii++) {
			std::cerr << "controller flag" << ii << " " << reported_errors[ii] << std::endl;
		}
		std::cerr << "\n\nABORTING controller " << myid << std::endl;
		MPI_Abort(MPI_COMM_WORLD, 652);
	}
}






