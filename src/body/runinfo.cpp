//contains the runinfo class

#include "runinfo.hpp"


bool runinfo::CheckRunStepOne(){
  // set the step1path to the string location
  // set various file names to the appropriate file names
  boost::filesystem::path step1path(this->location);
  step1path /= ("step1");  //concatenation in boost
  boost::filesystem::path nonsingular_solutions = step1path;
  nonsingular_solutions /= ("nonsingular_solutions");
  // test if we are finished with a run
  if (boost::filesystem::exists(nonsingular_solutions)){  // if it can see the 'finished' file
    // Redo the step1 or not. User choice.
    if (get_int_choice("found completed previous step1 run.\nremove, or bail out?\n0) bail out\n1) remove and continue\n: ",0,1)==1){
      boost::filesystem::remove_all( step1path );
      runinfo::mkdirstep1();
    }
    else{
      // Tell the user we are returning to the main menu without doing a step 1 run.
      std::cout << "returning to paramotopy main menu without running step one"
		<< std::endl;
      return false;
    }
    
  }  // if there is no nonsingular file, ii.e. run is not done
  else{
    typedef std::vector< boost::filesystem::path > vec;
    vec v;
    // not sure what this is doing ... boost stuff -- men
    copy(boost::filesystem::directory_iterator(step1path), boost::filesystem::directory_iterator(), back_inserter(v));
    for (vec::const_iterator it (v.begin()); it != v.end(); ++it)
      {
	boost::filesystem::remove( *it );
      }
  }
	
  return true;
}


//reads in the entire paramotopy input file to memory, and puts it in the string called 'paramotopy_file'
void runinfo::GetOriginalParamotopy(){
  // set the filename to null
	this->paramotopy_file = "";
	std::string tmpstr;
	std::ifstream fin;
	fin.open(this->inputfilename.c_str());
	// test if the user given input file exists
	if (!fin.is_open()) {
		std::cerr << "failed to open paramotopy_input_file " << this->inputfilename  << "for reading" <<  std::endl;
	}
	// if the file exists, 
	// then put the input file into memory into paramotopy_file
	while (getline(fin,tmpstr)) {
		this->paramotopy_file.append(tmpstr);
		this->paramotopy_file.append("\n");
	}
	fin.close();
	
	return;
}



//writes the original paramotopy input file, in its entirety, to the current run folder.
//
//assumes that you are in the original directory
void runinfo::WriteOriginalParamotopy(boost::filesystem::path dir){
	
	boost::filesystem::path filetoopen = dir;
	filetoopen /= "paramotopy_input_file";
	
	std::ofstream fout;
	fout.open(filetoopen.c_str());
	if (!fout.is_open()) {
		std::cerr << "writing paramotopy file " << filetoopen.string() << " failed." << std::endl;
	}
	fout << this->paramotopy_file;
	fout.close();
	
	
	filetoopen = dir;
	filetoopen /= "inputfilename";
	fout.open(filetoopen.c_str());
	if (!fout.is_open()) {
		std::cerr << "writing inputfilename file " << filetoopen.string() << " failed." << std::endl;
	}
	fout << this->inputfilename.string();
	fout.close();
	return;
}


//this function writes a modified paramotopy file, derived from the original, which is used in step2 failure analysis, to make the input file use a program-generated user-defined file of parameter points, which consist of the failed points.
void runinfo::WriteModifiedParamotopy(boost::filesystem::path dir, int iteration){
	
	boost::filesystem::path filetoopen = dir;
	std::stringstream paramotopy_file_str;
	
	paramotopy_file_str << this->numfunct << " " << this->numvargroup << " " << this->numparam << " " << this->numconsts << "\n";
	for (int ii=0; ii < this->numfunct; ++ii) {
		paramotopy_file_str << this->Functions[ii] << "\n";
	}
	for (int ii=0; ii < this->numvargroup; ++ii) {
		paramotopy_file_str << this->VarGroups[ii] << "\n";
	}
	if (this->numconsts > 0) {
		paramotopy_file_str << this->Constants[0] << "\n";
		
		for (int ii=0; ii < this->numconsts; ++ii) {
			paramotopy_file_str << this->ConstantNames[ii] << "\n";
		}
	}
	
	paramotopy_file_str << "1\n";
	paramotopy_file_str << "failed_points" << iteration << "\n";
	
	
	for (int ii = 0; ii < this->numparam; ++ii) {
		paramotopy_file_str << this->ParameterNames[ii] << "\n";
	}
	
	paramotopy_file_str << this->CustomLines << "\n"; //added 121127 dab  puts in the last lines of the paramotopy file.
	
	
	std::ofstream fout;
	filetoopen /= "paramotopy_input_file";
	fout.open(filetoopen.c_str());
	fout << paramotopy_file_str.str();
	fout.close();
	
	return;
}



void runinfo::make_base_dir_name(){
	//todo:  rewrite this using boost
	
	
	this->base_dir = "bfiles_";
	base_dir += this->inputfilename.filename();
	
	fundamental_dir = base_dir;
	
	
	boost::filesystem::path prefix_path(inputfilename);
	
	prefix = prefix_path.root_name();
	//std::cout << "prefix" << prefix << "\n";
	return;
}








void runinfo::SetLocation(){
	
	location = "unsetlocation";
	std::cout << "todo: setlocation whatnot\n";
}






