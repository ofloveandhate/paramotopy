#include "slave.hpp"




//////////////////////////////////
//
//         slave functions
//
/////////////////////////////





void slave_process::slave_main(ProgSettings input_settings,
					 runinfo input_p_info,
					 timer & process_timer)
{

	this->paramotopy_settings = input_settings;
	this->paramotopy_info = input_p_info;
	
	

	this->slavegatherer.SlaveSetup(paramotopy_settings,paramotopy_info,this->myid,this->called_dir);
	
	slave_process::SlaveSetup();
	
	slave_process::ReceiveStart(process_timer);

	slave_process::ReceiveInput(process_timer);
	
	slave_process::SetWorkingFolder();
	
	slave_process::PurgeWorkingFolder();
	
	slave_process::MoveToWorkingFolder();

	slave_process::LoopSwitch(process_timer);
	
	slave_process::GoCalledDir();
	
		
	

	return;
}//re:slave_main






void slave_process::SlaveSetup(){
	
	this->numparam = paramotopy_info.numparam;
	this->numfilesatatime = paramotopy_settings.settings["parallelism"]["numfilesatatime"].intvalue;
	this->numfilesatatime_orig = paramotopy_settings.settings["parallelism"]["numfilesatatime"].intvalue;
	
	this->filename = paramotopy_info.inputfilename;
	
	this->standardstep2 = paramotopy_settings.settings["mode"]["standardstep2"].intvalue;
	
}

////////////////////
//////////////
//////////
////////
//////           Seeding methods
////
///
//



void slave_process::SeedSwitch(timer & process_timer)
{

}

void slave_process::SeedBasic(timer & process_timer)
{
	
}



void slave_process::SeedSearch(timer & process_timer)
{
	
}








////////////////////
//////////////
//////////
////////
//////           Loops
////
///
//



void slave_process::LoopSwitch(timer & process_timer)
{
	switch (paramotopy_settings.settings["mode"]["main_mode"].intvalue) {
		case 0:
			slave_process::LoopBasic(process_timer);
			break;
			
		case 1:
			slave_process::LoopSearch(process_timer);
			break;
			
			
		default:
			break;
	}
}



void slave_process::LoopBasic(timer & process_timer)
{
	
	
	
	slave_process::WriteInput();
	slave_process::WriteStart();
	
	
	
#ifdef timingstep2
	process_timer.press_start("bertini");
#endif
	parse_input_file_bertini(this->currentSeed, this->MPType);
#ifdef timingstep2
	process_timer.add_time("bertini");
#endif
	
	
	
	
#ifdef timingstep2
	process_timer.press_start("read");
#endif
	slave_process::ReadDotOut();
#ifdef timingstep2
	process_timer.add_time("read",6);
#endif

	
	
	double* datareceived = new double[numfilesatatime*(2*numparam+1)]; //for catching from the mpi
	
	MPI_Status status;

	
	std::string target_file;
	
	slave_process::ReadyCheck();
	
	int* receive_buffer = new int[1];
	while (1) {
		/* Receive parameter points and line numbers from the master */
		
		
#ifdef timingstep2
		process_timer.press_start("receive");
#endif
		MPI_Recv(&receive_buffer[0], 1, MPI_INT, 0, MPI_ANY_TAG,
						 MPI_COMM_WORLD, &status);
#ifdef timingstep2
		process_timer.add_time("receive");
#endif
		int numtodo = receive_buffer[0];
		
		/* Check the tag of the received message. */
		if (status.MPI_TAG == TERMINATE) {
#ifdef verbosestep2
			std::cout << "worker" << myid << " received kill tag" << std::endl;
#endif
			
#ifdef timingstep2
			process_timer.press_start("write");
#endif
			slavegatherer.WriteAllData();
#ifdef timingstep2
			process_timer.add_time("write");
#endif
			break; //exits the while loop
		}
		
		
		
		linenumber=-1;
		if (numtodo>0) {
		
	#ifdef timingstep2
			process_timer.press_start("receive");
	#endif
						
			MPI_Recv(&datareceived[0], numtodo*(2*numparam+1), MPI_DOUBLE, 0, DATA_DOUBLE,
							 MPI_COMM_WORLD, &status);
			
	#ifdef timingstep2
			process_timer.add_time("receive");
	#endif
			
			
			
			for (int kk = 0; kk < numtodo; ++kk){
				
				linenumber = int(datareceived[kk*(2*numparam+1)+2*numparam]);
				
				double* current_params = new double[(2*numparam+1)];
				for (int mm =0; mm< 2*numparam+1; mm++){
					current_params[mm] = datareceived[kk*(2*numparam+1)+mm];
				}
				
				
#ifdef timingstep2
				process_timer.press_start("write");
#endif
				slave_process::WriteNumDotOut(current_params);
#ifdef timingstep2
				process_timer.add_time("write");
#endif
				
				
				std::cout << "worker" << myid << " solving point " << linenumber << std::endl;
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
				slavegatherer.SlaveCollectAndWriteData(current_params,  paramotopy_settings, process_timer);
#endif
				
			}//re: for (int kk = 0; kk < int(status.MPI_TAG);++kk)
		}//re: if numpointsthisiteration>0

		
#ifdef timingstep2
		process_timer.press_start("send");
#endif
		MPI_Send(&linenumber, 1, MPI_INT, 0, SLAVE_ITERATION_FINISHED, MPI_COMM_WORLD);
#ifdef timingstep2
		process_timer.add_time("send");
#endif
		
	}//re: while (1)
	
	
}

