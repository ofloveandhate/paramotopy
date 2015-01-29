#include "para_aux_funcs.hpp"



void ParamotopySplashScreen(){
	
	std::cout
	  << "\n"
		<< "*******************************\n"
		<< " Welcome to Paramotopy.\n"
		<< "     parametrized system analysis software by\n"
		<< "     daniel brake and matthew niemerg, with dan bates.\n\n"
		<< "     www.paramotopy.com     danielthebrake@gmail.com  matthew.niemerg@gmail.com \n"
		<< "**************************" << std::endl;
	
	
	
	return;
}

void BertiniSplashScreen(std::string bertinilocation){

char *args_splash[2];
args_splash[0] = const_cast<char *>("bertini");
args_splash[1] = const_cast<char *>("--version");

	
 std::cout << "\n\nlinked library bertini:\n\n";
	printf("\n   Bertini(TM) v%s", BERTINI_VERSION_STRING);
	printf("\n   (%s)\n\n", BERTINI_DATE_STRING);
	printf(" D.J. Bates, J.D. Hauenstein,\n A.J. Sommese, C.W. Wampler\n\n");
	printf("(using GMP v%d.%d.%d, MPFR v%s)\n\n", __GNU_MP_VERSION, __GNU_MP_VERSION_MINOR, __GNU_MP_VERSION_PATCHLEVEL, mpfr_get_version());
	
	
 std::cout << "\n\nstand-alone bertini splash screen:\n\n";
 
 std::stringstream command;
 command << bertinilocation << "/bertini --version" ;
 system(command.str().c_str());
 std::cout << "\n\n";
 
 return;
 
}



int GetFileParserIndex(std::string filename){
	
	std::map < std::string , int > possible_savefiles;
  possible_savefiles["real_solutions"] = 1;
  possible_savefiles["nonsingular_solutions"] = 1;
  possible_savefiles["singular_solutions"] = 1;
  possible_savefiles["raw_data"] = -1;
  possible_savefiles["raw_solutions"] = -1;
  possible_savefiles["main_data"] = -1;
  possible_savefiles["midpath_data"] = -1;
  possible_savefiles["failed_paths"] = 2;
  possible_savefiles["real_finite_solutions"] = 1;

	
	return possible_savefiles[filename];
	
}





void GetFilesToParse(boost::filesystem::path run_to_analyze,
										 std::vector< std::string > & gather_savefiles,
										 std::vector< int > & gather_parser_indices)
{
  std::vector < std::string > possible_savefiles;
  possible_savefiles.push_back("real_solutions");
  possible_savefiles.push_back("nonsingular_solutions");
  possible_savefiles.push_back("singular_solutions");
  possible_savefiles.push_back("raw_data");
  possible_savefiles.push_back("raw_solutions");
  possible_savefiles.push_back("main_data");
  possible_savefiles.push_back("midpath_data");
  possible_savefiles.push_back("failed_paths");
  possible_savefiles.push_back("real_finite_solutions");
  std::vector < int > possible_parser_indices;
  possible_parser_indices.push_back(1);//real_solutions
  possible_parser_indices.push_back(1);//nonsingular
  possible_parser_indices.push_back(1);//singular_
  possible_parser_indices.push_back(-1);//raw_data
  possible_parser_indices.push_back(-1);//raw_solutions
  possible_parser_indices.push_back(-1);//main_data
  possible_parser_indices.push_back(-1);//midpath_data
  possible_parser_indices.push_back(2);//failed_paths
  possible_parser_indices.push_back(1); // real_finite_solutions
  
  for (int ii = 0; ii < int(possible_savefiles.size()); ii++){
    boost::filesystem::path temppath = run_to_analyze;
    temppath /= "step2/DataCollected/c1";  // scan the c1 folder, because every run has a c1 folder, and it will always contain data.
    std::string expression = "^";  //specify beginning of string
    expression.append(possible_savefiles[ii]);
    std::vector < boost::filesystem::path > filelist = FindFiles(temppath, expression);  //this function is in para_aux_funcs
    if (int(filelist.size())>0){
      gather_savefiles.push_back(possible_savefiles[ii]);
      gather_parser_indices.push_back(possible_parser_indices[ii]);
    }
  }
  
  return;
}


		

bool TestIfFinished(boost::filesystem::path & path_to_check){
	
	
  boost::filesystem::path appended_path = path_to_check;
  appended_path /= "step2finished";
  
  if (boost::filesystem::exists(appended_path)){
    return true;
  }
  else{
    return false;
  }
}


///the parser for the failed_paths file type, which is output from bertini.
int ParseFailedPaths(std::ifstream & fin, int numvariables){
  int tempfailnum = 0;
  std::string tmpstr;
  
  
  while (1) {
    
    getline(fin,tmpstr);
    //faily_myfailfail << tmpstr;
    if (tmpstr.length()==0) { // if do not have a solution
      break;
    }
    else { //have a soln
      for (int i=0; i<3+numvariables; ++i) {
	getline(fin,tmpstr);
	//faily_myfailfail << tmpstr;
      }
      tempfailnum++;
    }
  }
  return tempfailnum;
}