std::string runinfo::WriteInputStepOne(ProgSettings paramotopy_settings){
  // Write the input section of the bertini step1 input file.
  std::stringstream inputfilestream;
  inputfilestream.precision(15);
	
	
  inputfilestream << "\nINPUT\n";
  runinfo::MakeVariableGroups(inputfilestream, paramotopy_settings);
  runinfo::MakeDeclareConstants(inputfilestream);
  runinfo::MakeDeclareFunctions(inputfilestream);
  runinfo::MakeConstants(inputfilestream);
  runinfo::MakeCustomLines(inputfilestream);
  runinfo::MakeFunctions(inputfilestream);
  inputfilestream << "\nEND;\n";
  
  return inputfilestream.str();
}


std::string runinfo::WriteInputStepTwo(std::vector<std::pair<double, double> > tmprandomvalues,
				       ProgSettings paramotopy_settings, bool failstep2){
  
  // Write the input portion of the step 2 bertini input file.
  std::stringstream inputfilestream;
	inputfilestream.precision(15);
	
  inputfilestream << "\nINPUT\n\n";
  // Variable Group portion


  std::string relevant_category;
  if (failstep2)
    relevant_category = "PathFailureBertiniCurrent";
  else
    relevant_category = "step2bertini";



  if (paramotopy_settings.settings["mode"]["standardstep2"].intvalue == 0 
                || 
      paramotopy_settings.settings[relevant_category]["USERHOMOTOPY"].value() == "2")
  {
    runinfo::MakeVariableGroups(inputfilestream, paramotopy_settings);
    inputfilestream << "\n\n";
  } 
  else
  {
    inputfilestream << "variable ";
    for (int ii = 0; ii < int(VarGroups.size());++ii){
      inputfilestream << VarGroups[ii] << ( ii != numvargroup-1? ",":";\n\n" );
    }

  }

  
  
  // constant portion for here and rand in the parameter homotopy
  if (paramotopy_settings.settings["mode"]["standardstep2"].intvalue == 1){ // normal parameter run


    inputfilestream << "constant ";
    for (int ii = 0; ii < numparam;++ii){
      inputfilestream << "rand" << ParameterNames[ii]
		      << (ii!= numparam-1?",":";\n");
    }
    inputfilestream << "constant ";
    for (int ii = 0; ii < numparam;++ii){
      inputfilestream << "here" << ParameterNames[ii]
		      << (ii!= numparam -1?",":";\n");
    }
    
    
    // user given constants
    if (Constants.size()!=0){
      inputfilestream << Constants[0] << "\n";
    }
    
    // Define Path Variable and the parameter homotopies
    
    inputfilestream << "pathvariable t;\n"
		    << "parameter ";
    for (int ii =0;ii < numparam;++ii){
      inputfilestream << ParameterNames[ii] << (ii!= numparam-1?",":";\n");
    }
    
    // Declare Functions
	  runinfo::MakeDeclareFunctions(inputfilestream);  // writes the line "function f1, f2, f3,....."
	  runinfo::MakeConstantsStep2(tmprandomvalues,inputfilestream, paramotopy_settings.settings["mode"]["standardstep2"].intvalue); // actually write the values for here and rand
  
	  
	  
	  
    //print the user-supplied custom stuffs.
    runinfo::MakeCustomLines(inputfilestream);
    
    //print the actual function definitions
    runinfo::MakeFunctions(inputfilestream);
    inputfilestream << "END;\n";
  }
  else{ // just leave all the parameters as constants, and update the num.out file accordingly
    
    inputfilestream << "constant ";
    for (int ii = 0; ii < numparam;++ii){
      inputfilestream << ParameterNames[ii]
                      << (ii!= numparam-1?",":";\n");
    }

    // user given constants                                                                            
    if (Constants.size()!=0){
      inputfilestream << Constants[0] << "\n";
    }


    // Declare Functions                                                                               
    runinfo::MakeDeclareFunctions(inputfilestream);


    runinfo::MakeConstantsStep2(tmprandomvalues,inputfilestream, paramotopy_settings.settings["mode"]["standardstep2"].intvalue);

    //print the user-supplied custom stuffs.                                                           
    runinfo::MakeCustomLines(inputfilestream);

    //print the actual function definitions                    


    runinfo::MakeFunctions(inputfilestream);
    inputfilestream << "END;\n";



  }
  return inputfilestream.str();
}




//for getting file name
void runinfo::GetInputFileName(){
	
  bool finfound = false;
  std::string filename;
  
  while (!finfound) {
    
    std::cout << "Enter the input file's name.       (% cancels when applicable)\n: ";
    filename = getAlphaNumeric_WithSpaces();
    
		size_t found=filename.find('%');
    if (found!=std::string::npos) {
      if ( (int(found) == 0) && boost::filesystem::exists(this->inputfilename)) { // if already have a good one, and user wants to bail out.
				std::cout << "canceling load.\n" << std::endl;
				filename = this->inputfilename.string();
				break;
      }
		}
		
    if (boost::filesystem::exists(filename)){
      finfound = true;
    }
    else {
      std::cout << "could not open a file of that name... try again.\n";
    }
  }
  
  inputfilename = filename;
  
  runinfo::make_base_dir_name();
  
  return;
}

void runinfo::GetInputFileName(boost::filesystem::path suppliedfilename){

  if (boost::filesystem::exists(suppliedfilename)){  // if it can see the 'finished' file
    this->inputfilename = suppliedfilename;
  }
  else{
    std::cout << "Unable to find file of supplied name (" << suppliedfilename << ").\n";
    runinfo::GetInputFileName();
  }
  runinfo::make_base_dir_name();
  
  return;
}



