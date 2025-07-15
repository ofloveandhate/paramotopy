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
#include "paramotopy/mtrand.hpp"
#include "paramotopy/step1_funcs.hpp"
#include "paramotopy/random.hpp"
#include "paramotopy/step2_funcs.hpp"
#include "paramotopy/para_aux_funcs.hpp"
#include "paramotopy/paramotopy_enum.hpp"
#include "paramotopy/menu_cases.hpp"
#include "paramotopy/failed_paths.hpp"
#include "paramotopy/datagatherer.hpp"
#include "paramotopy/xml_preferences.hpp"
//#include "paramotopy/preferences.h"
#include "paramotopy/tinyxml.h"
#include <mpi.h>
#include "paramotopy/runinfo.hpp"



/** Display the main paramotopy choice menu. 
 \return int - The integer choice of the user for the main menu. */
int ParamotopyMainMenu();

void SetNewInput(runinfo & paramotopy_info, ProgSettings & paramotopy_settings);
