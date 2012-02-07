#include "random.h"
#include "mtrand.h"
#include "step1.h"
#include "step2.h"
#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <stdio.h>



#ifndef __PARA_AUX_H__
#define __PARA_AUX_H__


int GetUserChoice(int intChoice);

std::vector< std::pair<double,double> > random_case(std::vector< std::pair<double,double> > & RandomValues,
													std::vector<std::string> ParamStrings);

void save_random_case(std::vector< std::pair<double,double> > RandomValues,
					  std::ofstream & fout,
					  int numparam);

void load_random_case(std::ifstream & fin3, 
					  std::vector< std::pair<double,double> > & RandomValues,
					  std::vector<std::string> ParamStrings);

void step2_case(int numfilespossible,
				ToSave *TheFiles,
				std::string filename,
				bool parallel,
				preferences *Prefs,
				int numparam, 
				std::vector< std::pair<double,double> > RandomValues,
				std::vector<std::string> ParamStrings);

#endif