void runinfo::WriteRandomValues(){
  std::ofstream fout;
  fout.precision(16);
  
  boost::filesystem::path randpointfilename = base_dir;
  randpointfilename /= "randompoints_step1";
  fout.open(randpointfilename.c_str());
  
  if (!fout.is_open()) {
    std::cerr << "failed to open " << randpointfilename << " to write random points\n";
  }
  // write the random points
  for (int ii = 0; ii < numparam; ++ii){
    fout << RandomValues[ii].first << " "
	 << RandomValues[ii].second << "\n";
  }
  
  fout.close();
}


void runinfo::RandomMenu(){
    std::stringstream ss;
    ss << "1) save values\n"
       << "2) load random values\n"
       << "3) make new values\n"
       << "4) show current values\n"
       << "*\n0) return to paramotopy\n: ";
    int choice = get_int_choice(ss.str(),0,4);
    switch (choice) {
    case 0:
      
      break;
    case 1:
      runinfo::SaveRandom();
      break;
    case 2:
      runinfo::LoadRandom();
      break;
    case 3:
      runinfo::SetRandom();
    case 4:
      runinfo::PrintRandom();
      break;

    default:
      break;
    }
  };


void runinfo::SaveRandom(){
  // menu option for saving the random points, asks the user for a file name
  // to save the random start points.
  std::string randpointfilename;
  std::cout << "Enter the filename you want to save the random start points to : ";
  
  randpointfilename = getAlphaNumeric_WithSpaces();
  
  std::ofstream fout;
  fout.precision(16);
  fout.open(randpointfilename.c_str());
  
  for (int ii = 0; ii < numparam; ++ii){
    fout << RandomValues[ii].first << " "
	 << RandomValues[ii].second << "\n";
  }
  fout.close();
  
  return;
}



void runinfo::LoadRandom(){
  std::string randfilename;
  int cancelthis = 0;
  
  std::ifstream fin;
  
  
  while ( !fin.is_open() ) {
    std::cout << "Enter the filename of the random points you"
	      << " want to load (% to cancel): ";
    randfilename = getAlphaNumeric_WithSpaces();
    
    size_t found=randfilename.find('%');
    if (found!=std::string::npos) {
      if ( int(found) == 0) {
				std::cout << "canceling load.\n" << std::endl;
				cancelthis = 1;
				break;
      }
      
    }
    
		
    fin.open(randfilename.c_str());
  }
  
  if (!cancelthis) {
    
    std::string mytemp;
    int ccount=0;
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
      std::cout << ParameterNames[ccount]
		<< " = "
		<< RandomValues[ccount].first
		<< " + "
		<< RandomValues[ccount].second
		<< "*I\n";
      ++ccount;
    }
    std::cout << "\n";
    fin.close();
  }
  
  
  runinfo::PrintRandom();//in random.cpp
  
  return;
}


bool runinfo::GetPrevRandom(){
  bool loadedrandom = false;
  RandomValues.clear();
  
  boost::filesystem::path randomfilename = base_dir;
  randomfilename /= "randompoints_step1";

  if (boost::filesystem::exists(randomfilename)) {
    
    
    std::ifstream fin;
    fin.open(randomfilename.c_str());
    
    if (!fin.is_open()) {
      std::cerr << "failed to open " << randomfilename << " to read previous random values" << std::endl;
      return loadedrandom;
    }
    else{
      
      std::string tmpstr;
      int ccount=0;
      while(getline(fin,tmpstr)){
	std::stringstream myss;
	myss << tmpstr;
	std::pair<double,double> crand;
	myss >> crand.first;
	myss >> crand.second;
	RandomValues.push_back(crand);
				
	++ccount;
      }
      fin.close();
      
      if (int(RandomValues.size()) == numparam){
	loadedrandom = true;
	std::cout << "loaded previous random values at " << randomfilename << std::endl;
      }
      else{
	std::cout << "tried to read the previous random values and failed.\n" 
		  <<  "there were " << RandomValues.size() << " read values, and " << numparam << " necessary values"
		  << std::endl;
      }
      
    }
  }
  
  return loadedrandom;
}

void runinfo::SetRandom(){
	
  MTRand drand(time(0));
  int randchoice;
  std::stringstream menu;
  
  menu << "1) Default range [0 to 1)\n"
       << "2) User-specified range\n"
       << "Enter in if you would like to use the standard \n"
	<< "default range for random floats or if you would \n"
       << "like to specify the range : ";
  
  randchoice = get_int_choice(menu.str(), 1, 2);
  if (randchoice == 1){
    MakeRandomValues();
  }
  if (randchoice == 2){
    
    int paramrandchoice = 1;
    while (paramrandchoice != 0){
      std::cout << "0 - Done specifying random values\n";
      for (int ii = 0; ii < numparam;++ii){
	std::cout << ii+1 << " - " << ParameterNames[ii]
		  << "\n";
      }
      paramrandchoice = get_int_choice("Enter the parameter you want to rerandomize : ",0,numparam);
      
      if (paramrandchoice !=0){
	std::cout << "Enter a low range followed "
		  << "by a high range for the"
		  << " real and imaginary parts "
		  << "of the chosen parameter."
		  << "\n";
	
	double creallow;
	double crealhigh;
	double cimaginarylow;
	double cimaginaryhigh;
	std::cout << "\n" << "Real Low : ";
	creallow = getDouble();
	std::cout << "Real High : ";
	crealhigh = getDouble();
	std::cout << "Imaginary Low : ";
	cimaginarylow = getDouble();
	std::cout << "Imaginary High : ";
	cimaginaryhigh = getDouble();
	double crandreal = drand();
	double crandimaginary = drand();
	crandreal*=(crealhigh-creallow);
	crandimaginary*=(cimaginaryhigh-cimaginarylow);
	crandreal+=creallow;
	crandimaginary+=cimaginarylow;
	RandomValues[paramrandchoice-1].first=crandreal;
	RandomValues[paramrandchoice-1].second=crandimaginary;
      }
    }
  }
  runinfo::PrintRandom();//in random.cpp
	
	
}




