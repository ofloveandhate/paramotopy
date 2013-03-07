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
#include <iomanip>
#include <omp.h>


#ifndef __MASTER_H__
#define __MASTER_H__

#include "step1_funcs.hpp"
#include "step2_funcs.hpp"
#include "mtrand.hpp"
#include "random.hpp"

#include "para_aux_funcs.hpp"
#include "step2readandwrite.hpp"
#include "runinfo.hpp"
#include "timing.hpp"



/** 
 * the master process.  to be used by which ever process gets myid==0.  handles the distribution of the parameter points to the workers via MPI.
 * \param filename the name of the input file being processed.
 * \param numfilesatatime how many parameter points get passed to a worker at the appropriate time.
 * \param saveprogresseverysomany how often to write the progress to a text file.
 * \param called_dir the directory from which the program was called.
 * \param step2mode a numeric switch for choosing between saved sets of bertini and paramotopy settings
 * \param paramotopy_settings the object of containing the bertini and paramotopy settings used during the run.
 * \param paramotopy_info the runinfo object containing the parsed input file.
 * \param process_timer timing data.  only used ifdef verbose
 *
 */
void master(std::string filename,
			int numfilesatatime,
			int saveprogresseverysomany,
			std::string called_dir,
			int step2mode,
			ProgSettings & paramotopy_settings,
			runinfo & paramotopy_info,
			timer & process_timer);

/** 
 * the function for determining the parameter values to send next.
 * \param numfilesatatime how many parameter points get passed to a worker at the appropriate time.
 * \param numparam the number of parameters in the problem being solved.
 * \param pointcounter counts the number of loops.  determines which index in tempsends gets set.
 * \param Values the parameter discretization values.
 * \param countingup counter.
 * \param Kvector for converting the number corresponding to the parameter point to an index for lookup in Values
 * \param tempsends the variable in which the parameter points get set, for distribution to the workers via MPI.
 */
void FormNextValues(int numfilesatatime,
						int numparam,
						int pointcounter,	
						std::vector< std::vector< std::pair<double,double> > > Values,
						int countingup,
					  std::vector< int > KVector,
						double tempsends[]);

/**
 * the user-defined parameter set function for setting the parameter values to send next
 * \param numfilesatatime how many parameter points get passed to a worker at the appropriate time.
 * \param numparam the number of parameters in the problem being solved.
 * \param pointcounter counts the number of loops.  determines which index in tempsends gets set.
 * \param mc_line_number the line number of the current parameter point, in the mc file.
 * \param mc_in_stream the file from which we are reading parameter points.
 * \param tempsends the variable in which the parameter points get set, for distribution to the workers via MPI.
 */
void FormNextValues_mc(int numfilesatatime,
					   int numparam,
					   int pointcounter,	
					   int mc_line_number,
					   std::ifstream & mc_in_stream,
					   double tempsends[]);

#endif
