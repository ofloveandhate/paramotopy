//contains the runinfo class
#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include "random.h"
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <sstream>
#include <fstream>
#include <string>
#include "para_aux_funcs.h"

#ifndef __RUNINFO_H__
#define __RUNINFO_H__


class runinfo {
	
public:
	
	// values of points -- two different ways storing the data
	// whether or not we are user-defined or not
	// if doing a mesh, the data is contained as follows
	// -- first vector size is the number of parameters
	// -- second vector size the number of mesh points for the given param
	// -- pair is the real (first) and imaginary (second)
	
	// if not doing a mesh, the data is contained as follows
	// -- first vector size is number of sample n-tuples)
	// -- second vector size is n
	// -- pair is the real (first) and imaginary (second)
	
	int numfunct, numvariables, numvargroup, numparam, numconsts;
	std::string location;//this may well be changed/ renamed/ deleted
	std::string base_dir;
	
	std::string inputfilename;
	
	std::vector<std::string> Functions; 
	std::vector<std::string> VarGroups; 
	std::vector<std::string> Parameters; 
	std::vector<std::string> ParameterNames;
	std::vector<std::string> Constants;
	std::vector<std::string> ConstantNames;
	
	std::vector< std::vector< std::pair<double,double> > > Values;
	
	bool userdefined;
	
	std::string mcfname;  
	std::vector< int > NumMeshPoints;
	
	// random initial values
	// -- vector size is the number of parameters
	// -- pair is the real (first) and imaginary (second)
	std::vector< std::pair<double,double> > RandomValues;
	
	runinfo(){};//default constructor
	

	std::string WriteInputStepOne();
	std::string WriteInputStepTwo(std::vector<std::pair<double, double> > tmprandomvalues);
	void GetInputFileName();
	void GetInputFileName(std::string suppliedfilename);
	void make_base_dir_name();
	
	
	void mkdirstep1(){
		
		std::string tempstr = base_dir;
		mkdirunix(tempstr);
		tempstr.append("/step1");
		mkdirunix(tempstr);
		
	};
	
	
	void ParseData();
	
	void SaveRandom();
	void LoadRandom();
	void SetRandom();
	
	void SetLocation();
	bool test_if_finished();
	void DisplayAllValues();
	
	std::vector<std::pair<double, double> > MakeRandomValues(int garbageint);
	
	
private:
	void GetNumVariables();
	void ReadSizes(std::ifstream & fin);
	void ReadParameters(std::ifstream & fin);
	void ReadFunctions(std::ifstream & fin);
	void ReadVarGroups(std::ifstream & fin);
	void ReadConstants(std::ifstream & fin);
	void ReadConstantStrings(std::ifstream & fin);
	
	void MakeParameterNames();
	
	void MakeVariableGroups(std::stringstream & fout);
	
	void MakeDeclareConstants(std::stringstream & fout);
	void MakeConstants(std::stringstream & fout);
	void MakeConstantsStep2(std::vector<std::pair<double, double> > CValues, std::stringstream & inputfilestream);
	
	
	void MakeDeclareFunctions(std::stringstream & inputstringstream);
	void MakeFunctions(std::stringstream & inputstringstream);
	

	
	void MakeRandomValues();
	void MakeRandomValues(std::vector< std::pair< std::pair< double, double >, 
						  std::pair< double, double > > > RandomRanges);
	
	
	void MakeValues(std::ifstream & fin);
	void MakeValues();
	void PrintRandom();
	
	
protected:
	
};



#endif