void runinfo::PrintRandom(){
	
  std::cout << "The random initial values for the parameters are : \n";
  for (int ii = 0; ii < int(ParameterNames.size());++ii){
    std::cout << ParameterNames[ii] << " = " << RandomValues[ii].first
	      << " + " << RandomValues[ii].second << "*I\n";
    
  }
  std::cout << std::endl;
  return;
}

void runinfo::CopyUserDefinedFile(){
  
  if (this->userdefined){
    
    
    boost::filesystem::path fileloc(this->base_dir);
    boost::filesystem::create_directories(fileloc);
    fileloc /= "mc";
    
    boost::filesystem::path mcfile(this->mcfname);
    if (!boost::filesystem::exists(mcfile)) {
      std::cerr << "mcfile specified by the input file does not exist..." 
				<< std::endl;
      std::cerr << "mcfile: " << mcfile << std::endl;
      return;
    }
    
    
    std::string thecommand = "cp ";
    thecommand.append(this->mcfname);
    thecommand.append(" ");
    thecommand.append(fileloc.string());
    std::cout << thecommand << std::endl;
    system(thecommand.c_str());  // replace this by non-system command;
    
		
    
    
  }
}

//this is the function to be called in step2.
void runinfo::ParseData(boost::filesystem::path dir){
  runinfo::make_base_dir_name();
  boost::filesystem::path filetoopen = dir;
  filetoopen /= "paramotopy_input_file";
  
  std::ifstream fin;
  fin.open(filetoopen.c_str());
  if (!fin.is_open()) {
    std::cerr << "failed to open paramotopy input file " << filetoopen << std::endl;
    std::exit(1);
  }
  // call the main function to parse the data
  runinfo::ParseDataGuts(fin);
  
  fin.close();
  // temporary output of the parsed data
  
  // make the random values 
  runinfo::MakeRandomValues();
  
  return;
	
}

//this is the function to parse an input file to memory.
void runinfo::ParseData(){
  runinfo::make_base_dir_name();
  runinfo::GetOriginalParamotopy();
  
  std::ifstream fin;
  fin.open(inputfilename.c_str());
  if (!fin.is_open()) {
    std::cerr << "failed to open paramotopy input file " << inputfilename << std::endl;
  }
  // call the main function to parse the data.
  runinfo::ParseDataGuts(fin);
  fin.close();
  
	
  // temporary output of the parsed data
  
  
  runinfo::DisplayAllValues();
}

void runinfo::ParseDataGuts(std::ifstream & fin){
  runinfo::ReadSizes(fin);
  runinfo::ReadFunctions(fin);
  runinfo::ReadVarGroups(fin);
  
  if (this->numconsts!=0){
    runinfo::ReadConstants(fin);
  }
  
  runinfo::ReadConstantStrings(fin);
  
  std::string tmp;
  std::stringstream ss;
  getline(fin,tmp);
  ss << tmp;
  ss >> this->userdefined;
  if (userdefined){
    std::string paramfilename;
    
    getline(fin, paramfilename);
    runinfo::ReadParameters(fin);
    runinfo::MakeParameterNames();
    
    // copy user defined file to bfiles_filename/mc
    
    runinfo::make_base_dir_name();
    std::stringstream blabla;
    blabla << prefix;
    blabla << "/" << paramfilename;
    mcfname = paramfilename;

  }
  else{
    runinfo::ReadParameters(fin);
    runinfo::MakeParameterNames();
    runinfo::MakeValues();
		runinfo::MakeBounds();
  }
  
  runinfo::ReadCustomLines(fin);
  
  GetNumVariables();
  
  return;
	
}

void runinfo::ReadCustomLines(std::ifstream & fin){
	
  this-> CustomLines.clear();
  CustomLines = "";
  
  std::string tmpstr;
  
  while (getline(fin, tmpstr)) {
    this-> CustomLines.append(tmpstr);
    this-> CustomLines.append("\n");
  }
  
  return;
}

//part of the suite of parse input functions, reads the top line of paramotopy input file.
void runinfo::ReadSizes(std::ifstream & fin){
  numfunct = 0;
  numvargroup = 0;
  numparam = 0;
  numconsts = 0;
  
  std::string tmp;
  getline(fin,tmp);
  std::stringstream ss;
  ss << tmp;
  ss >> numfunct;
  ss >> numvargroup;
  ss >> numparam;
  ss >> numconsts;
}



//part of the suite of parse input functions, reads the parameter declarations.
void runinfo::ReadParameters(std::ifstream & fin){
  Parameters.clear();
  for (int ii = 0; ii < numparam; ++ii){
    std::string tmp;
    getline(fin,tmp);
    Parameters.push_back(tmp);
  }
  return;
}



void runinfo::MakeRandomValues(){
  RandomValues.clear();
  MTRand drand(time(0));
  for (int ii = 0; ii < numparam;++ii){
    double creal = double(drand());
    double cimaginary = double(drand());
		RandomValues.push_back(std::pair<double,double>(creal,cimaginary));
  }
  
	
	return;
}


