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



#ifndef __SLAVE_H_INCLUDED__
#define __SLAVE_H_INCLUDED__


#include "step1_funcs.hpp"
#include "step2_funcs.hpp"
#include "mtrand.hpp"
#include "random.hpp"
#include "step2readandwrite.hpp"
#include "timing.hpp"

// you must include gmp pour include polysolve.
#include <gmp.h>

extern "C" {
#include "polysolve.h"
}




/**
 * the slave process.  to be used all processes except myid==0.  gets work to be done from the master, and runs bertini on the appropriate parameter points.
 * \param TheFiles a pointer to a struct containing the names of the files to be saved.
 * \param numfiles the number of files to be saved.
 * \param ParamNames the names of the parameters.
 * \param filename the name of the input file being processed.
 * \param numfilesatatime how many parameter points get passed to a worker at the appropriate time.
 * \param called_dir the directory from which the program was called.
 * \param tmpfolder the name of the folder in which to do the work.
 * \param newfilethreshold number of KB, over which a new data file is started.  
 * \param location 
 * \param buffersize number of KB, if exceeded the data file is written to disk.
 * \param paramotopy_settings object which holds the bertini and paramotop settings for this run
 * \param paramotopy_info the parsed input file.
 * \param process_timer timing data.  only used ifdef verbose
 *
 */
void slave(ToSave *TheFiles,
	   int numfiles,
	   std::vector<std::string> ParamNames, 
	   std::string filename,
	   int numfilesatatime,
	   std::string called_dir,
	   std::string tmpfolder,
	   int newfilethreshold,
	   std::string location,
	   int buffersize,
	   ProgSettings & paramotopy_settings,
	   runinfo & paramotopy_info,
	   timer & process_timer);



#endif
