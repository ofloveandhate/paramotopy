
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <sstream>
#include <fstream>
#include <string>
#include "random.hpp"
#include "mtrand.hpp"
#include <stdexcept>
#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include "paramotopy_enum.hpp"

#define BOOST_FILESYSTEM_VERSION 3
#define BOOST_FILESYSTEM_NO_DEPRECATED

#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/progress.hpp>
#include <boost/regex.hpp>


#ifndef __PARA_AUX_H__
#define __PARA_AUX_H__


void GetFilesToParse(boost::filesystem::path run_to_analyze, std::vector< std::string > & gather_savefiles, std::vector< int > & gather_parser_indices);



bool TestIfFinished(boost::filesystem::path & path_to_check);



int ParseFailedPaths(std::ifstream & fin, int numvariables);

/// takes in a string, finds a user-entered tilde, and replaces it with the $HOME variable.
std::string replace_tilde_with_home(std::string workwithme);


std::vector<std::string> FindFiles(std::string dir, std::string expression);
std::vector<boost::filesystem::path> FindDirectories(std::string dir, std::string expression);

std::string getAlphaNumeric_WithSpaces();
std::string getAlphaNumeric();

bool parseDouble( std::string const& text, double& results );
double getDouble();

int getInteger();
bool parseInteger( std::string const& text, int& results );
int get_int_choice(std::string display_string,int min_value,int max_value);



std::string make_base_dir_name(std::string filename);



// the main menu for paramotopy
int ParamotopyMainMenu();




std::string stackoverflow_getcwd();

void mkdirunix(std::string mydir);


#endif

