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

std::string WriteStep2(std::vector<std::pair<double, double> > CValues,
					   ProgSettings paramotopy_settings,
					   runinfo paramotopy_info);

std::string WriteFailStep2(std::vector<std::pair<double, double> > CValues,
						   ProgSettings paramotopy_settings,
						   runinfo paramotopy_info);




void CallBertiniStep2(std::string param_dir);



std::string MakeTargetFilename(std::string base_dir,
							   ToSave *TheFiles,
							   int index);


//void TouchFilesToSave(ProgSettings paramotopy_settings,
//					  std::string base_dir);




void SetFileCount(ProgSettings & paramotopy_settings,
				  std::string DataCollectedBaseDir);




#endif