void runinfo::MakeRandomValues(std::vector< std::pair< std::pair< double, double >,
							   std::pair< double, double > > > RandomRanges){
  
  RandomValues.clear();
  // make the random values with the specified ranges.
  MTRand drand(time(0));
  for (int ii = 0; ii < int(RandomRanges.size()); ++ii){
    double crandreal = drand();
    double crandimaginary = drand();
    crandreal*=(RandomRanges[ii].first.second
		- RandomRanges[ii].first.first);
    crandreal+=RandomRanges[ii].first.first;
    crandimaginary*=(RandomRanges[ii].second.second
		     -RandomRanges[ii].second.first);
    crandimaginary+=RandomRanges[ii].second.first;
    RandomValues.push_back(std::pair<double,double>(crandreal,
						    crandimaginary));
    
  }
	
  return;
}


std::vector<std::pair<double, double> > runinfo::MakeRandomValues(int seed_value){
  std::vector<std::pair<double, double> > NewRandomValues;
  
  MTRand drand(seed_value);
  
  
  //	for (int ii = 1; ii < fabs(ceil(42*drand())); ++ii){
  //		drand();
  //	}
  
  for (int ii = 0; ii < numparam;++ii){
    NewRandomValues.push_back(std::pair<double,double>(  double(drand()),double(drand())  ));
  }
  
  
  return NewRandomValues;
  
}





//read the random values from the step1.
void runinfo::GetRandomValues(){
  
  std::stringstream myss;
  std::ifstream fin;
  boost::filesystem::path randfilename = this->location;
  randfilename /= "randompoints_step1";
  fin.open(randfilename.c_str());
  
  if (!fin.is_open()) {
    std::cout << "failed to open randfilename: " << randfilename << "\n";
    exit(731);
  }
  
  int ccount=0;
  std::string mytemp;
  while(getline(fin,mytemp)){
    std::stringstream myss;
    myss << mytemp;
    double crandreal;
    double crandimaginary;
    myss >> crandreal;
    myss >> crandimaginary;
    this->RandomValues[ccount].first = crandreal;
    this->RandomValues[ccount].second = crandimaginary;
    ++ccount;
  }
  std::cout << std::endl;
  fin.close();
  //end get random values
  
}



//part of the suite of parse input functions, reads the functions from the paramotopy input file
void runinfo::ReadFunctions(std::ifstream & fin){
  Functions.clear();
  for (int ii = 0; ii < numfunct; ++ii){
    std::string tmp;
    getline(fin,tmp);
    Functions.push_back(tmp);
  }
  return;
}



//part of the suite of parse input functions, reads the variable groups from paramotopy input file
void runinfo::ReadVarGroups(std::ifstream & fin){
  VarGroups.clear();
  for (int ii = 0; ii < numvargroup; ++ii){
    std::string tmp;
    getline(fin,tmp);
    VarGroups.push_back(tmp);
  }
  return;
}



//part of the suite of parse input functions, reads the constants
void runinfo::ReadConstants(std::ifstream & fin){
  Constants.clear();
  std::string tmp;
  getline(fin,tmp);
  Constants.push_back(tmp);
  return;
}


//part of the suite of parse input functions, reads the constants
void runinfo::ReadConstantStrings(std::ifstream & fin){
  ConstantNames.clear();
  for (int ii = 0; ii < numconsts;++ii){
    std::string tmp;
    getline(fin,tmp);
    ConstantNames.push_back(tmp);
  }
  return;
}


void runinfo::GetNumVariables(){
	
  int count = 0;
  size_t comma_found;
  
  for (int ii=0; ii<numvargroup; ++ii) {
    count++;
    comma_found = VarGroups[ii].find(",");
    while ( comma_found != std::string::npos ) {
      count++;
      comma_found = VarGroups[ii].find(",",comma_found+1,1);
    }
  }
  numvariables = count;
  return;
}



void runinfo::MakeFunctions(std::stringstream & inputfilestream){
  inputfilestream << "\n";
  for (int ii = 0; ii < numfunct;++ii){
    inputfilestream << "f" << ii+1
		    << " = "
		    << Functions[ii]
		    << ";\n";
    
  }
}



//writes variable groups to stringstream for bertini input file.
void runinfo::MakeVariableGroups(std::stringstream & fout, ProgSettings paramotopy_settings){
  
  if (paramotopy_settings.settings["step1bertini"]["USERHOMOTOPY"].value() == "1"){
    fout << "\n";
    fout << "variable ";
    for (int ii = 0; ii < numvargroup; ++ii){
      fout << VarGroups[ii];
      fout << (ii != numvargroup - 1 ? ", " : "");
    }
    fout << ";\n";
    
  }
  else{
    for (int ii = 0; ii < numvargroup;++ii){
      fout << "\n";
      fout << "variable_group ";
      fout << VarGroups[ii];
      fout << ";";    
    }
    
  }
}


//writes constant declarations to stringsteam, for writing to file
void runinfo::MakeDeclareConstants(std::stringstream & fout){
  fout << "\n";
  fout << "constant ";
  for (int ii = 0; ii < int(this->ParameterNames.size());++ii){
    fout << this->ParameterNames[ii]
	 << (ii != int(this->ParameterNames.size())-1?",":";\n");
    
  }
  if (this->Constants.size()!=0){
    fout << this->Constants[0]
	 << "\n";
  }
}


//writes the function declaration to mutable stringstream, for writing to file for bertini later.
void runinfo::MakeDeclareFunctions(std::stringstream & inputfilestream){
  inputfilestream << "function ";
  for (int ii = 1; ii <=numfunct;++ii){
    inputfilestream << "f" << ii;
    if (ii != numfunct){
      inputfilestream << ", ";
    }
    else{
      inputfilestream << ";\n";
    }
  }
  return;
}


