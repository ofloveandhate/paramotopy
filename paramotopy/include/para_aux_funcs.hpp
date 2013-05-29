
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <sstream>
#include <fstream>
#include <string>

#include <stdexcept>
#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <stdio.h>


#define BOOST_FILESYSTEM_VERSION 3
#define BOOST_FILESYSTEM_NO_DEPRECATED

#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/progress.hpp>
#include <boost/regex.hpp>


#ifndef __PARA_AUX_H__
#define __PARA_AUX_H__

#include "paramotopy_enum.hpp"
#include "random.hpp"
#include "mtrand.hpp"
//#include "xml_preferences.hpp"

/** Function to call the bertini libraries. */

extern "C" {
	int bertini_main(int argC, char *args[]);
}


/**
 Display the Paramotopy splash screen.
 */
void ParamotopySplashScreen();


/** 
    Display the bertini version information.
*/
void BertiniSplashScreen(std::string bertinilocation);




/**
 get the index for the particular filename.  not the fastest lookup, should only perform seldomly.
 \param filename the string name of the file.
 \return parser index of the filename
 */
int GetFileParserIndex(std::string filename);




/** Get the files to parse.
 \param run_to_analyze - The path of the folder to analyze.
 \param gather_savefiles - Gather all the files to be saved 
 \param gather_parser_indices - A vector that that stores info as to whether or not to
 gather a particular file.
*/

void GetFilesToParse(boost::filesystem::path run_to_analyze, std::vector< std::string > & gather_savefiles, std::vector< int > & gather_parser_indices);


/** 
    Test if a step2 run is finished.
    \param paths_to_check
    \return bool - A boolean to indicate whether or not the step 2 run is finished.
 */
bool TestIfFinished(boost::filesystem::path & path_to_check);

/** 
    The parser for the failed_paths file type which is output from bertini.
    \param fin - The input file stream of the failed_paths file to parse
    \param numvariables - The number of variables.
    \return int - The number of failed_paths for a particular parameter point.
 */

int ParseFailedPaths(std::ifstream & fin, int numvariables);

/**
   Finds a user-entered tilde, and replaces it with the $HOME variable.
   \param workwithme - The string to test if a ~ exists to replace with the $HOME variable.
   \return string - The replacement string.
*/
std::string replace_tilde_with_home(std::string workwithme);

/** 
    Searches a directory for all *directories* fitting a regular expression.
    \param dir - The directory to search.
    \param expression - The expression for which to search.
    \return vector<string> - A vector of strings that stores the found directories.
 */
std::vector<boost::filesystem::path> FindFiles(boost::filesystem::path dir, std::string expression);

/**
   Searches a directory for all files fitting a regular expression. 
   \param dir - The directory to search.
   \param expression - The expression for which to search.
   \return vector<path> - A vector of boost style paths that stores the files found.
 */
std::vector<boost::filesystem::path> FindDirectories(boost::filesystem::path, std::string expression);

/** 
    Get alphnumeric input from the user with spaces.
    \return string - The string inputted by the user.
 */

std::string getAlphaNumeric_WithSpaces();

/** 
    Get alphanumeric input from the user without spaces.
    \return string - The string inputted by the user.
*/
std::string getAlphaNumeric();


/**
   Set results to the value in the string.
   \param text - The double as a string value.
   \param results - The double to store the string value as a double.
   \return bool - A boolean to indicate whether the parsing was successful or not.
*/
bool parseDouble( std::string const& text, double& results );

/**
   Have the user input a value until it's a double, return that value.
   \return double - The double the user inputted.

*/
double getDouble();

/**
   Have the user input a value untl it's an integer, return that value.
   \return int - The integer the user inputted.
*/

int getInteger();

/** 
    Parse a string that has an integer value in string form.
    \param text  - The integer value as a string.
    \param results - The value to set as a string.
    \return bool - A boolean to indicate whether the parsing was successful or not.
*/

bool parseInteger( std::string const& text, int& results );

/** 
    Display a menu option to the user and ask for an integer input within the specified range.
    \param display_string - The menu as a string.
    \param min_value - The minimum value allowed.
    \param max_value - The maximum value allowed.
    \return int - The integer the user specified.
 */

int get_int_choice(std::string display_string,int min_value,int max_value);


/** 
    Make the base directory given the associated paramotopy input filename.
    \param filename - The paramotopy input filename.
    \return string - A string of the format "bfiles_filename", where filename is the string value of filename.
*/
boost::filesystem::path make_base_dir_name(boost::filesystem::path filename);



/** Display the main paramotopy choice menu. 
 \return int - The integer choice of the user for the main menu. */
int ParamotopyMainMenu();



/** 
    Get the current working directory directly from stackoverflow
    \return string - The current working directory as a string..
 */
std::string stackoverflow_getcwd();



void safe_chdir(std::string desired_directory);

std::string convert_spaces_to_escaped(std::string workwithme);

#endif

