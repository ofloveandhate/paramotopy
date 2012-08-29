#include "para_aux_funcs.hpp"

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







//  use this function to get inout fro the user and ensure it is an interger (actually fails to detect non-integer numeric inputs
int get_int_choice(std::string display_string,int min_value,int max_value){
	
	int userinput = min_value - 1;
	
	while (  (std::cout << display_string) && 
		   ( !(std::cin >> userinput) || userinput < min_value || userinput > max_value))
	{
		std::cout << "Invalid entry -- try again:\n";
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
	return userinput;
}





//the main choice function for paramotopy.
int GetUserChoice(){
	
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



	