void slave_process::LoopSearch(timer & process_timer)
{
	
	
	slave_process::WriteInput();
	slave_process::WriteStart();
	
	
	
#ifdef timingstep2
	process_timer.press_start("bertini");
#endif
	parse_input_file_bertini(this->currentSeed, this->MPType);
#ifdef timingstep2
	process_timer.add_time("bertini");
#endif
	
	
	
	
#ifdef timingstep2
	process_timer.press_start("read");
#endif
	slave_process::ReadDotOut();
#ifdef timingstep2
	process_timer.add_time("read",6);
#endif
	
	
	
	double* datareceived = new double[numfilesatatime*(2*numparam+1)]; //for catching from the mpi
	
	MPI_Status status;
	
	
	std::string target_file;
	
	slave_process::ReadyCheck();
	
	double* current_params = new double[(2*numparam+1)];
	int* receive_buffer = new int[1];
	
	
	while (1) {
		/* Receive parameter points and line numbers from the master */
		
		
#ifdef timingstep2
		process_timer.press_start("receive");
#endif
		MPI_Recv(&receive_buffer[0], 1, MPI_INT, 0, MPI_ANY_TAG,
						 MPI_COMM_WORLD, &status);
#ifdef timingstep2
		process_timer.add_time("receive");
#endif
		int numtodo = receive_buffer[0];
		
		/* Check the tag of the received message. */
		if (status.MPI_TAG == TERMINATE) {
#ifdef verbosestep2
			std::cout << "worker" << myid << " received kill tag" << std::endl;
#endif
			
#ifdef timingstep2
			process_timer.press_start("write");
#endif
#ifndef nosolve
			slavegatherer.WriteAllData();
#endif
#ifdef timingstep2
			process_timer.add_time("write");
#endif
			break; //exits the while loop
		}
		
		
		
		int num_solns_this_iteration = 0;
		int num_points_with_solns_this_iteration = 0;
		
		if (numtodo>0) {
			
#ifdef timingstep2
			process_timer.press_start("receive");
#endif
			

			MPI_Recv(&datareceived[0], numtodo*(2*numparam+1), MPI_DOUBLE, 0, DATA_DOUBLE,
							 MPI_COMM_WORLD, &status);
			
#ifdef timingstep2
			process_timer.add_time("receive");
#endif
			
			
			
			for (int kk = 0; kk < numtodo; ++kk){
				
				linenumber = int(datareceived[kk*(2*numparam+1)+2*numparam]);
				
				for (int mm =0; mm< 2*numparam+1; mm++){
					current_params[mm] = datareceived[kk*(2*numparam+1)+mm];
				}
				

#ifdef timingstep2
				process_timer.press_start("write");
#endif
				slave_process::WriteNumDotOut(current_params);
#ifdef timingstep2
				process_timer.add_time("write");
#endif
				
				
				
				// Call Bertini
				std::cout << "worker" << myid << " solving point " << linenumber << std::endl;
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
				slavegatherer.SlaveCollectAndWriteData(current_params,  paramotopy_settings, process_timer);
				
				int num_solns_this_solve = slavegatherer.num_reals();
				
				//get the number of found solutions
				if (num_solns_this_solve>0) {
					num_points_with_solns_this_iteration ++;
					num_solns_this_iteration += num_solns_this_solve;
				}
#endif
				
			}//re: for (int kk = 0; kk < int(status.MPI_TAG);++kk)
		}//re: if numpointsthisiteration>0
		
		int sendme=-1;
		switch (paramotopy_settings.settings["mode"]["search_submode"].intvalue) {
			case 0:
				sendme = num_solns_this_iteration;
				break;
				
			case 1:
				sendme = num_points_with_solns_this_iteration;
				break;
				
			default:
				break;
		}
		
#ifdef timingstep2
		process_timer.press_start("send");
#endif
		MPI_Send(&sendme, 1, MPI_INT, 0, SLAVE_ITERATION_FINISHED, MPI_COMM_WORLD);
#ifdef timingstep2
		process_timer.add_time("send");
#endif
		
	}//re: while (1)
	
	
	delete[] receive_buffer;
	delete[] current_params;
}





