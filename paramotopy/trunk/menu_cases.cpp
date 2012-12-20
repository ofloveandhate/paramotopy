 #include "menu_cases.hpp"



///////////////////////////////////
//
//
//      SERIAL CASE
//
//
/////////////////////////////////

void serial_case(ProgSettings paramotopy_settings, runinfo paramotopy_info_original){
	
	runinfo paramotopy_info;
	paramotopy_info.ParseData(paramotopy_info_original.location);
	paramotopy_info.location = paramotopy_info_original.location;
	paramotopy_info.steptwomode = paramotopy_info_original.steptwomode;
	
	std::vector< int > lastnumsent;
	lastnumsent.resize(1);
	int myid = 1;
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
	
	std::vector<std::string> Numoutvector;
	std::vector<std::string> arroutvector;
	std::vector<std::string> degoutvector;
	std::vector<std::string> namesoutvector;
	std::vector<std::string> configvector;
	std::vector<std::string> funcinputvector;
	
	int terminationint,smallestnumsent =0;
	
	int numfilestosave=0;
	settingmap::iterator iter;
	std::stringstream commandss;
	
	std::string lastoutfilename0 = paramotopy_info.location;
	lastoutfilename0.append("/step2/lastnumsent0");
	std::string lastoutfilename1 = paramotopy_info.location;
	lastoutfilename1.append("/step2/lastnumsent1");
	
	
	for (iter=paramotopy_settings.settings["SaveFiles"].begin() ; iter!=paramotopy_settings.settings["SaveFiles"].end()  ;++iter){
		if ((*iter).second.intvalue==1) {//supposed to retrieve the integer value for the file setting
			numfilestosave++;
			commandss << (*iter).first << " " << (*iter).second.filenumber << " ";		}
	}
	
	
	ToSave *TheFiles;
	TheFiles = new ToSave[numfilestosave];
	for (int i = 0; i < numfilestosave;++i){
		commandss >> TheFiles[i].filename;
		TheFiles[i].saved = true;
		commandss >> TheFiles[i].filecount;
	}
	//setup the vector of filesizes
	std::vector<int> filesizes;
	filesizes.resize(numfilestosave);
	for (int i=0; i<numfilestosave; ++i) {
		filesizes[i] = 0;
	}
	
	std::vector<std::string> runningfile; //for storing the data between writes.
	runningfile.resize(numfilestosave);
	for (int i=0; i<numfilestosave; ++i) {
		runningfile[i].reserve(2*paramotopy_settings.settings["MainSettings"]["buffersize"].intvalue);
	}
	
	
	std::string called_dir = stackoverflow_getcwd();
	std::string templocation;
	if (paramotopy_settings.settings["MainSettings"]["useramdisk"].intvalue==1) {
		templocation = paramotopy_settings.settings["MainSettings"]["tempfilelocation"].value();
	}
	else {
		templocation = called_dir;
	}
	
	SetUpFolders(paramotopy_info.location,
				 2,
				 paramotopy_settings.settings["MainSettings"]["numfilesatatime"].intvalue,
				 templocation);
	
	
	//make data file location
	std::stringstream myss;
	myss << called_dir << "/" << paramotopy_info.location << "/step2/DataCollected/c" << myid << "/";
	std::string DataCollectedbase_dir = myss.str();//specific to this worker, as based on myid
	myss.clear();
	myss.str("");
	
	std::vector<std::pair<double,double> >  AllParams;
	AllParams.resize(paramotopy_info.numparam);//preallocate

	
	std::string finishedfile = paramotopy_info.location;
	finishedfile.append("/step2finished");
	
	
	std::string initrunfolder;
	myss << templocation << "/" << paramotopy_info.base_dir <<  "/tmpstep2/init" << myid;
	myss >> initrunfolder;
	myss.clear();
	myss.str("");
	mkdirunix(initrunfolder.c_str());
	
	std::string workingfolder;
	myss << templocation << "/" << paramotopy_info.base_dir <<  "/tmpstep2/work" << myid;
	myss >> workingfolder;
	myss.clear();
	myss.str("");
	mkdirunix(workingfolder.c_str());
	
	
	
#ifdef timingstep2
	double t_start, t_initial, t_receive=0, t_send=0, t_write=0, t1,  t_read = 0, t_bertini = 0; //for timing the whatnot 
	int readcounter = 0, writecounter = 0, sendcounter = 0, receivecounter = 0, bertinicounter = 0;
	t_start = omp_get_wtime();
	std::ofstream timingout;
	std::string timingname = paramotopy_info.location;
	timingname.append("/timing/serialtiming");
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
		
		int num_lines_mcfile = GetMcNumLines(paramotopy_info.location,paramotopy_info.numparam); // verified correct for both newline terminated and not newline terminated.  dab
		
		
		
		terminationint = num_lines_mcfile;
		
		mc_in_stream.open(mcfname.c_str(), std::ios::in);
		
	}
	

	
	
	///////////////////
	//
	//       get the start and config files
	//
	///////////////////
	
	std::string start;
	
	
