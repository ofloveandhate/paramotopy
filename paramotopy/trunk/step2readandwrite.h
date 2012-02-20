#include <mpi.h>
#include "step1.h"
//#include "step2.h"
#include "mtrand.h"
#include "random.h"
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

#define timingstep2


#ifndef __STEP2READANDWRITE_H__
#define __STEP2READANDWRITE_H__

void WriteData(int runid, 
			   std::string orig_file, 
			   std::string target_file,
			   //	       bool append,
			   std::vector<std::string> ParamStrings,
			   std::vector<std::pair<double, double> > CValues);


void GetStartConfig(std::string base_dir,
					std::vector< std::string > & startvector,
					std::vector< std::string > & configvector);

void GetLastNumSent(std::string base_dir,
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

#endif
