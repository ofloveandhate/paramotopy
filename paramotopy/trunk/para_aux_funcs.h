
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



#ifndef __PARA_AUX_H__
#define __PARA_AUX_H__

//finds the step 2 program.  searches current directory, then the path variable.  if not found, prompts the user for the location.
// definition of preferences struc found in preferences.h

int get_int_choice(std::string display_string,int min_value,int max_value);



std::string make_base_dir_name(std::string filename);



// the main menu for paramotopy
int GetUserChoice();




std::string stackoverflow_getcwd();

void mkdirunix(std::string mydir);


#endif