#ifdef verbosestep2
	std::cout << "making input file" << std::endl;
#endif
	
	//	// read in the start file
	GetStart(paramotopy_info.location,
			 start,
			 paramotopy_settings.settings["MainSettings"]["startfilename"].value());
	
	
	// for the step 2.1 solve, we need random values
	std::vector<std::pair<double, double> > tmprandomvalues = paramotopy_info.MakeRandomValues(42);
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
			//exit(-202);
			break;
	}
	
	
	std::vector< std::pair<double,double> > TmpValues;
	TmpValues.resize(paramotopy_info.numparam);
	
	std::vector< int > indexvector;//for forming a subscript from an index, for !userdefined case
	indexvector.resize(paramotopy_info.numparam);
	
	
	//input
	std::ofstream fout;
	std::string initruninput = initrunfolder;
	initruninput.append("/input");
	fout.open(initruninput.c_str());
	fout << inputstring;
	fout.close();
	
	
	//start
	std::string initrunstart = initrunfolder;
	initrunstart.append("/start");
	fout.open(initrunstart.c_str());
	fout << start;
	fout.close();
	
	
	
	int blaint = chdir(initrunfolder.c_str());
#ifdef timingstep2
	t1 = omp_get_wtime();
#endif
	int currentSeed = bertini_main(11,args_parse);
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
	
	std::string currentSeedstring;
	myss << currentSeed;
	myss >> currentSeedstring;
	args_noparse[11] = const_cast<char *>(currentSeedstring.c_str());
	myss.clear();
	myss.str("");
	
	
	long loopcounter = 0;  //is for counting how many solves this worker has performed, for data collection file incrementing
	blaint = chdir(workingfolder.c_str());  //formerly, used to move down on each iteration of loop.  now just stay in after initially moving into working folder.
	
	
	//NO double ParamSends[(numprocs-1)*numfilesatatime*(2*paramotopy_info.numparam+1)];//for sending to workers, to print into datacollected files
	int I, J, index;
	double a, b;//real and imaginary parts of the parameter values.
	std::stringstream ssdeleteme;
	