////////////////////
//////////////
//////////
////////
//////           CLEANUP FUNCTIONS
////
///
//




void slave_process::CleanupSwitch(timer & process_timer)
{

	
	int arbitraryinteger = 1;
	MPI_Bcast(&arbitraryinteger, 1, MPI_INT, 0, MPI_COMM_WORLD); // could also be MPI_Barrier()
	
	
}



void slave_process::CleanupBasic(timer & process_timer)
{
	
}

void slave_process::CleanupSearch(timer & process_timer)
{
	
}






////////////////////
//////////////
//////////
////////
//////           Important/Common Send Functions
////
///
//



void slave_process::ReceiveInput(timer & process_timer)
{
	
	if (this->have_input) {
		delete[] input_file;
	}
	
	
	MPI_Status status;

#ifdef timingstep2
	process_timer.press_start("receive");
#endif
	int input_length;
	MPI_Recv(&input_length, 1, MPI_INT, 0, NUM_CHARACTERS, MPI_COMM_WORLD, &status);
	
	
	input_file = new char[input_length];
	
	
	MPI_Recv(&input_file[0], input_length, MPI_CHAR, 0, TEXT_FILE, MPI_COMM_WORLD, &status);
	
#ifdef timingstep2
	process_timer.add_time("receive");
#endif
	
	have_input = true;
	
}

void slave_process::ReceiveStart(timer & process_timer)
{
	
	MPI_Status status;
	
	int start_length;
	
	if (this->have_start) {
		delete[] start_file;
	}
	
	///////
	//
	//      get the start file from master
	//
	///////////
	

	
#ifdef timingstep2
	process_timer.press_start("receive");
#endif
	
	MPI_Recv(&start_length, 1, MPI_INT, 0, NUM_CHARACTERS, MPI_COMM_WORLD, &status);
	
	
#ifdef timingstep2
	process_timer.add_time("receive");
#endif
	

	start_file = new char[start_length];
	
	
#ifdef timingstep2
	process_timer.press_start("receive");
#endif
	MPI_Recv(&start_file[0], start_length, MPI_CHAR, 0, TEXT_FILE, MPI_COMM_WORLD, &status);
	
#ifdef timingstep2
	process_timer.add_time("receive");
#endif
	
	
	
	have_start = true;
}



void slave_process::WriteStart(){
	
	if (!(this->have_start)) {
		std::cerr << "slave" << myid << " did not have start file when attempting to write it." << std::endl;
		MPI_Abort(MPI_COMM_WORLD,919);
	}
	
	if (!(this->in_working_folder)) {
		std::cerr << "slave" << myid << " was not in working folder before writing start." << std::endl;
		MPI_Abort(MPI_COMM_WORLD,920);
	}
	
	
	std::ofstream fout;
	fout.open("start");
	if (!fout.is_open()) {
		std::cerr << "failed to properly open " << "start" << " to open start file, worker " << myid << std::endl;
		exit(719);
	}
	fout << start_file;
	fout.close(); fout.clear();
}



void slave_process::WriteInput(){

	
	if (!(this->have_input)) {
		std::cerr << "slave" << myid << " did not have input before attempting to write input" << std::endl;
		MPI_Abort(MPI_COMM_WORLD,919);
	}
	
	if (!(this->in_working_folder)) {
		std::cerr << "slave" << myid << " was not in working folder before writing input" << std::endl;
		MPI_Abort(MPI_COMM_WORLD,920);
	}
	
	
	std::ofstream fout;
	fout.open("input");
	if (!fout.is_open()) {
		std::cerr << "failed to properly open " << "input" << " to open input file, worker " << myid << std::endl;
		exit(720);
	}
	fout << input_file;
	fout.close(); fout.clear();
	
}






