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
#include "timing.hpp"

#ifndef __MASTER_H__
#define __MASTER_H__

void master(std::string filename,
			int numfilesatatime,
			int saveprogresseverysomany,
			std::string called_dir,
			int step2mode,
			ProgSettings & paramotopy_settings,
			runinfo & paramotopy_info,
			timer & process_timer);

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