void runinfo::MakeConstants(std::stringstream & fout){
  
  for (int ii = 0; ii < int(this->RandomValues.size());++ii){
    fout << this->ParameterNames[ii]
	 << " = "
	 << this->RandomValues[ii].first
	 << " + "
	 << this->RandomValues[ii].second
		<< "*I;\n";
  }
  
  for (int ii = 0; ii < int(ConstantNames.size());++ii){
    fout << "\n" << this->ConstantNames[ii];
  }
  fout << "\n";
}


void runinfo::MakeCustomLines(std::stringstream & inputfilestream){
  
  inputfilestream << "\n\n" << this->CustomLines;
  
  return;
}


void runinfo::MakeConstantsStep2(std::vector<std::pair<double, double> > CurrentValues, std::stringstream & inputfilestream, bool standardstep2){
	
	
  if (standardstep2){
    for (int ii = 0; ii < int(this->ParameterNames.size()); ++ii){
      inputfilestream << "rand"
		      << this->ParameterNames[ii]
		      << " = "
		      << this->RandomValues[ii].first << " + " << this->RandomValues[ii].second
		      << "*I;\n";
    }
    
    for (int ii = 0; ii < int(this->ParameterNames.size()); ++ii){
      inputfilestream << "here"
		      << this->ParameterNames[ii]
		      << " = "
		      << CurrentValues[ii].first << " + " << CurrentValues[ii].second
		      << "*I;\n";
    }
    
    
    
    for (int ii = 0; ii < int(this->ParameterNames.size()); ++ii){
      inputfilestream << this->ParameterNames[ii]
		      << "= t*rand"
		      << this->ParameterNames[ii]
		      << " + (1-t)*here"
		      << this->ParameterNames[ii]
		      << ";\n";
    }
    
	  
	  for (int ii = 0; ii < int(this->ConstantNames.size());++ii){
		  inputfilestream << this->ConstantNames[ii]
		  << "\n";
	  }
	  
	  
  }
  else{
    for (int ii = 0; ii < int(this->ParameterNames.size()); ++ii){
    inputfilestream << this->ParameterNames[ii]
		    << " = "
		    << this->RandomValues[ii].first << " + " << this->RandomValues[ii].second
		    << "*I;\n";
    }
    for (int ii = 0; ii < int(this->ConstantNames.size());++ii){
      inputfilestream << this->ConstantNames[ii]
                      << "\n";
    }


  }
  return;
}






void runinfo::MakeValues(std::ifstream & fin){
	
	this->NumMeshPoints.clear();
  Values.clear();
	
  std::string temp;
  int lcount = 0;
  while(getline(fin,temp)){
    std::vector< std::pair<double, double> > CValue;
    std::stringstream ss;
    ss << temp;
    for (int ii = 0; ii < numparam;++ii){
      double creal;
      double cimaginary;
      ss >> creal;
      ss >> cimaginary;
      CValue.push_back(std::pair<double, double>(creal,cimaginary));
    }
    ++lcount;
    
    Values.push_back(CValue);
  }
  std::cout << "line count = " << lcount << "\n\n";
  return;
}





void runinfo::MakeBounds(){
	
	if (this->userdefined!=0) {
		std::cout << "attempting to set bounds in a non-computer-generated" << std::endl;
		exit(-12);
	}
	
	this->BoundsLeft.clear();
	this->BoundsRight.clear();
	// parse the parameter string per line
  for (int ii = 0; ii < int(Parameters.size());++ii){
		std::string temp;
    std::stringstream ss;
    ss << Parameters[ii];
    ss >> temp; // pull the name off the string

    // values given by lep, rep, and # of mesh points
    double lepr; // left end point real
    double lepi; // right end point real
    double repr; //
    double repi;
		
    ss >> lepr;
    ss >> lepi;
    ss >> repr;
    ss >> repi;
		this->BoundsLeft.push_back(std::pair< double, double> (lepr, lepi));
		this->BoundsRight.push_back(std::pair< double, double> (repr, repi));
  }
  return;

	
	
}
//sets the Values and NumMeshPoints variables in the runinfo class
void runinfo::MakeValues(){
	
  this->NumMeshPoints.clear();
  this->Values.clear();


  // parse the parameter string per line
  for (int ii = 0; ii < int(Parameters.size());++ii){
    std::string temp;
    //    bool userdefined;
    
    std::stringstream ss;
    ss << Parameters[ii];
    ss >> temp;
    //   ss >> userdefined;
    //   if (!userdefined){
    // values given by lep, rep, and # of mesh points
    double lepr; // left end point real
    double lepi; // right end point real
    double repr; //
    double repi;
    int meshpoints;
    std::vector< std::pair<double,double> > currentmesh;
    ss >> lepr;
    ss >> lepi;
    ss >> repr;
    ss >> repi;
    ss >> meshpoints;
		
    this->NumMeshPoints.push_back(meshpoints); // add the number of meshpoints to the vector keeping track
    double stepr = (repr - lepr)/(meshpoints-1);
    double stepi = (repi - lepi)/(meshpoints+1);
    double currentr = lepr;
    double currenti = lepi;
    for (int jj = 0; jj < meshpoints;++jj){
      currentmesh.push_back(std::pair<double,double>(currentr,currenti)); // set the temp value
      currentr+=stepr;
      currenti+=stepi;
    }
    this->Values.push_back(currentmesh);// add to the Values vector.
    
    
    // fin.close();
    
  }
  return;
}


//given a vector of strings to parse, makes and returns a vector of parameter names
void runinfo::MakeParameterNames(){
	
  ParameterNames.clear();
  for (int ii = 0; ii < numparam;++ii){
    std::stringstream ss;
    ss << Parameters[ii];
    std::string paramname;
    ss >> paramname;
    ParameterNames.push_back(paramname);
  }
  
  return;
}