void slave_process::ReadDotOut(){
	
  //read in Num.out file
  numdotout.clear();
  
  std::ifstream fin;
  std::string copyme;
  
  
  fin.open("num.out");
  
  while (!fin.eof()) {
    getline(fin,copyme);
    numdotout.push_back(copyme);
  }
  fin.close();
  //end read num.out
  
  
  //read in arr.out file
  fin.open("arr.out");
  
  while (!fin.eof()) {
    getline(fin,copyme);
    arrdotout.push_back(copyme);
  }
  fin.close();
  //end read arr.out
  
  
  //read in deg.out file
  fin.open("deg.out");
  
  while (!fin.eof()) {
    getline(fin,copyme);
    degdotout.push_back(copyme);
  }
  fin.close();
  //end read deg.out
  
  //read in names.out file
  fin.open("names.out");
  
  while (!fin.eof()) {
    getline(fin,copyme);
    namesdotout.push_back(copyme);
  }
  fin.close();
  //end read names.out
  
  //read in names.out file
  fin.open("config");
  
  while (!fin.eof()) {
    getline(fin,copyme);
    config.push_back(copyme);
  }
  fin.close();
  //end read names.out
  
  
  //read in names.out file
  fin.open("func_input");
  
  while (!fin.eof()) {
    getline(fin,copyme);
    funcinput.push_back(copyme);
  }
  fin.close();
  //end read names.out
	
  fin.open("preproc_data");
	
  while (!fin.eof()) {
    getline(fin,copyme);
    preproc_data.push_back(copyme);
  }
  fin.close();
  // end read preproc_data
	
	
	
	this->have_dotout = true;
}



void slave_process::WriteDotOut(){
  
  std::ofstream fout;
  
  
  fout.open("arr.out");
  for (int ii = 0; ii< int(arrdotout.size())-1; ii++) {
    fout << arrdotout[ii] << "\n";
  }
  fout.close();
  
  fout.open("deg.out");
  for (int ii = 0; ii< int(degdotout.size())-1; ii++) {
    fout << degdotout[ii] << "\n";
  }
  fout.close();
  fout.open("names.out");
  for (int ii = 0; ii< int(namesdotout.size())-1; ii++) {
    fout << namesdotout[ii] << "\n";
  }
  fout.close();
  
  fout.open("config");
  for (int ii = 0; ii< int(config.size())-1; ii++) {
    fout << config[ii] << "\n";
  }
  fout.close();
  
  fout.open("func_input");
  for (int ii = 0; ii< int(funcinput.size())-1; ii++) {
    fout << funcinput[ii] << "\n";
  }
  fout.close();
	
  fout.open("preproc_data");
	
  for (int ii = 0; ii < int(preproc_data.size())-1; ++ii){
    fout << preproc_data[ii] << "\n";
  }
  fout.close();
	
	
}



void slave_process::WriteNumDotOut(double current_params[]){
  
  //modified 2012.02.19 to make a single write call. db.
  
	
  char *numer = NULL, *denom = NULL;
  //	char *passto_computeNumDenom = NULL;
	
  
  std::string convertmetochar;
  std::stringstream ss, writess;
  std::ofstream fout;
	
  fout.open("num.out");
	
  if (!fout.is_open()) {
    std::cerr << "failed to open num.out" << std::endl;
    exit(741);
  }
	ss.precision(15);
	fout.precision(15);
	
  //write the first two lines
  writess << numdotout[0] << "\n" << numdotout[1] << "\n";
	
  if (standardstep2==1){
		
    //write the random values associated with this run:
    for (int ii=0; ii<2*numparam; ++ii) {
      writess << numdotout[ii+2] << "\n";
      
    }
		
    //write the parameter lines
    for (int ii = 0; ii<numparam; ++ii) {
      if (current_params[2*ii]==0) {
				writess << "0/1 ;\n";
      }
      else {
				ss << current_params[2*ii];
				ss >> convertmetochar;
				ss.clear();
				ss.str("");
				
				//make a call from the bertini library
				computeNumDenom(&numer, &denom, (char *) convertmetochar.c_str());
				
				writess << numer << "/" << denom << " ;\n";
      }
      
      
      if (current_params[2*ii+1]==0) {
				writess << "0/1 ;\n";
      }
      else {
				ss << current_params[2*ii+1];
				ss >> convertmetochar;
				ss.clear();
				ss.str("");
				
				computeNumDenom(&numer, &denom, (char *) convertmetochar.c_str());
				writess << numer << "/" << denom << " ;\n";
      }
      
    }
    
		//put the remainder of the file on the buffer
    for (int ii = (2+4*numparam); ii< int(numdotout.size())-1; ++ii) {
      writess << numdotout[ii] << "\n";
    }
		
		
  } // end standard step2
  else{  // begin nonstandard step2 ... doing a standard total degree
		// bertini run with just parameters changed
    
    // Write the parameter values to be used in the SLP
    for (int ii=0; ii < numparam; ++ii){
      if (current_params[2*ii] == 0){
				writess << "0/1 ; \n";
      }
      else{
        ss << current_params[2*ii];
        ss >> convertmetochar;
        ss.clear();
        ss.str("");
				
        //make a call from the bertini library
        computeNumDenom(&numer, &denom, (char *) convertmetochar.c_str());
				
        writess << numer << "/" << denom << " ;\n";
				
      }
			
      if (current_params[2*ii+1]==0) {
        writess << "0/1 ;\n";
      }
      else {
        ss << current_params[2*ii+1];
        ss >> convertmetochar;
        ss.clear();
        ss.str("");
				
        computeNumDenom(&numer, &denom, (char *) convertmetochar.c_str());
        writess << numer << "/" << denom << " ;\n";
      }
    } // end write parameter values
    
    // this is for all the values that are constant
    for (int ii = (2+2*numparam); ii< int(numdotout.size())-1; ++ii) {
      writess << numdotout[ii] << "\n";
    }
		
		
  }
	
	fout << writess.str();
	fout.close();
	
	
	
  
}





