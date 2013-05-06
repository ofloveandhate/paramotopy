#include "step2readandwrite.hpp"






//opens the streams, sets terminationint, and initializes index_conversion_vector
void getTermination_OpenMC(std::ifstream & mc_in_stream,
													 std::ofstream & mc_out_stream,
													 int & terminationint,
													 std::vector< int > & index_conversion_vector,
													 runinfo & paramotopy_info,
													 ProgSettings & paramotopy_settings)
{
	std::string mcfname = paramotopy_info.location;
	mcfname.append("/mc");
	if (!paramotopy_info.userdefined) {
		index_conversion_vector.push_back(1);
		for (int i=1; i<paramotopy_info.numparam; ++i) {
			index_conversion_vector.push_back(index_conversion_vector[i-1]*paramotopy_info.NumMeshPoints[i-1]);
		}
		terminationint = index_conversion_vector[paramotopy_info.numparam-1]*paramotopy_info.NumMeshPoints[paramotopy_info.numparam-1];
		
		boost::filesystem::remove(mcfname);
		
		if (paramotopy_settings.settings["files"]["writemeshtomc"].intvalue==1){
			//open mc file for writing out to
			mc_out_stream.open(mcfname.c_str());

			//end open mc file
			if (!mc_out_stream.is_open()){
				std::cerr << "failed to open the parameter value out file: " << mcfname << std::endl;
				exit(11);
			
			}
      mc_out_stream << terminationint << std::endl;
		}
	}
	
	else {
		
		terminationint = GetMcNumLines(paramotopy_info.location,paramotopy_info.numparam); // verified correct for both newline terminated and not newline terminated.  dab
		
		
		std::cout << terminationint << " lines in mc file" << std::endl;
		
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


bool SlaveCollectAndWriteData(int & numfiles,
			      std::vector<std::string> & runningfile,
			      const int  linenumber,
			      ToSave * TheFiles,
			      const std::vector<std::string> ParamNames,
			      const std::vector<std::pair<double,double> > AllParams,
			      int buffersize,
			      const std::string DataCollectedbase_dir,
			      std::vector< int > & filesizes,
			      const int newfilethreshold,
			      const int myid,
			      timer & process_timer){
  

	// Collect the Data
	for (int j = 0; j < numfiles;++j){
		
	  
	  //get data from file
#ifdef timingstep2
	  process_timer.press_start("read");
#endif
	  
	  runningfile[j].append(AppendData(linenumber,
					   TheFiles[j].filename,
					   ParamNames,
					   AllParams));
	  
	  
#ifdef timingstep2
	  process_timer.add_time("read");
#endif
	  
	  
	  //if big enough
	  if (int(runningfile[j].size()) > buffersize){
	    std::string target_file = MakeTargetFilename(DataCollectedbase_dir,TheFiles,j);
	    filesizes[j] += int(runningfile[j].size());
#ifdef verbosestep2
	    std::cout << "writing data to " << target_file << " from worker " << myid << std::endl;
#endif
#ifdef timingstep2
	    process_timer.press_start("write");
#endif
			
	    WriteData(runningfile[j],
		      target_file,
		      ParamNames);
	    
#ifdef timingstep2
	    process_timer.add_time("write");
#endif
	    runningfile[j].clear();//reset the string
	    if (filesizes[j] > newfilethreshold) { //update the file count
	      TheFiles[j].filecount+=1;
	      filesizes[j] = 0;
	    }
	  }
	  
		
	}
	return true;
}




std::string AppendData(int point_index,
		       std::string orig_filename,
		       std::vector<std::string> ParamStrings,
		       std::vector<std::pair<double, double> > CValues){
  
  
  
  std::string cline = "";
  std::stringstream outstring;
  std::ifstream fin(orig_filename.c_str());
  
  if (!fin.is_open()) {
    std::cerr << "failed to open file '" << orig_filename << "' to read data" << std::endl;
    exit(-42);
  }
  
  
  outstring << point_index << "\n";
  for (int i = 0; i < int(CValues.size());++i){
    outstring << std::setprecision(16) << CValues[i].first << " " << CValues[i].second << " ";
  }
  outstring << "\n";
  while(getline(fin,cline)){
    outstring << cline << "\n";
  }
  fin.close();
  
  
  return outstring.str();
}


void WriteData(std::string outstring,
	       std::string target_file,
	       std::vector<std::string> ParamStrings){
	
	
  std::ofstream fout;
  // test if file target file is open
  
  struct stat filestatus;
  
  if (stat( target_file.c_str(), &filestatus ) ==0){  // if it can see the 'finished' file
    std::cout << "file " << target_file << "already found, opening for append" << std::endl;
    fout.open(target_file.c_str(),std::ios::app);
    
  }
  else{
    std::cout << "file " << target_file << "not found, opening fresh" << std::endl;
    fout.open(target_file.c_str());
    for (int i = 0; i < int(ParamStrings.size());++i){
      fout << ParamStrings[i] << (i != int(ParamStrings.size())-1? " ": "\n");
    }
  }
  
  if (!fout.is_open()) {
    std::cerr << "failed to open target dataout file '" << target_file << "'\n";
    exit(-41);
  }
  
  fout << outstring;
  fout.close();
}



int GetStart(std::string dir,
	     std::string & start,
	     std::string startfilename){
  

  std::stringstream tempss;
  
  std::ifstream fin;
  //get start file in memory
  std::string copyme;
  std::string startstring=dir;
  startstring.append("/step1/");
  startstring.append(startfilename);
  fin.open(startstring.c_str());
  
  if (!fin.is_open()) {
    std::cout << "failed to open specified solutions file: " << startstring <<"\n";
    exit(721);
  }
  
  getline(fin,copyme);
  std::stringstream converter;
  tempss << copyme << "\n";
  converter << copyme;
  int numsolutions;
  converter >> numsolutions;
  
  while (!fin.eof()) {
    getline(fin,copyme);
    tempss << copyme << "\n";
  }
  fin.close();
  //end get start in memory
  start = tempss.str();
  
  return numsolutions;
}






int GetLastNumSent(std::string base_dir,
		   std::vector< int > & lastnumsent,
		   int numprocs){
  
  std::ifstream fin;
  std::string blank;
  std::stringstream commandss;
  // query whether had run or not yet. 
  std::string lastoutfilename0 = base_dir;
  lastoutfilename0.append("/step2/lastnumsent0");	
  fin.open(lastoutfilename0.c_str());
  std::vector< int > lastnumsent0;
  lastnumsent0.push_back(0);
  int tmpint, smallest = 0;
  if (fin.is_open()){
    int lastoutcounter=1;
    for (int i=1; i<numprocs; ++i) {
      getline(fin,blank);
      if (blank=="") {
	break;
      }
      commandss << blank;
      commandss >> tmpint;
      lastnumsent0.push_back(tmpint);
      lastoutcounter++;
      commandss.clear();
      commandss.str("");
    }
    
  }
  fin.close();
  
  
  std::string lastoutfilename1 = base_dir;
  lastoutfilename1.append("/step2/lastnumsent1");	  
  fin.open(lastoutfilename1.c_str());
  std::vector< int > lastnumsent1;
  lastnumsent1.push_back(0);
  if (fin.is_open()){
    int lastoutcounter=1;
    for (int i=1; i<numprocs; ++i) {
      getline(fin,blank);
      if (blank=="") {
	break;
      }
      commandss << blank;
      commandss >> tmpint;
      lastnumsent1.push_back(tmpint);
      lastoutcounter++;
      commandss.clear();
      commandss.str("");
    }
    
  }
  fin.close();	
  
  
  int vectortosave = -1;
  if ( (int(lastnumsent1.size())<numprocs) && int(lastnumsent0.size())<numprocs ) {//failed overall...suck.
    return 0;
    //both vectors fail.  initialize fresh run.
  }
  else if (int(lastnumsent0.size())<numprocs){//failed during writing of 0?
    vectortosave = 1;
  }
  else if (int(lastnumsent1.size())<numprocs){//failed during writing of 1?
    vectortosave = 0;
  }
  else {//not possible for exactly one of the two following things to happen, given the previous 3 failed
    for (int i=1; i<numprocs; ++i) {
      if (lastnumsent0[i]<lastnumsent1[i]) {
	vectortosave = 1;
	break;
      }
      else if (lastnumsent1[i]<lastnumsent0[i]){
	vectortosave = 0;
	break;
      }
    }
  }
  
  //not possible for them to be the same.  vectortosave==(-1) if neither size matched.  fresh run if -1
  lastnumsent.push_back(0);
  if (vectortosave==0) {
    smallest = lastnumsent0[0];
    for (int i=1; i<numprocs; ++i) {
      lastnumsent.push_back(lastnumsent0[i]);
      if (lastnumsent0[i] < smallest) {
	smallest = lastnumsent0[i];
      }
    }	
    
  }
  else if (vectortosave==1){
    smallest = lastnumsent1[0];
    for (int i=1; i<numprocs; ++i) {
      lastnumsent.push_back(lastnumsent1[i]);
      if (lastnumsent1[i] < smallest) {
	smallest = lastnumsent1[i];
      }
    }
    
  }
  return smallest;
}





//read the random values from the step1.
void GetRandomValues(std::string base_dir,
		     std::vector< std::pair<double,double> > & RandomValues){
  
  std::stringstream myss;
  std::ifstream fin;
  std::string randfilename = base_dir;
  randfilename.append("/randstart");
  fin.open(randfilename.c_str());
  
  if (!fin.is_open()) {
    std::cout << "failed to open randfilename: " << randfilename << "\n";
    exit(731);
  }
  
  int ccount=0;
  std::string mytemp;
  std::cout << "\n\n";
  while(getline(fin,mytemp)){
    std::stringstream myss;
    myss << mytemp;
    double crandreal;
    double crandimaginary;
    myss >> crandreal;
    myss >> crandimaginary;
    RandomValues[ccount].first = crandreal;
    RandomValues[ccount].second = crandimaginary;
    std::cout << "random values:" << std::endl
	      << "Parameter" << ccount
	      << " = "
	      << RandomValues[ccount].first 
	      << " + "
	      << RandomValues[ccount].second
	      << "*I" << std::endl;
    ++ccount;
  }
  std::cout << std::endl;
  fin.close();
  //end get random values
  
}


void ReadDotOut(std::vector<std::string> & Numoutvector, 
		std::vector<std::string> & arroutvector,
		std::vector<std::string> & degoutvector,
		std::vector<std::string> & namesoutvector,
		std::vector<std::string> & configvector,
		std::vector<std::string> & funcinputvector,
		std::vector<std::string> & preproc_datavector){

  //read in Num.out file
  
  
  std::ifstream fin;
  std::string copyme;
  
  
  fin.open("num.out");
  
  while (!fin.eof()) {
    getline(fin,copyme);
    Numoutvector.push_back(copyme);
  }
  fin.close();
  //end read num.out
  
  
  //read in arr.out file
  fin.open("arr.out");
  
  while (!fin.eof()) {
    getline(fin,copyme);
    arroutvector.push_back(copyme);
  }
  fin.close();
  //end read arr.out
  
  
  //read in deg.out file
  fin.open("deg.out");
  
  while (!fin.eof()) {
    getline(fin,copyme);
    degoutvector.push_back(copyme);
  }
  fin.close();
  //end read deg.out
  
  //read in names.out file
  fin.open("names.out");
  
  while (!fin.eof()) {
    getline(fin,copyme);
    namesoutvector.push_back(copyme);
  }
  fin.close();
  //end read names.out
  
  //read in names.out file
  fin.open("config");
  
  while (!fin.eof()) {
    getline(fin,copyme);
    configvector.push_back(copyme);
  }
  fin.close();
  //end read names.out	
  
  
  //read in names.out file
  fin.open("func_input");
  
  while (!fin.eof()) {
    getline(fin,copyme);
    funcinputvector.push_back(copyme);
  }
  fin.close();
  //end read names.out	

  fin.open("preproc_data");

  while (!fin.eof()) {
    getline(fin,copyme);
    preproc_datavector.push_back(copyme);
  }
  fin.close();
  // end read preproc_data
}



void WriteDotOut(std::vector<std::string> & arroutvector,
		 std::vector<std::string> & degoutvector,
		 std::vector<std::string> & namesoutvector,
		 std::vector<std::string> & configvector,
		 std::vector<std::string> & funcinputvector,
		 std::vector<std::string> & preproc_datavector){
  
  std::ofstream fout;
  
  
  fout.open("arr.out");
  for (int ii = 0; ii< int(arroutvector.size())-1; ii++) {
    fout << arroutvector[ii] << "\n";
  }
  fout.close();
  
  fout.open("deg.out");
  for (int ii = 0; ii< int(degoutvector.size())-1; ii++) {
    fout << degoutvector[ii] << "\n";
  }
  fout.close();
  fout.open("names.out");
  for (int ii = 0; ii< int(namesoutvector.size())-1; ii++) {
    fout << namesoutvector[ii] << "\n";
  }
  fout.close();
  
  fout.open("config");
  for (int ii = 0; ii< int(configvector.size())-1; ii++) {
    fout << configvector[ii] << "\n";
  }
  fout.close();
  
  fout.open("func_input");
  for (int ii = 0; ii< int(funcinputvector.size())-1; ii++) {
    fout << funcinputvector[ii] << "\n";
  }
  fout.close();

  fout.open("preproc_data");

  for (int ii = 0; ii < int(preproc_datavector.size())-1; ++ii){
    fout << preproc_datavector[ii] << "\n";
  }
  fout.close();


}




void WriteNumDotOut(std::vector<std::string> Numoutvector,
		    std::vector<std::pair<double,double> > AllParams,
		    int numparam, bool standardstep2){
  
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
  
	
	
  //write the first two lines
  writess << Numoutvector[0] << "\n" << Numoutvector[1] << "\n";

  if (standardstep2){




    //write the random values associated with this run:
    for (int ii=0; ii<2*numparam; ++ii) {
      writess << Numoutvector[ii+2] << "\n";
      
    }
  
    //write the parameter lines
    for (int ii = 0; ii<numparam; ++ii) {
      if (AllParams[ii].first==0) {
	writess << "0/1 ;\n";
      }
      else {
	ss << AllParams[ii].first;
	ss >> convertmetochar;
	ss.clear();
	ss.str("");
	
	//make a call from the bertini library 
	computeNumDenom(&numer, &denom, (char *) convertmetochar.c_str());
	
	writess << numer << "/" << denom << " ;\n";
      }
      
      
      if (AllParams[ii].second==0) {
	writess << "0/1 ;\n";
      }
      else {
	ss << AllParams[ii].second; 
	ss >> convertmetochar;
	ss.clear();
	ss.str("");
	
	computeNumDenom(&numer, &denom, (char *) convertmetochar.c_str());
	writess << numer << "/" << denom << " ;\n";
      }
      
    }
    
    for (int ii = (2+4*numparam); ii< int(Numoutvector.size())-1; ++ii) {
      writess << Numoutvector[ii] << "\n";
    }
  } // end standard step2
  else{  // begin nonstandard step2 ... doing a standard total degree
         // bertini run with just parameters changed
    
    // Write the parameter values to be used in the SLP
    for (int ii=0; ii < numparam; ++ii){
      if (AllParams[ii].first == 0){
	writess << "0/1 ; \n";
      }
      else{
        ss << AllParams[ii].first;
        ss >> convertmetochar;
        ss.clear();
        ss.str("");

        //make a call from the bertini library                                                
        computeNumDenom(&numer, &denom, (char *) convertmetochar.c_str());

        writess << numer << "/" << denom << " ;\n";

      }

      if (AllParams[ii].second==0) {
        writess << "0/1 ;\n";
      }
      else {
        ss << AllParams[ii].second;
        ss >> convertmetochar;
        ss.clear();
        ss.str("");

        computeNumDenom(&numer, &denom, (char *) convertmetochar.c_str());
        writess << numer << "/" << denom << " ;\n";
      }
    } // end write parameter values
    
    // this is for all the values that are constant
    for (int ii = (2+2*numparam); ii< int(Numoutvector.size())-1; ++ii) {
      writess << Numoutvector[ii] << "\n";
    }


  }
    
    fout << writess.str();
    fout.close();
    
    

  
}



int GetMcNumLines(std::string base_dir, int numparam){
	std::string mcfname = base_dir;
	mcfname.append("/mc");
  
  std::ifstream fin(mcfname.c_str());
  
  if (!fin.is_open()){
    std::cerr << "failed to open mc file to get line count" << std::endl;
    exit(2132);
  }
  
  std::string tmpstr;
  std::getline(fin,tmpstr);
  std::stringstream converter;
  int terminationint;
  converter << tmpstr;
  converter >> terminationint;
  fin.close();
  
  
  return terminationint;
	
}



