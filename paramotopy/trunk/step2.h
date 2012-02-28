#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <sstream>
#include "step1.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#ifndef __STEPTWO_H__
#define __STEPTWO_H__

struct ToSave{
  
  std::string filename;
  bool saved;
  int filecount;

};





void SetUpFolders(std::string base_dir,
			 int numprocs,
			 int numfilesatatime);

void WriteStep2(std::vector< std::string > configvector, 
		std::ofstream & fout,
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

void MakeConstantsStep2(std::ofstream & fout,
			std::vector< std::pair<double,double> > RandomValues,
		        std::vector< std::pair<double,double> > CValues,
			std::vector<std::string> ParamStrings,
			std::vector<std::string> Consts,
			std::vector<std::string> ConstantStrings,
			int numparam);


//void CallCopyStartStep2(std::string base_dir, std::string param_dir);
// antiquated 11.27.04 DAB

void CallBertiniStep2(std::string param_dir);



std::string MakeTargetFilename(std::string base_dir,
			       ToSave *TheFile,
			       int index);
			       //	       bool & append);

void TouchFilesToSave(ToSave *TheFiles,int numfilespossible,
		      std::string base_dir);


//void UpdateFileCount(ToSave *TheFiles, int numfiles,
//		     std::string DataCollectedBaseDir);


void SetFileCount(ToSave *TheFiles, int numfiles,
		  std::string DataCollectedBaseDir);


#endif
