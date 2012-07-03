#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <sstream>
#include "step1_funcs.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdexcept>

#ifndef __STEPTWO_H__
#define __STEPTWO_H__







void SetUpFolders(std::string base_dir,
				  int numprocs,
				  int numfilesatatime,
				  std::string templocation);

std::string WriteStep2(std::string config,
				std::vector<std::pair<double, double> > CValues,
				std::vector<std::string> FunctVector, 
				std::vector<std::string> VarGroupVector,
				std::vector<std::string> ParamVector,
				std::vector<std::string> ParamStrings,
				std::vector<std::string> Consts,
				std::vector<std::string> ConstantStrings,
				std::vector<std::pair<double,double> > RandomValues,
				int numfunct,
				int numvar,
				int numparam,
				int numconsts);

void MakeConstantsStep2(std::stringstream & inputstringstream,
						std::vector< std::pair<double,double> > RandomValues,
						std::vector< std::pair<double,double> > CValues,
						std::vector<std::string> ParamStrings,
						std::vector<std::string> Consts,
						std::vector<std::string> ConstantStrings,
						int numparam);




void CallBertiniStep2(std::string param_dir);



std::string MakeTargetFilename(std::string base_dir,
							   ToSave *TheFiles,
							   int index);


void TouchFilesToSave(ProgSettings paramotopy_settings,
					  std::string base_dir);




void SetFileCount(ProgSettings & paramotopy_settings,
				  std::string DataCollectedBaseDir);




#endif
