//contains the runinfo class
#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h> 

#include "random.hpp"
#include "para_aux_funcs.hpp"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/progress.hpp>
#include <boost/regex.hpp>

#include "tinyxml.h"
#include "xml_preferences.hpp"

#include "datagatherer.hpp"

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
	std::string fundamental_dir;
	std::string base_dir;
	

	int run_number;
	time_t initiated;
	time_t updated;
	
	std::string paramotopy_file;
	std::string inputfilename;
	std::string prefix;
	
	int steptwomode;// indicates to step2 whether to use step2 or pathfailure bertini settings.
	
	std::vector<std::string> Functions;
	std::vector<std::string> VarGroups; 
	std::vector<std::string> Parameters; 
	std::vector<std::string> ParameterNames;
	std::vector<std::string> Constants;
	std::vector<std::string> ConstantNames;
	std::string CustomLines;
	
	
	std::vector< std::vector< std::pair<double,double> > > Values;
	
	bool userdefined;
	
	std::string mcfname;  
	std::vector< int > NumMeshPoints;
	
	// random initial values
	// -- vector size is the number of parameters
	// -- pair is the real (first) and imaginary (second)
	std::vector< std::pair<double,double> > RandomValues;
	
	runinfo(){
		steptwomode = -1;
	};//default constructor
	

	std::string WriteInputStepOne(ProgSettings paramotopy_settings);
	std::string WriteInputStepTwo(std::vector<std::pair<double, double> > tmprandomvalues);
	
	void GetOriginalParamotopy();
	void GetInputFileName();
	void GetInputFileName(std::string suppliedfilename);
	
	void make_base_dir_name();
	
	
	void mkdirstep1(){
		std::string tempstr = base_dir;
		mkdirunix(tempstr);
		tempstr.append("/step1");
		mkdirunix(tempstr);
		};
	
	void clear(){
		Functions.clear(); VarGroups.clear(); 
		Parameters.clear(); ParameterNames.clear(); 
		Constants.clear(); ConstantNames.clear(); 
		Values.clear(); NumMeshPoints.clear();
		CustomLines.clear();
		RandomValues.clear(); 
		numfunct = 0; numvariables = 0; numvargroup = 0; numparam = 0; numconsts = 0; 
		};
	
	void ParseData();
	void ParseData(std::string dir);
	void ParseDataGuts(std::ifstream & fin);
	
	void ReadCustomLines(std::ifstream & fin);
	
	bool made_new_folder;
	void DataManagementMainMenu();
	void RandomMenu(){
		std::stringstream ss;
		ss << "1) save values\n"
			<< "2) load random values\n"
			<< "3) make new values\n"
			<< "*\n0)return to paramotopy\n: ";
		int choice = get_int_choice(ss.str(),0,3);
		switch (choice) {
			case 0:
					
				break;
			case 1:
				runinfo::SaveRandom();
				break;
			case 2:
				runinfo::LoadRandom();
				break;
			case 3:
				runinfo::SetRandom();
				break;
			default:
				break;
		}
	};
	void SaveRandom();
	void LoadRandom();
	void SetRandom();
	
	bool GetPrevRandom();
	
	void SetLocation();
	bool test_if_finished();
	void DisplayAllValues();
	
	void CopyUserDefinedFile();
	
	std::vector<std::pair<double, double> > MakeRandomValues(int garbageint);
	
	void WriteOriginalParamotopy(std::string dir);
	void WriteModifiedParamotopy(std::string dir, int iteration);
	void WriteRandomValues();
	
	
//	ManageData
	void SetBaseDirZero();
	void SetBaseDirManual(std::vector< boost::filesystem::path >);
	void SetBaseDirMostRecent(std::vector< boost::filesystem::path >);
	void SetBaseDirNew(std::vector< boost::filesystem::path >);
	void AutoScanData(const int preferred_behaviour);
	void ScanData();
	void save();
	void load(std::string filename);
	void get_run_xml(std::string filename, int & run, time_t  & wheninitiated, time_t & whenupdated);
	void UpdateAndSave();
	
	void MakeRandomValues();
	
	bool CheckRunStepOne();
	
	
	void GatherData();
	
	boost::filesystem::path GetAvailableRuns();
	
	void CollectSpecificFiles(std::string file_to_gather, std::vector < std::string > folders_with_data,std::string run_to_analyze, int parser_index, bool mergefailed);
	std::vector< std::string > GetFoldersForData(std::string dir);
	void IncrementOutputFolder(std::string & output_folder_name, std::string base_output_folder_name, int & output_folder_index);
	void MergeFolders(std::string file_to_gather, std::string left_folder, std::string right_folder, std::string output_folder_name, int parser_index);
	void finalize_run_to_file(std::string file_to_gather, std::string source_folder,std::string base_output_folder_name, int parser_index, bool mergefailed);
	
	
	void rest_of_files(std::ifstream & datafile, std::string & output_buffer, std::ofstream & outputfile, std::vector < std::string > filelist, int file_index, int parser_index);
	bool endoffile_stuff(std::ifstream & datafile, int & file_index, std::vector < std::string > filelist);
	
	///function for parsing ***_solutions files, as output from bertini.

	bool ReadPoint(std::ifstream & fin, int & next_index, std::string & data, int parser_index);
	std::string ParseSolutionsFile(std::ifstream & fin);
	std::vector< std::vector < std::pair< std::string, std::string > > > ParseSolutionsFile_ActualSolutions(std::ifstream & fin);
	
	/// function for parsing the failed_paths file (just one instance of it)
	//gets called repeatedly to see if any points had failed paths (each point has one failed_paths file.
	std::string ParseFailedPaths(std::ifstream & fin);

private:
	void GetNumVariables();
	void ReadSizes(std::ifstream & fin);
	void ReadParameters(std::ifstream & fin);
	void ReadFunctions(std::ifstream & fin);
	void ReadVarGroups(std::ifstream & fin);
	void ReadConstants(std::ifstream & fin);
	void ReadConstantStrings(std::ifstream & fin);
	
	void MakeParameterNames();
	
  void MakeVariableGroups(std::stringstream & fout, ProgSettings paramotopy_settings);
	
	void MakeDeclareConstants(std::stringstream & fout);
	void MakeConstants(std::stringstream & fout);
	void MakeConstantsStep2(std::vector<std::pair<double, double> > CValues, std::stringstream & inputfilestream);
	
	void MakeCustomLines(std::stringstream & inputfilestream);
	
	void MakeDeclareFunctions(std::stringstream & inputstringstream);
	void MakeFunctions(std::stringstream & inputstringstream);
	

	
	void MakeRandomValues(std::vector< std::pair< std::pair< double, double >, 
						  std::pair< double, double > > > RandomRanges);
	
	
	void MakeValues(std::ifstream & fin);
	void MakeValues();
	void PrintRandom();
	
	
	

	void SetDataFirst();
	void SetDataMostRecent();
	void SetDataInteractive();
	void WriteDataDateStarted();
	void DeleteRunFolder();


	
protected:

	
};



#endif


