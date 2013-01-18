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
#include <omp.h>
#include "bertini_funcs.hpp"
#include "step2_funcs.hpp"
#include "timing.hpp"


#ifndef __STEP2READANDWRITE_H__
#define __STEP2READANDWRITE_H__

bool SlaveCollectAndWriteData(int & numfiles,
							  std::vector<std::string> & runningfile,
							  const int  linenumber,
							  ToSave * TheFiles,
							  const std::vector<std::string> ParamNames,
							  const std::vector<std::pair<double,double> > AllParams,
							  int & buffersize,
							  const std::string DataCollectedbase_dir,
							  std::vector< int > & filesizes,
							  const int newfilethreshold,
							  const int myid,
							  timer & process_timer);

std::string AppendData(int runid, 
				std::string orig_file, 
				std::vector<std::string> ParamStrings,
				std::vector<std::pair<double, double> > CValues);

void WriteData(std::string outstring,
			   std::string target_file,
			   std::vector<std::string> ParamStrings);


int GetStart(std::string dir,
			  std::string & start,
			  std::string startfilename);

int GetLastNumSent(std::string base_dir,
					std::vector< int > & lastnumsent,
					int numprocs);

void GetRandomValues(std::string base_dir,
					 std::vector< std::pair<double,double> > & RandomValues);

void ReadDotOut(std::vector<std::string> & Numoutvector, 
				std::vector<std::string> & arroutvector,
				std::vector<std::string> & degoutvector,
				std::vector<std::string> & namesoutvector,
				std::vector<std::string> & configvector,
				std::vector<std::string> & funcinputvector);

void WriteDotOut(std::vector<std::string> & arroutvector,
				 std::vector<std::string> & degoutvector,
				 std::vector<std::string> & namesoutvector,
				 std::vector<std::string> & configvector,
				 std::vector<std::string> & funcinputvector);

void WriteNumDotOut(std::vector<std::string> Numoutvector,
					std::vector<std::pair<double,double> > AllParams,
					int numparam);

int GetMcNumLines(std::string base_dir, int numparam);


#endif
