
#include <vector>
#include <map>
#include <iostream>
#include <string>
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
	std::vector< int > index_vector;
	std::vector< std::vector< std::pair<double,double> > >  fail_vector;
	int totalfails;
	std::vector< std::string > foldervector;
	int current_iteration;
	
	//FUNCTIONS
	
	void MainMenu(ProgSettings & paramotopy_settings, runinfo & paramotopy_info);

	
private:
	
	
	void StartOver(runinfo & paramotopy_info);
	void RecoverProgress(runinfo & paramotopy_info);
	void PerformAnalysis(ProgSettings & paramotopy_settings, runinfo & paramotopy_info);
	
	
	int find_failed_paths(runinfo paramotopy_info);
	void get_folders_for_fail(std::string dir);
	std::string new_step_one(ProgSettings paramotopy_settings,runinfo paramotopy_info);
	
	void write_failed_paths(runinfo paramotopy_info, int iteration);
	void copy_step_one(std::string from_dir, std::string to_dir, int iteration, std::string startfilename);
	
	void report_failed_paths(runinfo paramotopy_info);
	
};



// scans the data files for the failed paths




#endif


