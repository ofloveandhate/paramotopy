#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <stdio.h>

/** Function to write the shell script that calls bertini in the step 1 process */



void WriteShell1(int architecture,int usemachine);

void WriteShell1Parallel(int architecture, int usemachine);

//void WriteShell2();

void WriteShell3(int architecture);


void ParseData(int & numfunct, 
	       int & numvar, int & numparam, int & numconsts,
	       std::vector<std::string> & FunctVector,
	       std::vector<std::string> & VarGroupVector, 
	       std::vector<std::string> & ParamVector, 
	       std::vector< std::string > & ParamStrings,
	       std::vector<std::string>  & Consts,
	       std::vector<std::string> & ConstantStrings,
	       std::vector< std::vector< std::pair<float,float> > > & Values,  
	       std::vector< std::pair<float,float> > & RandomValues,
	       bool & userdefined,
	       std::ifstream & fin, std::vector< int > & NumMeshPoints,
	       std::string filename);


bool DeterminePreferences(int & architecture, int & usemachine, std::string & machinefile,int & numprocs,bool rerun, int & numfilesatatime, std::vector< bool > & FilePrefVector, int & saveprogresseverysomany);
/*
 read the user's preferences for parallelism on the machine.
	@param rerun - a bool to flag redetermining the preferences
*/


void SetPrefVersion(int version);

int GetPrefVersion();


/**
   Read the user-given values found in the configuration file and set
   the parameters to the appropriate values
   @param numfunct - number of functions
   @param numvar - number of variable groups
   @param numparam - number of parameters
   @param fin - the file input stream
*/


void ReadSizes(int & numfunct, int & numvar, int & numparam, int & numconsts,
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

std::vector<std::string> ReadVarGroups(int numvar, std::ifstream & fin);

std::vector<std::string> ReadConstants(std::ifstream & fin);

std::vector<std::string> ReadConstantStrings(int numconsts, 
					     std::ifstream & fin);

/** Read in the parameters from the given input file stream
    @param numvar - the number of variables
    @param fin - the file input stream
*/



std::vector<std::string> ReadParameters(int numparam, std::ifstream & fin);



void WriteMeshToMonteCarlo(int level, std::vector<std::vector<std::pair<float,float> > > Values, std::ofstream fout,std::string cline);




void MakeConfig(std::ifstream & fin, std::ofstream & fout);

//polymorphism kicks ass
void MakeConfig(std::vector< std::string > configvector, std::ofstream & fout);


void WriteStep1(std::string filename, 
		std::string configfilename, 
		std::vector<std::string> & FunctVector, 
		std::vector<std::string> & ParamStrings, 
		std::vector<std::string> & VarGroupVector,
		std::vector<std::string> & Consts,
		std::vector<std::string> & ConstantStrings,
		std::vector<std::pair<float, float> > & RandomValues);


void MakeFunctions(std::ofstream & fout, 
		   std::vector<std::string> FunctVector);


void MakeVariableGroups(std::ofstream & fout, 
			std::vector<std::string> VarGroupVector);


void MakeDeclareConstants(std::ofstream & fout, 
			  std::vector<std::string> ParamStrings, 
			  std::vector<std::string> Consts);


void MakeDeclareFunctions(std::ofstream & fout, int size);


void MakeConstants(std::ofstream & fout, 
		   std::vector<std::string> ParamStrings, 
		   std::vector<std::string> ConstantStrings,
		   std::vector<std::pair<float,float> > RandomValues);


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

std::vector< std::vector< std::pair<float,float> > > MakeValues(
					std::vector<std::string> ToParse, std::vector< int > & NumMeshPoints);


std::vector< std::vector< std::pair<float,float>  > > MakeValues(
				       int numparam, std::ifstream & fin);

void WriteStep1(std::string filename, 
		std::string configfilename, 
		std::vector<std::string> & FunctVector, 
		std::vector<std::string> & ParamStrings, 
		std::vector<std::string> & VarGroupVector,
		std::vector<std::string> & Consts,
		std::vector<std::string> & ConstantStrings,
		std::vector<std::pair<float, float> > & RandomValues);

// assuming we open the input file stream and the output filestream
void MakeConfig(std::ifstream & fin, std::ofstream & fout);

void WriteMeshToMonteCarlo(int level, 
	       std::vector<std::vector<std::pair<float, float> > > Values, 
			   std::string dirfilename, 
			   std::string cline);