void slave_process::SetWorkingFolder(){
	
	
	
	
	boost::filesystem::path tmppath;
	
	if (paramotopy_settings.settings["files"]["customtmplocation"].intvalue==1){
		tmppath = paramotopy_settings.settings["files"]["tempfilelocation"].pathvalue;
	}
	else {
		tmppath = called_dir;
	}
	
	tmppath /= "bfiles_";
	tmppath += paramotopy_info.inputfilename;
	tmppath /= "tmpstep2";
	
	this->tmpfolder = tmppath;
	
	tmppath /= "work";
	std::stringstream converter;
	converter << myid;
	tmppath += converter.str();
	
	this->workingfolder = tmppath;

}



void slave_process::PurgeWorkingFolder(){
	boost::filesystem::remove_all(workingfolder);
}



void slave_process::MoveToWorkingFolder(){
	boost::filesystem::create_directories(this->workingfolder);
	safe_chdir(this->workingfolder.c_str());
	
	this->in_working_folder = true;
	
}



void slave_process::GoCalledDir(){
	safe_chdir(called_dir.c_str()); //used to move back to the starting directory to write the timing files.  now stay down there for loop, move here.   no sense in moving, when each worker is just writing its own files.

	this->in_working_folder = false;
}


void slave_process::ReadyCheck()
{
	
	bool its_all_good = true;
	std::vector< std::string > reported_errors;
	
	if (!have_dotout) {
		its_all_good = false;
		reported_errors.push_back("num.out");
	}
	
	if (!have_input) {
		its_all_good = false;
		reported_errors.push_back("no input");
	}
	
	if (!have_start) {
		its_all_good = false;
		reported_errors.push_back("no start");
	}
	
	
	if (!in_working_folder) {
		its_all_good = false;
		reported_errors.push_back("not in working folder");
	}
	
	if (filename.string().size()==0) {
		reported_errors.push_back("no filename");
		its_all_good = false;
	}
	
	if (tmpfolder.string().size()==0) {
		reported_errors.push_back("no tmpfolder");
		its_all_good = false;
	}

	if (numfilesatatime<=0) {
		reported_errors.push_back("unset numfilesatatime");
		its_all_good = false;
	}

	if (numparam<=0) {
		reported_errors.push_back("unset numparam");
		its_all_good = false;
	}
	
	if (standardstep2<0) {
		reported_errors.push_back("unset standardstep2");
		its_all_good = false;
	}
	

	if (!its_all_good){
		std::cerr << "slave " << myid << " reported its not all good, with bad flags:\n";
		
		for (int ii=0; ii<int(reported_errors.size()); ii++) {
			std::cerr << "slave " << myid << " flag" << ii << " " << reported_errors[ii] << std::endl;
		}
		std::cerr << "\n\nABORTING slave " << myid << std::endl;
		MPI_Abort(MPI_COMM_WORLD, 652);
	}
}