std::string replace_tilde_with_home(std::string workwithme){
	
  size_t found;
  
  found=workwithme.find('~');
  if ( (int(found)==0) ) {
    std::string replaced_string = getenv("HOME");
    replaced_string.append( workwithme.substr(found+1,workwithme.length()-1));
    return replaced_string;
  }
  else{
    return workwithme;
  }
}

//searches a directory for all files fitting a regular expression.
std::vector<boost::filesystem::path> FindDirectories(boost::filesystem::path dir, std::string expression){
  boost::filesystem::path p(dir);
  boost::regex e(expression);
  std::vector<boost::filesystem::path> found_folders;
  
  if (boost::filesystem::exists(p)) {
    boost::filesystem::directory_iterator dir_end;
    boost::filesystem::path filename;
    for(boost::filesystem::directory_iterator dir_iter(p); dir_iter != dir_end; ++dir_iter) {
      filename = dir_iter->path().filename();
      if (boost::regex_search(filename.string(),e)) {
				if (is_directory(dir_iter->path())){
					found_folders.push_back(dir_iter->path());
				}
      }
    }
  }
  else {
    std::cerr << "directory " << dir.string() << " does not exist!\n";
  }
  std::sort(found_folders.begin(), found_folders.end());
  return found_folders;
  
  
}

//searches a directory for all *directories* fitting a regular expression.
std::vector< boost::filesystem::path > FindFiles(boost::filesystem::path dir, std::string expression){
  
  boost::filesystem::path p(dir);
  boost::regex e(expression);
  std::vector<boost::filesystem::path> found_files;
  
  
  if (boost::filesystem::exists(p)) {
    boost::filesystem::directory_iterator dir_end;
    boost::filesystem::path filename;
    for(boost::filesystem::directory_iterator dir_iter(p); dir_iter != dir_end; ++dir_iter) {
      filename = dir_iter->path().filename();
      if (boost::regex_search(filename.string(),e)) {
				if (is_regular_file(dir_iter->path())){
					found_files.push_back(dir_iter->path());
				}
      }
    }
  }
  else {
    std::cerr << "directory " << dir << " does not exist!\n";
  }
  
  
  std::sort(found_files.begin(), found_files.end());
  return found_files;
  
}




boost::filesystem::path make_base_dir_name(boost::filesystem::path filename){  //intended to be called from step2
  
	boost::filesystem::path dir = "bfiles_";
	
  dir /= filename.filename();
  
  return dir;
}







bool parseDouble( std::string const& text, double& results )
{
    std::istringstream parser( text );
    return parser >> results >> std::ws && parser.peek() == EOF;
}

double getDouble()
{
	
	
  std::cin.ignore( std::numeric_limits<std::streampos>::max(), '\n' ) ;
  std::cin.clear() ; // to be safe, clear error flags which may be set
  
  double results;
  std::string line;
  while (1){
    while ( ! std::getline( std::cin, line )  )
      {
	
	std::cin.ignore( std::numeric_limits<std::streampos>::max(), '\n' ) ;
	std::cin.clear() ; // to be safe, clear error flags which may be set
	
      }
    
    if (! parseDouble( line, results )) {
      std::cout << "Only 'numeric' value(s) allowed:" << std::endl;
    }
    else{
      break;
    }
    
  }
  return results;
}



std::string getAlphaNumeric_WithSpaces(){
  std::string tmpstr;
  
  while (!std::getline(std::cin,tmpstr)) {
    std::cin.clear();
  }
  
  std::cin.clear();
  
  return tmpstr;
  
};


std::string getAlphaNumeric(){
	std::string tmpstr;
	
	while (!std::getline(std::cin,tmpstr)) {
		std::cin.clear();
	}
	
	std::cin.clear();
	
	std::istringstream parser( tmpstr);
	std::string returnme;
	parser >> returnme;
	return returnme;

};



bool parseInteger( std::string const& text, int& results )
{
    std::istringstream parser( text );
	parser >> results;
	return !(parser.fail());// >> std::ws && parser.peek() == EOF;
}

int getInteger()
{
	
  std::cin.ignore( std::numeric_limits<std::streampos>::max(), '\n' ) ;
  std::cin.clear() ; // to be safe, clear error flags which may be set
  
  int results;
  std::string line;
  while (1){
    while ( ! std::getline( std::cin, line )  )
      {
	
	std::cin.ignore( std::numeric_limits<std::streampos>::max(), '\n' ) ;
	std::cin.clear() ; // to be safe, clear error flags which may be set
	
      }
    
    if (! parseInteger( line, results )) {
      std::cout << "Only 'numeric' value(s) allowed:" << std::endl;
    }
    else{
      break;
    }
    
  }
  return results;
}


