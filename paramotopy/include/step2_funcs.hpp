#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <sstream>


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdexcept>



#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <cmath>



#ifndef __STEPTWO_H__
#define __STEPTWO_H__

#include "step1_funcs.hpp"

#include <gmp.h>

extern "C" {
	#include "bertini.h"
}


/**
 * a wrapper around the parse_input() function from the bertini library.
 * \return MPType an integer indicating the mptype.
 * \return currentSeed the current random seed.
 */
void parse_input_file_bertini(unsigned int & currentSeed, int & MPType);



/**
 * a wrapper around the zero_dim_main() function from the bertini library.
 * \param MPType an integer indicating the mptype.
 * \param currentSeed the current random seed.
 */
void run_zero_dim_main(int MPType, unsigned int currentSeed);




/**
 * sets up the various data folders for the run.  also sets up the text file which has the list of folders containing data
 * \param base_dir the directory in which to do the setup
 * \param numprocs the number of processors being used
 * \param numfilesatatime the number of parameter points to hand out at each distribution
 * \param templocation the location of the temporary files, in which each worker will produce the data by running bertini
 */
void SetUpFolders(std::string base_dir,
				  int numprocs,
				  int numfilesatatime,
				  std::string templocation);

/** 
 * creates the bertini input file for step2, and writes it to a string.
 * \param CValues current parameter values.
 * \param paramotopy_settings the bertini and paramotopy settings
 * \param paramotopy_info the parsed input file.
 */
std::string WriteStep2(std::vector<std::pair<double, double> > CValues,
		       ProgSettings paramotopy_settings,
		       runinfo paramotopy_info);

/**
 * creates the bertini input file for step2 IN FAILURE ANALYSIS MODE, and writes it to a string.
 * \param CValues current parameter values.
 * \param paramotopy_settings the bertini and paramotopy settings
 * \param paramotopy_info the parsed input file.
 */
std::string WriteFailStep2(std::vector<std::pair<double, double> > CValues,
			   ProgSettings paramotopy_settings,
			   runinfo paramotopy_info);




//void CallBertiniStep2(std::string param_dir);


/** 
 * makes the appropriate output file name
 * \param base_dir the string of style bfiles_filename/run0/
 * \param TheFiles struct containing the names of the data files to be saved.
 * \param index the number of the filename to make
 * \return concatenated file name.
 */
std::string MakeTargetFilename(std::string base_dir,
			       ToSave *TheFiles,
			       int index);


//void TouchFilesToSave(ProgSettings paramotopy_settings,
//					  std::string base_dir);



/**
 * makes the appropriate output file name
 * \param paramotopy_settings the bertini and paramotopy settings
 * \param DataCollectedBaseDir the name of the bfiles_filename/run0/step2/DataCollected location
 */
void SetFileCount(ProgSettings & paramotopy_settings,
				  std::string DataCollectedBaseDir);




#endif
