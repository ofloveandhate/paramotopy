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

#ifndef __STEPONE_H__
#define __STEPONE_H__





/** Function to write the shell script that calls bertini in the step 1 process */
void WriteShell1(int architecture,int usemachine);

void WriteShell1Parallel(int architecture, int usemachine);

//void WriteShell2();

//void WriteShell3(int architecture);


void ParseData(int & numfunct, 
	       int & numvargroup, int & numparam, int & numconsts,
	       std::vector<std::string> & FunctVector,
	       std::vector<std::string> & VarGroupVector, 
	       std::vector<std::string> & ParamVector, 
	       std::vector< std::string > & ParamStrings,
	       std::vector<std::string>  & Consts,
	       std::vector<std::string> & ConstantStrings,
	       std::vector< std::vector< std::pair<double,double> > > & Values,  
	       std::vector< std::pair<double,double> > & RandomValues,
	       bool & userdefined,
	       std::ifstream & fin, std::vector< int > & NumMeshPoints,
	       std::string filename);

int GetNumVariables(int numvargroup, std::vector<std::string> VarGroupVector);



/**
   Read the user-given values found in the configuration file and set
   the parameters to the appropriate values
   @param numfunct - number of functions
   @param numvargroup - number of variable groups
   @param numparam - number of parameters
   @param fin - the file input stream
*/


void ReadSizes(int & numfunct, int & numvargroup, int & numparam, int & numconsts,
	       std::ifstream & fin);


/** Read in the functions from the given input file stream 
    @param numfunct - the number of functions
    @param fin - the file input stream
*/

std::vector<std::string> ReadFunctions(int numfunct, std::ifstream & fin);

/** Read in the variable groups from the given input file stream
    @param numfunct - the number of functions
    @param fin - the file input stream
*/

std::vector<std::string> ReadVarGroups(int numvargroup, std::ifstream & fin);

std::vector<std::string> ReadConstants(std::ifstream & fin);

std::vector<std::string> ReadConstantStrings(int numconsts, 
					     std::ifstream & fin);

/** Read in the parameters from the given input file stream
    @param numvargroup - the number of variables
    @param fin - the file input stream
*/



std::vector<std::string> ReadParameters(int numparam, std::ifstream & fin);



void WriteMeshToMonteCarlo(int level, std::vector<std::vector<std::pair<double,double> > > Values, std::ofstream fout,std::string cline);




void MakeConfig(std::ifstream & fin, std::ofstream & fout);
void MakeConfig(std::string config, std::stringstream & inputstringstream);


void WriteStep1(std::string filename, 
		std::string configfilename, 
		std::vector<std::string> & FunctVector, 
		std::vector<std::string> & ParamStrings, 
		std::vector<std::string> & VarGroupVector,
		std::vector<std::string> & Consts,
		std::vector<std::string> & ConstantStrings,
		std::vector<std::pair<double, double> > & RandomValues);


void MakeFunctions(std::ofstream & fout, 
		   std::vector<std::string> FunctVector);
void MakeFunctions(std::stringstream & inputstringstream, 
				   std::vector<std::string> FunctVector);


void MakeVariableGroups(std::ofstream & fout, 
			std::vector<std::string> VarGroupVector);


void MakeDeclareConstants(std::ofstream & fout, 
			  std::vector<std::string> ParamStrings, 
			  std::vector<std::string> Consts);


void MakeDeclareFunctions(std::ofstream & fout, int size);
void MakeDeclareFunctions(std::stringstream & inputstringstream, int size);

void MakeConstants(std::ofstream & fout, 
		   std::vector<std::string> ParamStrings, 
		   std::vector<std::string> ConstantStrings,
		   std::vector<std::pair<double,double> > RandomValues);


void mkdirunix(std::string mydir);

void mkdirstep1(std::string inputfilename);

void CallBertiniStep1(std::string base_dir);

void CallBertiniStep1Parallel(std::string base_dir,std::string machinefile, int numprocs);

/** Store what the parameters are as strings in a vector 
    @param ToParse -  the string, space delimited, that has the
    appropriate mesh information.
*/

std::vector< std::string > MakeParameterStrings(
	     				std::vector<std::string> ToParse);

std::vector< std::vector< std::pair<double,double> > > MakeValues(
					std::vector<std::string> ToParse, std::vector< int > & NumMeshPoints);


std::vector< std::vector< std::pair<double,double>  > > MakeValues(
				       int numparam, std::ifstream & fin);

void WriteStep1(std::string filename, 
		std::string configfilename, 
		std::vector<std::string> & FunctVector, 
		std::vector<std::string> & ParamStrings, 
		std::vector<std::string> & VarGroupVector,
		std::vector<std::string> & Consts,
		std::vector<std::string> & ConstantStrings,
		std::vector<std::pair<double, double> > & RandomValues);

// assuming we open the input file stream and the output filestream
//duplicate void MakeConfig(std::ifstream & fin, std::ofstream & fout);

void WriteMeshToMonteCarlo(int level, 
	       std::vector<std::vector<std::pair<double, double> > > Values, 
			   std::string dirfilename, 
			   std::string cline);

#endif