//  use this function to get input from the user and ensure it is an integer (actually fails to detect non-integer numeric inputs
int get_int_choice(std::string display_string,int min_value,int max_value){
	
	std::cout << display_string;
	
	int userinput = min_value - 1;

	std::string tmpstr;
	
	while (1)
	{
		userinput = getInteger();
		if (userinput <= max_value && userinput >= min_value){
			break;
		}
		else{
			std::cout << "value out of bounds." << std::endl;
		}
    }
	return userinput;
}

////  use this function to get inout fro the user and ensure it is an interger (actually fails to detect non-integer numeric inputs
//int get_int_choice(std::string display_string,int min_value,int max_value){
//
//	int userinput = min_value - 1;
//	
//	while (  (std::cout << display_string) &&
//		   ( !(std::cin >> userinput) || userinput < min_value || userinput > max_value))
//	{
//		std::cout << "Invalid entry -- try again:\n";
//		std::cin.clear();
//		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
//    }
//	return userinput;
//}




//the main choice function for paramotopy.
int ParamotopyMainMenu(){
	
  std::stringstream menu;
	
  menu << "\n\nYour choices : \n\n"
       << "1) Parse an appropriate input file. \n"
       << "2) Data Management. \n"
       << "3)   -unprogrammed- \n"
       << "4) Manage start point (load/save/new). \n"
       << "5) Write Step 1.\n"
       << "6) Run Step 1.\n"
       << "7) Run Step 2.\n"
       << "8) Failed Path Analysis.\n"
       << "\n"
       << "99) Preferences.\n"
       << "*\n"
       << "0) Quit the program.\n\n"
       << "Enter the integer value of your choice : ";
  
	std::cout << menu.str();
	
  int intChoice = -1;
	while (  (intChoice>=9 && intChoice<=98) || (intChoice >=100 || intChoice < 0) ) {
		intChoice = get_int_choice("", 0, 99);
	}
  
	
	
  return intChoice;
}  //   re: getuserchoice








//  this function, to get the current directory, is *directly* from stackoverflow
//
//
std::string stackoverflow_getcwd()
{
    const size_t chunkSize=255;
    const int maxChunks=10240; // 2550 KiBs of current path are more than enough
	
    char stackBuffer[chunkSize]; // Stack buffer for the "normal" case
    if(getcwd(stackBuffer,sizeof(stackBuffer))!=NULL)
        return stackBuffer;
    if(errno!=ERANGE)
    {
        // It's not ERANGE, so we don't know how to handle it
        throw std::runtime_error("Cannot determine the current path.");
        // Of course you may choose a different error reporting method
    }
    // Ok, the stack buffer isn't long enough; fallback to heap allocation
    for(int chunks=2; chunks<maxChunks ; chunks++)
    {
        // With boost use scoped_ptr; in C++0x, use unique_ptr
        // If you want to be less C++ but more efficient you may want to use realloc
        std::auto_ptr<char> cwd(new char[chunkSize*chunks]); 
        if(getcwd(cwd.get(),chunkSize*chunks)!=NULL)
            return cwd.get();
        if(errno!=ERANGE)
        {
            // It's not ERANGE, so we don't know how to handle it
            throw std::runtime_error("Cannot determine the current path.");
            // Of course you may choose a different error reporting method
        }   
    }
    throw std::runtime_error("Cannot determine the current path; the path is apparently unreasonably long");
}




void safe_chdir(std::string desired_directory){
	int success;
	
	
	success = chdir(desired_directory.c_str());
	
	if (success == -1) {
		
		
		std::string throwme = "failed to change directory to ";
		throwme.append(desired_directory);
		throwme.append(" giving error ");
		throwme.append(strerror (errno));
		throw std::runtime_error(throwme);
	}
//TODO: make this safe
}




std::string convert_spaces_to_escaped(std::string workwithme)
{
	size_t found;
  found=workwithme.find(' ');
	std::string newstring = "";
	
	
	while (found != std::string::npos) {
		newstring.append(workwithme.substr(0,found));
		newstring.append("\\ ");
		workwithme = workwithme.substr(found+1,workwithme.length()-1);
		found=workwithme.find(' ');
	}
  
	newstring.append(workwithme);

	
  return newstring;
	
}






int GetMcNumLines(boost::filesystem::path base_dir, int numparam){
	boost::filesystem::path mcfname = base_dir;
	mcfname /= "mc";
	
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





int GetStart(boost::filesystem::path dir,
			 std::string & start,
			 std::string startfilename){
	
	
	std::stringstream tempss;
	
	std::ifstream fin;
	//get start file in memory
	std::string copyme;
	
	boost::filesystem::path startstring=dir;
	startstring /= "step1";
	startstring /= startfilename;
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


