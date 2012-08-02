
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <sstream>
#include <fstream>
#include <string>
#include "random.h"
#include "mtrand.h"
#include <stdexcept>
#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include "paramotopy_enum.h"

#define BOOST_FILESYSTEM_VERSION 3
#define BOOST_FILESYSTEM_NO_DEPRECATED

#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/progress.hpp>
#include <boost/regex.hpp>


#ifndef __PARA_AUX_H__
#define __PARA_AUX_H__



std::vector<std::string> FindFiles(std::string dir, std::string expression);
std::vector<boost::filesystem::path> FindDirectories(std::string dir, std::string expression);


int get_int_choice(std::string display_string,int min_value,int max_value);



std::string make_base_dir_name(std::string filename);



// the main menu for paramotopy
int GetUserChoice();




std::string stackoverflow_getcwd();

void mkdirunix(std::string mydir);


#endif

