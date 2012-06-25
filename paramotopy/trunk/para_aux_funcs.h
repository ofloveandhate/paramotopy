
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

#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include "paramotopy_enum.h"



#ifndef __PARA_AUX_H__
#define __PARA_AUX_H__

//finds the step 2 program.  searches current directory, then the path variable.  if not found, prompts the user for the location.
// definition of preferences struc found in preferences.h

int get_int_choice(std::string display_string,int min_value,int max_value);


std::string make_base_dir_name(std::string filename);

int Find_Program_Step2(preferences *Prefs);


// the main menu for paramotopy
int GetUserChoice();




// checks if can recover a failed run.  mostly broken due to implemented incremental saving in step2 program.
void TryToRecoverPrevRun(std::vector< std::pair<double,double> > & RandomValues,
						 std::string filename,
						 std::string base_dir,
						 preferences *Prefs,
						 OPTIONS & currentChoice,
						 std::vector<std::string> ParamStrings);

// checks for 'finished' file in the bfiles folder associated with the loaded filename.
bool test_if_finished(std::string base_dir);

#endif

