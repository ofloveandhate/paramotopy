#include "para_aux_funcs.hpp"


void BertiniSplashScreen(std::string bertinilocation){

char *args_splash[2];
args_splash[0] = const_cast<char *>("bertini");
args_splash[1] = const_cast<char *>("--version");

	
 std::cout << "\n\nlinked library bertini:\n\n";
 bertini_main(2,args_splash);
 std::cout << "\n\nstand-alone bertini:\n\n";
 
 
 std::stringstream command;
 command << bertinilocation << "/bertini --version" ;
 system(command.str().c_str());
 std::cout << "\n\n";
 
 return;
 
}



void GetFilesToParse(boost::filesystem::path run_to_analyze, std::vector< std::string > & gather_savefiles, std::vector< int > & gather_parser_indices){
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
    std::vector < std::string > filelist = FindFiles(temppath.string(), expression);  //this function is in para_aux_funcs
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
std::vector<boost::filesystem::path> FindDirectories(std::string dir, std::string expression){
  boost::filesystem::path p(dir);
  boost::regex e(expression);
  std::vector<boost::filesystem::path> found_folders;
  
  if (boost::filesystem::exists(p)) {
    boost::filesystem::directory_iterator dir_end;
    std::string filename;
    for(boost::filesystem::directory_iterator dir_iter(p); dir_iter != dir_end; ++dir_iter) {
      filename = dir_iter->path().filename().string();
      if (boost::regex_search(filename,e)) {
	if (is_directory(dir_iter->path())){
	  found_folders.push_back(dir_iter->path());
	}
      }
    }
  }
  else {
    std::cerr << "directory " << dir << " does not exist!\n";
  }
  std::sort(found_folders.begin(), found_folders.end());
  return found_folders;
  
  
}

//searches a directory for all *directories* fitting a regular expression.
std::vector<std::string> FindFiles(std::string dir, std::string expression){
  
  boost::filesystem::path p(dir);
  boost::regex e(expression);
  std::vector<std::string> found_files;
  
  
  if (boost::filesystem::exists(p)) {
    boost::filesystem::directory_iterator dir_end;
    std::string filename;
    for(boost::filesystem::directory_iterator dir_iter(p); dir_iter != dir_end; ++dir_iter) {
      filename = dir_iter->path().filename().string();
      if (boost::regex_search(filename,e)) {
	if (is_regular_file(dir_iter->path())){
	  found_files.push_back(dir_iter->path().string());
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




std::string make_base_dir_name(std::string filename){  //intended to be called from step2
  std::string dir = "";
  
  std::string remainder = filename;
  size_t found;
  found = remainder.find('/');
  while (found!=std::string::npos) {  //if found the delimiter '/'
    
    dir.append(remainder.substr(0,found+1));  //
    std::cout << dir << "\n";
    remainder = remainder.substr(found+1,remainder.length()-found);    // the remainder of the path.  will scan later.
    found = remainder.find('/');                             // get the next indicator of the '/' delimiter.
  }// re:while
  dir.append("bfiles_");
  dir.append(remainder);
  
  return dir;
}





//makes a system call to make directory, and all parent directories.  
void mkdirunix(std::string mydir){
	
  std::string tmp = "mkdir -p ";
  tmp.append(mydir);
  system(tmp.c_str());
  
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
	return parser >> results;// >> std::ws && parser.peek() == EOF;
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
       << "9) Preferences.\n"
       << "*\n"
       << "0) Quit the program.\n\n"
       << "Enter the integer value of your choice : ";
  
  int intChoice = get_int_choice(menu.str(), 0, 9);//= -1;
  
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



	
