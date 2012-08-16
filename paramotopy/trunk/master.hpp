#include <mpi.h>
#include "step1_funcs.hpp"
#include "step2_funcs.hpp"
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
#include "para_aux_funcs.hpp"
#include "step2readandwrite.hpp"
#include "runinfo.hpp"

#ifndef __MASTER_H__
#define __MASTER_H__

void master(std::vector<std::string> dir,
			std::string filename,
			int numfilesatatime,
			int saveprogresseverysomany,
			std::string called_dir,
			std::string templocation,
			std::string location);

void FormNextValues(int numfilesatatime,
						int numparam,
						int localcounter,	
						std::vector< std::vector< std::pair<double,double> > > Values,
						int countingup,
					  std::vector< int > KVector,
						double tempsends[]);
	
void FormNextValues_mc(int numfilesatatime,
					   int numparam,
					   int localcounter,	
					   int countingup,
					   std::ifstream & mc_in_stream,
					   double tempsends[]);

#endif
