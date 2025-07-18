#include <mpi.h>
#include "paramotopy/step1_funcs.hpp"
#include "paramotopy/mtrand.hpp"
#include "paramotopy/random.hpp"
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



#ifndef __STEP2READANDWRITE_H__
#define __STEP2READANDWRITE_H__


/**
 * the computeNumdenom function is in the bertini library, but not the headers, hence its inclusion here.  it takes in a character array, and pointers which return the numerator and denominator of the number.  We pass the input by casting via (char *)
 * \param s the number to be converted to rational form
 * \return numer the numerator
 * \return denom the denomenator
 */
//extern "C" {
//	void computeNumDenom(char **numer, char **denom, char *s);
//}

#include "paramotopy/step2_funcs.hpp"
#include "paramotopy/timing.hpp"

//#include <gmp.h>



/**
 * gets the number of points in the parameter set, and opens the mc_out_stream file (if applicable).  for both the user-defined style and the computer-generated.
 * \param mc_in_stream MUTABLE the stream for reading parameter points in the case of user-defined parameter points.
 * \param mc_out_stream MUTABLE the stream for WRITING parameter points in case of computer-generated points.
 * \param terminationint MUTABLE the number of points in the solve
 * \param KVector vector of indices, for conversion from an integer to an index vector.
 * \param paramotopy_info the parsed input file.
 * \param paramotopy_settings the bertini and paramotopy settings for the current run.
 */
void getTermination_OpenMC(std::ifstream & mc_in_stream,std::ofstream & mc_out_stream,int & terminationint,std::vector< int > & KVector,runinfo & paramotopy_info,ProgSettings & paramotopy_settings);




/**
 * Deprecated.  Gets the highest index set on a previous attempt at the run.  
 * \param base_dir Directory in which to look for the lastnumsent0,1 files
 * \param lastnumsent Mutable vector in which to place the lastnumsent info read in by this function
 * \param numprocs Number of processors in this attempt.
 */
int GetLastNumSent(boost::filesystem::path base_dir,
					std::vector< int > & lastnumsent,
					int numprocs);




/**
 * Reads in all .out files, into mutable vectors of strings.  Assumes the process is working in the directory containing the files.
 * \param Numoutvector Stores the num.out file.
 * \param arroutvector Stores the arr.out file.
 * \param degoutvector Stores the deg.out file.
 * \param namesoutvector Stores the names.out file.
 * \param configvector Stores the config file.
 * \param funcinputvector Stores the func_input file.
 * \param preproc_datavector Store the preproc_data file.
 */
void ReadDotOut(std::vector<std::string> & Numoutvector, 
				std::vector<std::string> & arroutvector,
				std::vector<std::string> & degoutvector,
				std::vector<std::string> & namesoutvector,
				std::vector<std::string> & configvector,
		std::vector<std::string> & funcinputvector,
		std::vector<std::string> & preproc_datavector);





/**
 * Writes to disk the .out files, but not num.out.  This function needs only be called once, and the process must be in the correct directory.
 * \param arroutvector Stores the arr.out file.
 * \param degoutvector Stores the deg.out file.
 * \param namesoutvector Stores the names.out file.
 * \param configvector Stores the config file.
 * \param funcinputvector Stores the func_input file.
 * \param preproc_datavector Stores the preproc_data file. 
 */
void WriteDotOut(std::vector<std::string> & arroutvector,
		 std::vector<std::string> & degoutvector,
		 std::vector<std::string> & namesoutvector,
		 std::vector<std::string> & configvector,
		 std::vector<std::string> & funcinputvector,
		 std::vector<std::string> & preproc_datavector);



/**
 * Writes the num.out file for the bertini solve.  We overwrite the appropriate places of the original file with the current parameter values.
 * \param Numoutvector Stores the num.out file, as read in by ReadDotOut.
 * \param AllParams The current parameter values.
 * \param numparam The numbers of parameters in the problem.
 * \param standardstep2 Flag to indicate if a parameter continuation run is done *                     or if a default bertini run is done on multiple parameter points

 */
void WriteNumDotOut(std::vector<std::string> Numoutvector,
		    std::vector<std::pair<double,double> > AllParams,
		    int numparam,
		    bool standardstep2);




#endif
