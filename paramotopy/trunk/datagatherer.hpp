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
#include "point.hpp"

#ifndef __DATAGATHERER_H__
#define __DATAGATHERER_H__




class datagatherer {
	
public:
	std::string fundamental_dir;
	std::string base_dir;
	
	int numvariables;
	
	boost::filesystem::path run_to_analyze;
	
	std::vector < std::string > gather_savefiles;
	std::vector < int > gather_parser_indices;
	
	
	//data members
	datagatherer(){
		fundamental_dir = "";
		numvariables = 0;
		base_dir = "";
	};
	datagatherer(std::string bdir, std::string fundir, int numvar){
		this->base_dir = bdir;
		this->fundamental_dir = fundir;
		this->numvariables = numvar;
		this->run_to_analyze = "";
		};
	datagatherer(int numvar, std::string loc){
		this->fundamental_dir = "";
		this->numvariables = numvar;
		this->run_to_analyze = loc;
	};
	
	
	//~datagatherer(){};
	
	
	
	//functions
	bool GatherDataForFails(std::vector< point > terminal_fails, std::vector< point > & successful_resolves);
	void GatherDataFromMenu();
	std::vector< point > CompareInitial_Gathered(std::vector< point > terminal_fails, std::vector< point > current_data);
	std::vector < point > GatherFinalizedDataToMemory(boost::filesystem::path folder_with_data);
	
	void WriteUnsuccessfulResolves(std::map< int, point> successful_resolves);
	std::map< int, point > ReadSuccessfulResolves();
	void CheckForResolvedFailedPoint(std::string file_to_gather,int next_index,std::string & next_data,std::map< int, point>successful_resolves);
	
	
	boost::filesystem::path GetAvailableRuns();
	
	void CollectSpecificFiles(std::string file_to_gather, std::vector < std::string > folders_with_data,std::string run_to_analyze, int parser_index, bool mergefailed);
	
	std::vector< std::string > GetFoldersForData(std::string dir);
	
	void IncrementOutputFolder(std::string & output_folder_name, std::string base_output_folder_name, int & output_folder_index);
	
	void MergeFolders(std::string file_to_gather, std::string left_folder, std::string right_folder, std::string output_folder_name, int parser_index);
	
	void finalize_run_to_file(std::string file_to_gather, std::string source_folder,std::string base_output_folder_name, int parser_index, bool mergefailed);
	
	
	void rest_of_files(std::ifstream & datafile, std::string & output_buffer, std::ofstream & outputfile, std::vector < std::string > filelist, int file_index, int parser_index);
	bool endoffile_stuff(std::ifstream & datafile, int & file_index, std::vector < std::string > filelist);
	
	///function for parsing ***_solutions files, as output from bertini.
	
	bool ReadPoint(std::ifstream & fin, std::string & data, int parser_index);
	bool ReadPoint(std::ifstream & fin, int & next_index, std::string & data, int parser_index);
	
	std::string ParseSolutionsFile(std::ifstream & fin);
	
	std::vector< std::vector < std::pair< std::string, std::string > > > ParseSolutionsFile_ActualSolutions(std::ifstream & fin);
	
	/// function for parsing the failed_paths file (just one instance of it)
	//gets called repeatedly to see if any points had failed paths (each point has one failed_paths file.
	std::string ParseFailedPaths(std::ifstream & fin);
	

	
	
private:
	
	
	
};






#endif




