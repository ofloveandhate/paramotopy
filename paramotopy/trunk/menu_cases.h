#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include "mtrand.h"
#include "paramotopy_enum.h"
#include "step1_funcs.h"
#include "step2_funcs.h"


#ifndef __MENU_CASES__
#define __MENU_CASES__



// user choice for making new random values for the start point
std::vector< std::pair<double,double> > random_case(std::vector< std::pair<double,double> > & RandomValues,
													std::vector<std::string> ParamStrings);

// user choice for saving random values to a text file
void save_random_case(std::vector< std::pair<double,double> > RandomValues,
					  std::ofstream & fout,
					  int numparam);

// user choice for loading the random values from a text file
void load_random_case(std::ifstream & fin3, 
					  std::vector< std::pair<double,double> > & RandomValues,
					  std::vector<std::string> ParamStrings);

// for running the step2 program from paramotopy
void step2_case(int numfilespossible,
				ToSave *TheFiles,
				std::string filename,
				bool parallel,
				preferences *Prefs,
				int numparam, 
				std::vector< std::pair<double,double> > RandomValues,
				std::vector<std::string> ParamStrings);


#endif