void runinfo::DisplayAllValues(){
	
  std::cout << "number of functions " << numfunct << std::endl;
  for (int ii=0; ii<numfunct; ++ii) {
    std::cout << Functions[ii] << std::endl;
  }
  
  std::cout << "number of variable groups " << numvargroup << std::endl;
  for (int ii=0; ii<numvargroup; ++ii) {
    std::cout << VarGroups[ii] << std::endl;
  }
  
  std::cout << "number of parameters " << numparam << std::endl;
  for (int ii=0; ii<numparam; ++ii) {
    std::cout << Parameters[ii] << std::endl;
  }
  for (int ii=0; ii<numparam; ++ii) {
    std::cout << "parameter name " << ParameterNames[ii] << std::endl;
  }
  
  
  std::cout << "number of constants " << numconsts << std::endl;
  for (int ii=0; ii<numconsts; ++ii) {
    std::cout  << ConstantNames[ii] << std::endl;
  }
  if (numconsts>0) {
    std::cout  << Constants[0] << std::endl;
  }
	
	
  
  std::cout << "base directory " << base_dir << ", "
	    << "input filename " << inputfilename << std::endl;
  
  std::cout << "number variables " << numvariables << std::endl;
  if (userdefined==1) {
    std::cout << "parameter sample userdefined, with filename: " << mcfname << std::endl;
  }
  else{
    std::cout << "using computer-generated parameter mesh." << std::endl;
  }
  
  if ( int(this->CustomLines.size()>0) ) {
    std::cout << "custom lines for input file: " << this->CustomLines << std::endl;
  }
  
  return;
}














//////////////////////////////////////////////////////////////////////////
//
//
//                 data management
//
//
////////////////////////////////////////////////////////////////////////












//tests if step2 finished.
//todo:  rewrite using boost
bool runinfo::test_if_finished(){
  bool finished = false;
  
  boost::filesystem::path finishedfile = this->base_dir;
  finishedfile /= "step2finished";

  
  if (boost::filesystem::exists(finishedfile) ){  // if it can see the 'finished' file
    finished = true;
  }
  return finished;
}






////////////////
//
//
//       functions for managing data folders.
//
///
///////////////////



void runinfo::UpdateAndSave(){
  updated = time(NULL);
  runinfo::save();
  return;
}


void runinfo::SetBaseDirZero(){
  
  base_dir = fundamental_dir;
  base_dir /= "run0";
  
  run_number = 0;
  initiated = time(0);
  updated = time(0);
  
  
  return;
}


void runinfo::SetBaseDirManual(std::vector< boost::filesystem::path > found_runs){
  time_t wheninitiated, whenupdated;  //for display
  
  
  std::cout << "the following are your options for data folders:\n\n";
  std::cout << "     name                date modified\n";
  for (int ii = 0; ii<int(found_runs.size()); ++ii) {
    std::string tmpdir = found_runs[ii].string();
    tmpdir.append("/info.xml");
    int tmprun = 0;
    get_run_xml(tmpdir,tmprun,wheninitiated,whenupdated);
    
    std::cout << ii << ": " << found_runs[ii].string();
    for (int jj=0; jj<(20 - int(found_runs[ii].string().length())); ++jj) {
      std::cout << " ";
		}
    std::cout << ctime(&whenupdated);
  }
  
  std::cout << found_runs.size() << ": " << "make new folder\n" << std::endl;
  
  int choice = get_int_choice("which folder to use?\n\n: ",0,found_runs.size());
  
  std::cout << choice << " " << found_runs.size() << "\n";
  
  if (choice<int(found_runs.size())) {
    
    base_dir = found_runs[choice].string();
    std::cout << "loading old directory " << base_dir << "\n";
    
    boost::filesystem::path fname = base_dir;
    fname /= "info.xml";
    runinfo::load(fname);//gets the run number, initiation date, and last time updated.
    made_new_folder = false;
  }
  else{
    std::cout << "making new directory\n";
    runinfo::SetBaseDirNew(found_runs);
    
  }
  
	
  
  return;
	
}



void runinfo::SetBaseDirNew(std::vector< boost::filesystem::path> found_runs){
  //get highest number, increment, set base dir and stuff
  
  int max_run_number = -101;
  
  time_t wheninitiated, whenupdated;
  int run;
  
  for (int ii=0; ii<int(found_runs.size()); ++ii) {
    std::string tmpdir = found_runs[ii].string();
    tmpdir.append("/info.xml");
    get_run_xml(tmpdir,run,wheninitiated,whenupdated);
    if (run > max_run_number){
      max_run_number = run;
    }
  }
  
  run_number = max_run_number+1;
  initiated = time(0);
  updated = time(0);
  
  base_dir = fundamental_dir;
  base_dir /= "run";
  std::stringstream ss;
  ss << run_number;
  base_dir += ss.str();
  
  boost::filesystem::create_directories(base_dir);
  runinfo::save();
  
  made_new_folder = true;
  return;
}



void runinfo::SetBaseDirMostRecent(std::vector< boost::filesystem::path> found_runs){
  //get highest number, increment, set base dir and stuff
  
  time_t most_recent = -1;
  boost::filesystem::path tmpdir;
  time_t wheninitiated, whenupdated;
  int tmprun;
  int runindex = 0;
  
  std::cout << "\n   name                date modified\n\n";
  
  for (int ii=0; ii<int(found_runs.size()); ++ii) {
    tmpdir = found_runs[ii];
    tmpdir /= "info.xml";
    get_run_xml(tmpdir,tmprun,wheninitiated,whenupdated);
    
    std::cout << found_runs[ii].string();
    for (int jj=0; jj<(20 - int(found_runs[ii].string().length())); ++jj) {
      std::cout << " ";
    }
    std::cout << ctime ( &whenupdated );
    if (whenupdated > most_recent){
      most_recent = whenupdated;
      runindex = ii;
    }
    
  }
  
  std::cout << std::endl;
  tmpdir  = found_runs[runindex];
  
  base_dir = tmpdir;
  
  tmpdir /= "info.xml";
  runinfo::load(tmpdir);
  
  return;
}