int lastoutcounter=0;//for writing the lastnumsent file.  every so many, we write the file.
int countingup = 0; //essentially the linenumber of the mc file, or index of the parameter point.
std::string target_file;
while (1) {
	
	smallestnumsent = countingup;
	//figure out how many files to do
	int numtodo = paramotopy_settings.settings["MainSettings"]["numfilesatatime"].intvalue;
	if (numtodo+countingup>=terminationint) {
		numtodo = terminationint - countingup;
	}
	
	
	/* Check the tag of the received message. */
	if (countingup == terminationint) {

		
		for (int j = 0; j < numfilestosave;++j){
			

			target_file = MakeTargetFilename(DataCollectedbase_dir,TheFiles,j);
			
			
#ifdef verbosestep2
			std::cout << "final writing data to " << target_file << " from worker " << myid << "\n";
#endif
			
#ifdef timingstep2
			t1= omp_get_wtime();
#endif
			WriteData(runningfile[j],
					  target_file,
					  paramotopy_info.ParameterNames);
			runningfile[j].clear();
#ifdef timingstep2
			t_write += omp_get_wtime() - t1;
			writecounter++;
#endif
		}
		break; //exits the while loop
	}
	
	
	
	// make tempsends in the step2 loop
	double data[paramotopy_settings.settings["MainSettings"]["numfilesatatime"].intvalue*(2*paramotopy_info.numparam+1)];
	memset(data, 0, paramotopy_settings.settings["MainSettings"]["numfilesatatime"].intvalue*(2*paramotopy_info.numparam+1)*sizeof(double) );
	
	int localcounter=0;
	for (int i = 0; i < numtodo; ++i){
		if (paramotopy_info.userdefined) {
			FormNextValues_mc(paramotopy_settings.settings["MainSettings"]["numfilesatatime"].intvalue,
							  paramotopy_info.numparam,
							  localcounter,
							  countingup,
							  mc_in_stream,data);
		}
		else {
			FormNextValues(paramotopy_settings.settings["MainSettings"]["numfilesatatime"].intvalue,
						   paramotopy_info.numparam,
						   localcounter,
						   paramotopy_info.Values,
						   countingup,
						   KVector,
						   data);
		}
		
#ifdef timingstep2
		t1 = omp_get_wtime();
#endif			//write the mc line if not userdefined
		if (!paramotopy_info.userdefined) {
			for (int j=0; j<paramotopy_info.numparam; ++j) {
				mc_out_stream << data[localcounter*(2*paramotopy_info.numparam+1)+2*j] << " "
					<< data[localcounter*(2*paramotopy_info.numparam+1)+2*j+1] << " ";
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
	
	//done getting next data points
	
	
	
	localcounter = 0;
	for (int k = 0; k < 1; ++k){
		
		
		// unpack some data for passing around; perhaps should rewrite stuff to make this unnecessary
		for (int mm=0; mm<paramotopy_info.numparam; ++mm) {
			AllParams[mm].first = data[localcounter*(2*paramotopy_info.numparam+1)+2*mm];
			AllParams[mm].second = data[localcounter*(2*paramotopy_info.numparam+1)+2*mm+1];
		}
		int linenumber = int(data[localcounter*(2*paramotopy_info.numparam+1)+2*paramotopy_info.numparam]);
		
		
		// Call Bertini
#ifdef timingstep2
		t1= omp_get_wtime();
#endif
		if (loopcounter==0){
			fout.open("start");
			fout << start;
			fout.close();
			WriteDotOut(arroutvector,
						degoutvector,
						namesoutvector,
						configvector,
						funcinputvector);
		}
		WriteNumDotOut(Numoutvector,
					   AllParams,
					   paramotopy_info.numparam);
#ifdef timingstep2
		t_write += omp_get_wtime() - t1;
		writecounter++; //increment the counter
#endif
		
		
		
		
#ifdef timingstep2
		t1 = omp_get_wtime();
#endif
#ifdef verbosestep2
		std::cout << linenumber << " ";
		for (int ii=0; ii < paramotopy_info.numparam; ++ii) {
			std::cout << AllParams[ii].first << " " << AllParams[ii].second << " ";
		}
		std::cout << std::endl;
#endif
		blaint = bertini_main(12,args_noparse);
#ifdef timingstep2
		t_bertini += omp_get_wtime() - t1;
		bertinicounter++;
#endif
		
		
		
		
		// Collect the Data
		for (int j = 0; j < numfilestosave;++j){
			
			
			//get data from file
#ifdef timingstep2
			t1= omp_get_wtime();
#endif
			
			runningfile[j].append(AppendData(linenumber,
											 TheFiles[j].filename,
											 paramotopy_info.ParameterNames,
											 AllParams));
			//std::cout << "read in from file " << TheFiles[j].filename << std::endl;
#ifdef timingstep2
			t_read += omp_get_wtime() - t1;
			readcounter++; //increment the counter
#endif
			
			
			//if big enough
			if (int(runningfile[j].size()) > 65536){
				std::string target_file = MakeTargetFilename(DataCollectedbase_dir,TheFiles,j);
				filesizes[j] += int(runningfile[j].size());
#ifdef verbosestep2
				std::cout << "writing data to " << target_file << " from worker " << myid << " with folder" << k << std::endl;
#endif
#ifdef timingstep2
				t1= omp_get_wtime();
#endif
				
				WriteData(runningfile[j],
						  target_file,
						  paramotopy_info.ParameterNames);
				
#ifdef timingstep2
				t_write += omp_get_wtime() - t1;
				writecounter++; //increment the counter
#endif
				runningfile[j].clear();//reset the string
				if (filesizes[j] >  paramotopy_settings.settings["MainSettings"]["newfilethreshold"].intvalue) { //update the file count
					TheFiles[j].filecount+=1;
					filesizes[j] = 0;
				}
			}
			
			
		}
		
		
		
		++localcounter;
		++loopcounter;
		
		
		
	}//re: for (int k = 0; k < int(status.MPI_TAG);++k)
	
	
	
	
	
	
	lastnumsent[0] = smallestnumsent;//
	
	if ((lastoutcounter%paramotopy_settings.settings["MainSettings"]["saveprogresseverysomany"].intvalue)==0) {
#ifdef timingstep2
		t1 = omp_get_wtime();
#endif
		std::ofstream lastout;
		if (lastoutcounter%(2*paramotopy_settings.settings["MainSettings"]["saveprogresseverysomany"].intvalue)==0) {
			lastout.open(lastoutfilename1.c_str());
		}
		else {
			lastout.open(lastoutfilename0.c_str());
		}
		
		std::stringstream tempss;
		for (int i=1; i<2; ++i) {
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
	
	
	lastoutcounter++;//essentially counts this loop.  
	}//re: while loop
	
	
	
	
	
	/////////////////////////////
	//
	//   done with solves, write timing data
	//
	////////////////////////////
	
	
#ifdef timingstep2
	t1 = omp_get_wtime();
#endif
	
	mc_out_stream.close();
	mc_in_stream.close();
	
	blaint = chdir(called_dir.c_str()); //used to move back to the starting directory to write the timing files.  now stay down 
	
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
		std::cout << stackoverflow_getcwd() << "\n";
		std::cerr << "serial timing out file " << timingname << "failed to open" << std::endl;
	}
	timingout <<  "initialize: " << t_initial << "\n"
		<< "bertini: " << t_bertini << " " << bertinicounter << "\n"
		<< "write: " << t_write+t_read << " " << writecounter+readcounter << "\n"
		<< "send: " << t_send << "\n"
		<< "receive: " << t_receive << "\n"
		<< "total: " << omp_get_wtime() - t_start << "\n";
	timingout.close();
#endif


	fout.open(finishedfile.c_str());
	fout << 1;
	fout.close();
	
	
	return;
}//re: serial_case()
	
	
	
	
	
	
	
	
	
	/////////////////////////////////
	//
	//
	//   PARALLEL STEP 2 CASE
	//
	//
	/////////////////////////////////

void parallel_case(ProgSettings paramotopy_settings, runinfo paramotopy_info){

	std::stringstream mpicommand;

	mpicommand << paramotopy_settings.settings["MainSettings"]["architecture"].value() << " -n ";

	mpicommand << paramotopy_settings.settings["MainSettings"]["numprocs"].value() << " ";
	mpicommand << paramotopy_settings.settings["MainSettings"]["step2location"].value() << "/step2 ";
	mpicommand << paramotopy_info.inputfilename << " ";
	mpicommand << paramotopy_info.location << " ";

	
	
	
	int numfilestosave=0;
	settingmap::iterator iter;
	std::stringstream commandss;

	for (iter=paramotopy_settings.settings["SaveFiles"].begin() ; iter!=paramotopy_settings.settings["SaveFiles"].end()  ;++iter){
		if ((*iter).second.intvalue==1) {//supposed to retrieve the integer value for the file setting
			numfilestosave++;
			commandss << (*iter).first << " " << (*iter).second.filenumber << " ";// << filename incrementedfilenumber ;  incrementedfilenumber set in touchfiletosave()
		}
	}

	mpicommand << numfilestosave << " ";
	mpicommand << commandss.str();

	mpicommand << paramotopy_settings.settings["MainSettings"]["numfilesatatime"].value() << " ";
	mpicommand << paramotopy_info.ParameterNames.size() << " ";

	for (int i = 0; i < int(paramotopy_info.ParameterNames.size());++i){
		mpicommand << paramotopy_info.ParameterNames[i] << " ";
	}

	mpicommand << paramotopy_settings.settings["MainSettings"]["saveprogresseverysomany"].value() << " ";
	mpicommand << paramotopy_settings.settings["MainSettings"]["newfilethreshold"].value() << " ";
	mpicommand << paramotopy_settings.settings["MainSettings"]["buffersize"].value() << " ";
	mpicommand << paramotopy_settings.settings["MainSettings"]["useramdisk"].value() << " ";
	if (paramotopy_settings.settings["MainSettings"]["useramdisk"].intvalue==1){
		mpicommand << " " << paramotopy_settings.settings["MainSettings"]["tempfilelocation"].value() << " ";
	}
	mpicommand << paramotopy_info.steptwomode << " ";
	if (paramotopy_settings.settings["MainSettings"]["stifle"].intvalue==1){
		mpicommand << " > /dev/null ";
	}



	std::cout << "\n\n\n\n\n\n\n\n" << mpicommand.str() << "\n\n\n\n\n\n\n\n\n";

	system(mpicommand.str().c_str());  //make the system call to bertini

}//re: parallel_case()
	
	










////////////////////////////////////////
//
//
//          MAIN STEP 2, CALLED FROM PARAMOTOPY
//
//
////////////////////////////////////////


void steptwo_case(ProgSettings paramotopy_settings,
				  runinfo paramotopy_info){
	
	

	
	
	//check if folder exists already.  if it does, prompt user
	boost::filesystem::path step2path(paramotopy_info.location);
	step2path /= ("step2");  //concatenation in boost
	
	if (boost::filesystem::exists(step2path)){  // if it can see the current run's step2 folder
		if (get_int_choice("found previous step2 folder.  remove, or bail out?\n0) bail out\n1) remove and continue\n: ",0,1)==1){
			boost::filesystem::remove_all( step2path );
			
			step2path += "finished";   // remove the step2finished file if it exists.
			if (boost::filesystem::exists(step2path)) {
				boost::filesystem::remove( step2path );
			}
		}
		else{
			std::cout << "returning to paramotopy main menu without running step2" << std::endl;
			return;
		}
	}
	
	
	

	
	paramotopy_settings.WriteConfigStepTwo();
	
	
	// open the bfiles_filename/mc file that contains the monte carlo points
	// in order in accordance to the order of the paramstrings in ParamStrings
	
	
	std::ifstream fin;
	std::ofstream fout;
	
	
	
	std::string curline;
	
	
	//make the DataCollected directory.
	std::string DataCollectedBaseDir=paramotopy_info.location;
	DataCollectedBaseDir.append("/step2/DataCollected/");
	mkdirunix(DataCollectedBaseDir.c_str());
	
	//touch files to save, in folders to save data.
	
	if (paramotopy_settings.settings["MainSettings"]["parallel"].intvalue == 0){
		std::stringstream CurDataCollectedBaseDir;
		CurDataCollectedBaseDir <<  DataCollectedBaseDir << "c" << 1 << "/";
		mkdirunix(CurDataCollectedBaseDir.str().c_str());
		SetFileCount(paramotopy_settings,DataCollectedBaseDir);
		//no need to touch any files.   
	}
	else{
		for (int i = 1; i < paramotopy_settings.settings["MainSettings"]["numprocs"].intvalue ;++i){
			std::stringstream CurDataCollectedBaseDir;
			CurDataCollectedBaseDir <<  DataCollectedBaseDir << "c" << i << "/";
			mkdirunix(CurDataCollectedBaseDir.str().c_str());
			SetFileCount(paramotopy_settings,CurDataCollectedBaseDir.str());
			//no need to touch.  boost and conditional opening took care of the yelling problem.

		}
	}
	
	//write a file containing the random values.
	std::string randpointfilename;
	randpointfilename = paramotopy_info.location;
	randpointfilename.append("/randstart");
	
	fout.open(randpointfilename.c_str());
	fout.precision(16);
	for (int i = 0; i < paramotopy_info.numparam; ++i){
		fout << paramotopy_info.RandomValues[i].first << " "
		<< paramotopy_info.RandomValues[i].second << "\n";
		
	}
	fout.close();
	

	
	
	//actually run that shit
	
	if (paramotopy_settings.settings["MainSettings"]["parallel"].intvalue == 0) {//this section needs a lot of work
		
		serial_case(paramotopy_settings, paramotopy_info);
		
	}// end not parallel
	
	else{//parallel case...
		parallel_case(paramotopy_settings, paramotopy_info);
		
	} // end parallel case
	
	
	
	
}//re: step2case()

