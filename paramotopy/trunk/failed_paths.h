
#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <math.h>
#include "random.h"
#include "mtrand.h"
#include "step1_funcs.h"
#include "step2_funcs.h"
#include "para_aux_funcs.h"
#include "xml_preferences.h"
#include "runinfo.h"
#include "menu_cases.h"
#include "step2readandwrite.h"

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
	void copy_step_one(std::string from_dir, std::string to_dir, int iteration);
	
	void report_failed_paths(runinfo paramotopy_info);
	
};



// scans the data files for the failed paths




#endif


