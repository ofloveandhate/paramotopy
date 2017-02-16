// the main header file for paramotopy's main.


#pragma once

#include <iostream>
#include <ios>
#include <string>
#include <fstream>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <list>
#include <vector>
#include <map>
#include <sstream>
#include <cmath>
#include "mtrand.hpp"
#include "step1_funcs.hpp"
#include "random.hpp"
#include "step2_funcs.hpp"
#include "para_aux_funcs.hpp"
#include "paramotopy_enum.hpp"
#include "menu_cases.hpp"
#include "failed_paths.hpp"
#include "datagatherer.hpp"
#include "xml_preferences.hpp"
//#include "preferences.h"
#include "tinyxml.h"
#include <mpi.h>
#include "runinfo.hpp"



/** Display the main paramotopy choice menu. 
 \return int - The integer choice of the user for the main menu. */
int ParamotopyMainMenu();

void SetNewInput(runinfo & paramotopy_info, ProgSettings & paramotopy_settings);
