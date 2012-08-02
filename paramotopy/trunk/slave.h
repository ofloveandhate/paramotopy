#include <mpi.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#include "step1_funcs.h"
#include "step2_funcs.h"
#include "mtrand.h"
#include "random.h"
#include "step2readandwrite.h"
#include "bertini_funcs.h"


#ifndef __SLAVE_H_INCLUDED__
#define __SLAVE_H_INCLUDED__










void slave(std::vector<std::string> dir, 
		   ToSave *TheFiles,
		   int numfiles,
		   std::vector<std::string> ParamNames, 
		   std::string filename,
		   int numfilesatatime,
		   std::string called_dir,
		   std::string templocation,
		   int newfilethreshold,
		   std::string location);


#endif
