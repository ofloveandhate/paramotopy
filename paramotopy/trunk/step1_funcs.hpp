#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include "random.hpp"
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <sstream>
#include <fstream>
#include <string>
#include "para_aux_funcs.hpp"
#include "xml_preferences.hpp"
#include "runinfo.hpp"
#include <omp.h>

#ifndef __STEPONE_H__
#define __STEPONE_H__





void WriteMeshToMonteCarlo(int level, std::vector<std::vector<std::pair<double,double> > > Values, std::ofstream fout,std::string cline);



void WriteStep1(ProgSettings paramotopy_settings,
				runinfo paramotopy_info);




void CallBertiniStep1(ProgSettings paramotopy_settings, runinfo paramotopy_info);

/** Store what the parameters are as strings in a vector 
    @param ToParse -  the string, space delimited, that has the
    appropriate mesh information.
*/

//std::vector< std::string > MakeParameterStrings(
//	     				std::vector<std::string> ToParse);

//std::vector< std::vector< std::pair<double,double> > > MakeValues(
//					std::vector<std::string> ToParse, std::vector< int > & NumMeshPoints);
//
//
//std::vector< std::vector< std::pair<double,double>  > > MakeValues(
//				       int numparam, std::ifstream & fin);

//void WriteStep1(std::string filename, 
//		std::string configfilename, 
//		std::vector<std::string> & FunctVector, 
//		std::vector<std::string> & ParamStrings, 
//		std::vector<std::string> & VarGroupVector,
//		std::vector<std::string> & Consts,
//		std::vector<std::string> & ConstantStrings,
//		std::vector<std::pair<double, double> > & RandomValues);

// assuming we open the input file stream and the output filestream
// duplicate void MakeConfig(std::ifstream & fin, std::ofstream & fout);

void WriteMeshToMonteCarlo(int level, 
	       std::vector<std::vector<std::pair<double, double> > > Values, 
			   std::string dirfilename, 
			   std::string cline);

#endif

