#include <mpi.h>
#include "step1_funcs.hpp"
#include "mtrand.hpp"
#include "random.hpp"
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
#include "bertini_funcs.hpp"
#include "step2_funcs.hpp"
#include "timing.hpp"


#ifndef __STEP2READANDWRITE_H__
#define __STEP2READANDWRITE_H__


/**
 * the computeNumdenom function is in the bertini library.  it takes in a character array, and pointers which return the numerator and denominator of the number.  We pass the input by casting via (char *)
 * \param s the number to be converted to rational form
 * \return numer the numerator
 * \return denom the denomenator
 */
extern "C" {
	void computeNumDenom(char **numer, char **denom, char *s);
}





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
 * reads data from bertini output files, and writes it to files in DataCollected folder, if the threshold is big enough
 * \param numfiles The number of files being saved.
 * \param runningfile MUTABLE The vector of strings containing the data accrued so far.
 * \param linenumber Parameter point index
 * \param TheFiles Struct containing the names of the files to be collected
 * \param ParamNames Names of the parameters
 * \param AllParams Current parameter values.
 * \param buffersize The threshold for dumping to disk
 * \param DataCollectedbase_dir The directory in which to write data
 * \param filesizes Keeps track of how large the files are, in number of characters.
 * \param newfilethreshold Number of KB over which a new data file is started
 * \param myid Worker id determined by MPI
 * \param process_timer Timer object for collecting timing data.
 */
bool SlaveCollectAndWriteData(int & numfiles,
							  std::vector<std::string> & runningfile,
							  const int  linenumber,
							  ToSave * TheFiles,
							  const std::vector<std::string> ParamNames,
							  const std::vector<std::pair<double,double> > AllParams,
							  int buffersize,
							  const std::string DataCollectedbase_dir,
							  std::vector< int > & filesizes,
							  const int newfilethreshold,
							  const int myid,
							  timer & process_timer);

/**
 * creates a string to append read-in data to the running file.
 * \param point_index parameter point index
 * \param orig_filename Name of the data file to be read.
 * \param ParamStrings deprecated, needs removal.
 * \param CValues Currend parameter values.
 * \return String of the entire data file.
 */
std::string AppendData(int point_index,
				std::string orig_filename,
				std::vector<std::string> ParamStrings,
				std::vector<std::pair<double, double> > CValues);

/** 
 * Write data to disk.
 * \param outstring The file to write
 * \param target_file name of the file to write to.
 * \ParamStrings names of the parameters
 */
void WriteData(std::string outstring,
			   std::string target_file,
			   std::vector<std::string> ParamStrings);

/**
 * reads in the start file to memory
 * \param dir The directory in which to read the start file
 * \param start MUTABLE String containing the start file
 * \param startfilename The name of the start file. This is a user-set name.
 */
int GetStart(std::string dir,
			  std::string & start,
			  std::string startfilename);

/**
 * Deprecated.  Gets the highest index set on a previous attempt at the run.  
 * \param base_dir Directory in which to look for the lastnumsent0,1 files
 * \param lastnumsent Mutable vector in which to place the lastnumsent info read in by this function
 * \param numprocs Number of processors in this attempt.
 */
int GetLastNumSent(std::string base_dir,
					std::vector< int > & lastnumsent,
					int numprocs);


/**
 * Reads in the random values to be used in this solve.
 * \param base_dir Directory in which to look for the random values file.
 * \param RandomValues Mutable vector in which to store the random values read in by this function.
 */
void GetRandomValues(std::string base_dir,
					 std::vector< std::pair<double,double> > & RandomValues);


/**
 * Reads in all .out files, into mutable vectors of strings.  Assumes the process is working in the directory containing the files.
 * \param Numoutvector Stores the num.out file.
 * \param arroutvector Stores the arr.out file.
 * \param degoutvector Stores the deg.out file.
 * \param namesoutvector Stores the names.out file.
 * \param configvector Stores the config file.
 * \param funcinputvector Stores the func_input file.
 */
void ReadDotOut(std::vector<std::string> & Numoutvector, 
				std::vector<std::string> & arroutvector,
				std::vector<std::string> & degoutvector,
				std::vector<std::string> & namesoutvector,
				std::vector<std::string> & configvector,
				std::vector<std::string> & funcinputvector);

/**
 * Writes to disk the .out files, but not num.out.  This function needs only be called once, and the process must be in the correct directory.
 * \param arroutvector Stores the arr.out file.
 * \param degoutvector Stores the deg.out file.
 * \param namesoutvector Stores the names.out file.
 * \param configvector Stores the config file.
 * \param funcinputvector Stores the func_input file.
 */
void WriteDotOut(std::vector<std::string> & arroutvector,
				 std::vector<std::string> & degoutvector,
				 std::vector<std::string> & namesoutvector,
				 std::vector<std::string> & configvector,
				 std::vector<std::string> & funcinputvector);

/**
 * Writes the num.out file for the bertini solve.  We overwrite the appropriate places of the original file with the current parameter values.
 * \param Numoutvector Stores the num.out file, as read in by ReadDotOut.
 * \param AllParams The current parameter values.
 * \param numparam The numbers of parameters in the problem.
 */
void WriteNumDotOut(std::vector<std::string> Numoutvector,
					std::vector<std::pair<double,double> > AllParams,
					int numparam);

/**
 * Gets the number of lines in the mc file.  essentially reads the top line of the file.
 * \param base_dir The directory in which to look for the mcfile.
 * \param numparam Number of parameters in the problem.  Deprecated?
 * \return terminationint  - The number of parameter points in the file.  Note that this could acceptably be smaller than the actual number, but not larger.
 */
int GetMcNumLines(std::string base_dir, int numparam);


#endif
