#include "random.h"
#include "mtrand.h"
#include "step1.h"
#include "step2.h"
#include "para_aux_funcs.h"
#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <stdio.h>



#ifndef __FAILED_PATHS_H__
#define __FAILED_PATHS_H__

int failedpaths_case(int numparam, 
					 int numvariables, 
					 std::string base_dir,
					 std::string filename,
					 int & iteration);

int find_failed_paths(std::vector< int > & index_vector,
					  std::vector< std::vector< std::pair<double,double> > > & fail_vector, 
					  int & totalfails, 
					  int numparam, 
					  int numvariables, 
					  std::string base_dir,
					  std::vector< std::string > foldervector);

std::vector< std::string > get_folders_for_fail(std::string base_dir);

#endif


