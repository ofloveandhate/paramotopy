
#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <iterator>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <iomanip>
#include <math.h>
#include "random.hpp"
#include "mtrand.hpp"
#include "step1_funcs.hpp"
#include "step2_funcs.hpp"
#include "para_aux_funcs.hpp"
#include "xml_preferences.hpp"
#include "runinfo.hpp"
#include "menu_cases.hpp"
#include "step2readandwrite.hpp"


#include "point.hpp"

#include "datagatherer.hpp"


#ifndef __FAILED_PATHS_H__
#define __FAILED_PATHS_H__


//the case to be called from paramotopy
int failedpaths_case(runinfo & paramotopy_info, 
					 ProgSettings & paramotopy_settings,
					 int & iteration);





class failinfo {
public:

	//VARIABLES
	int num_points_inspected;

	std::vector< point > master_fails;
	std::map< int, std::vector < point > > initial_fails;
	std::map< int, std::vector < point > > terminal_fails;
	
	int totalfails;
	std::vector< std::string > foldervector;
	int current_iteration;
	
	//FUNCTIONS
	
	void MainMenu(ProgSettings & paramotopy_settings, runinfo & paramotopy_info);

	
private:
	
	
	void StartOver(runinfo & paramotopy_info);
	void RecoverProgress(runinfo & paramotopy_info);
	void PerformAnalysis(ProgSettings & paramotopy_settings, runinfo & paramotopy_info);
	
	void set_location_fail(runinfo & paramotopy_info);
	
	void report_restored_data();
	int find_failed_paths(runinfo paramotopy_info, int temporal_flag, int iteration);
	void get_folders_for_fail(std::string dir);
	std::string new_step_one(ProgSettings & paramotopy_settings,runinfo & paramotopy_info);
	void make_master_from_recovered(std::map< int, point > successful_resolves);
	void write_failed_paths(runinfo paramotopy_info, int iteration);
	void find_successful_resolves(runinfo & paramotopy_info);
	void write_successful_resolves(runinfo paramotopy_info);
	void merge_successful_resolves(std::vector< point > current_successful_resolves, std::vector< int > relative_indices, std::vector< int > final_indices);
	
	void copy_step_one(std::string from_dir, std::string to_dir, int iteration, std::string startfilename);
	
	void report_failed_paths(runinfo paramotopy_info);
	
	
};



// scans the data files for the failed paths




#endif