void runinfo::ScanData(){
  std::cout << "Output directory is now: " << base_dir << std::endl;
  
  std::vector< boost::filesystem::path > found_runs = FindDirectories(fundamental_dir, "^run");
  
  if (found_runs.size()==0) {
    std::cout << "found no previous data, starting new folder\n";
    runinfo::SetBaseDirZero();
    made_new_folder = true;
  }
  else {
    runinfo::SetBaseDirManual(found_runs);
  }
  
  runinfo::save();
  
  
  return;
}

void runinfo::AutoScanData(const int preferred_behaviour){
	
  std::vector< boost::filesystem::path > found_runs = FindDirectories(fundamental_dir, "^run");
  
  if (found_runs.size()==0) {
    std::cout << "found no previous data, starting new folder\n";
    runinfo::SetBaseDirZero();
  }
  else {
    switch (preferred_behaviour) {
    case 1:
      std::cout << "setting base_dir most_recent\n";
      runinfo::SetBaseDirMostRecent(found_runs);
      
      break;
      
    case 2:
      std::cout << "Setting base dir new\n";
      runinfo::SetBaseDirNew(found_runs);
      std::cout << base_dir << "\n";
      break;
      
    case 3:
      std::cout << "setting basedir manually\n";
      runinfo::SetBaseDirManual(found_runs);
      break;
      
    default:
      break;
    }
  }
  
  std::cout << "run folder set to " << base_dir << std::endl;
  runinfo::save();
  
  
  return;
}






//saves the run info to an xml file
void runinfo::save(){
  TiXmlDocument* doc = new TiXmlDocument;
  TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
  
  doc->LinkEndChild( decl );
  
  TiXmlElement * root = new TiXmlElement("run_info");
  //categorymap::iterator iter;
  
  TiXmlText* txt;
  std::stringstream ss;
  
  
  TiXmlElement * value_name1 = new TiXmlElement( "run_number" );
  ss << run_number;
  txt = new TiXmlText( ss.str().c_str() );
  ss.clear();
  ss.str("");
  value_name1->LinkEndChild(txt);
  
  TiXmlElement * value_name2 = new TiXmlElement( "initiated" );
  ss << initiated;
  txt = new TiXmlText( ss.str().c_str() );
  ss.clear();
  ss.str("");
  value_name2->LinkEndChild(txt);
  
  TiXmlElement * value_name3 = new TiXmlElement( "updated" );
  time_t currtime = time(NULL);
  ss << currtime;
  txt = new TiXmlText( ss.str().c_str() );
  ss.clear();
  ss.str("");
  value_name3->LinkEndChild(txt);
  
  root->LinkEndChild(value_name1);
  root->LinkEndChild(value_name2);
  root->LinkEndChild(value_name3);
  
  
  doc->LinkEndChild( root );
	
  boost::filesystem::create_directories(base_dir);
  
  boost::filesystem::path filename = base_dir;
  filename /= "info.xml";
  //save it
  if(doc->SaveFile(filename.c_str())){
    
  }
  else{
    //couldn't save for some reason.  this may be a problem on queued systems?
    std::cout << "run info *failed* to save to " << 	filename.string() << "!\n";
  };
  
  
  return;
}


void runinfo::load(boost::filesystem::path filename){
  int tmp_run=0;
  time_t tmp_initiated=-2, tmp_updated=-1;
  
  get_run_xml(filename,tmp_run,tmp_initiated,tmp_updated);
  run_number = tmp_run;
  initiated = tmp_initiated;
  updated = time(NULL);
	
  return;
}


//returns via reference the run number, time initiated, and time updated, of a run.
void runinfo::get_run_xml(boost::filesystem::path filename, int & run, time_t  & wheninitiated, time_t & whenupdated){
	
  TiXmlDocument* doc = new TiXmlDocument(filename.string().c_str());
  
  bool doc_loaded = doc->LoadFile();
  if (!doc_loaded){
    
  }
  else{
    TiXmlHandle hDoc(doc);
    TiXmlElement* pElem;
    TiXmlHandle hRoot(0);
    
    pElem=hDoc.FirstChildElement().Element();
    // should always have a valid root but handle gracefully if it doesn't
    
    if (!pElem) {
      std::cerr << "bad xml file for this run.\n";
    }
    else{
      
      std::stringstream ss;
      
      std::string main_name =pElem->Value();  //unused?
      hRoot=TiXmlHandle(pElem);  // the handle for the data we will be reading
      
      TiXmlElement* qElem = hRoot.FirstChild("run_number").Element();  //read the name
      const char *numberText=qElem->GetText();
      ss << numberText;
      ss >> run;
      ss.clear();
      ss.str("");
      
      qElem = hRoot.FirstChild("initiated").Element();  //read the name
      const char *initiatedText=qElem->GetText();
      ss << initiatedText;
      ss >> wheninitiated;
      ss.clear();
      ss.str("");
      
      
      qElem = hRoot.FirstChild("updated").Element();  //read the name
      const char *updatedText=qElem->GetText();
      ss << updatedText;
      ss >> whenupdated;
      ss.clear();
      ss.str("");
      
      
    }
  }
  
  return;
}
