
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



#ifndef __FAILED_PATHS_H__
#define __FAILED_PATHS_H__

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


/** the case to be called from paramotopy menu
 *
 */
int failedpaths_case(runinfo & paramotopy_info, 
		     ProgSettings & paramotopy_settings,
		     int & iteration);




/** A class for the failed path analysis.  Instantiated in failedpath_case() called from the main menu.
 * \brief The class for performing failed path analysis.
 * \class failinfo
 */
class failinfo {
public:

  //VARIABLES
  /** The number of points to test if there was a failed path. */
  int num_points_inspected;
  /** The parameter points that had failed paths. */
  std::vector< point > master_fails;
  /** The parameter points that had initial files. */
  std::map< int, std::vector < point > > initial_fails;
  /** The parameter points that keep on failing. */
  std::map< int, std::vector < point > > terminal_fails;
  
  int totalfails;
  std::vector< std::string > foldervector;
  int current_iteration;
  
  //FUNCTIONS
  /** Display the main menu of the failed paths analysis. 
      \param paramotopy_settings - The current program preference settings.
      \param paramotopy_info - The current run information.
  */
  void MainMenu(ProgSettings & paramotopy_settings, runinfo & paramotopy_info);
  
  
private:
  
  /** Clear Data and rerun failed paths.
      \param paramotopy_info - The current run information. 
  */
  void StartOver(runinfo & paramotopy_info);
  /** Get the iteration number, recovers a list of points which have path failures, and reports the paths to the user. 
      \param paramotopy_info The current run information. 
  */
  void RecoverProgress(runinfo & paramotopy_info);
  /** Run an analysis on the failed paths.
      \param paramotopy_settings - The paramotopy preference settings.
      \param paramotopy_info - The current run info.
  */
  void PerformAnalysis(ProgSettings & paramotopy_settings, runinfo & paramotopy_info);
  /** set the directory of the failed paths and the current pass. 
   \param paramotopy_info - The current run information. */
  void set_location_fail(runinfo & paramotopy_info);
  /** 
      Report to the user the initial fails and the terminal fails. 
   */
  void report_restored_data();
  /** 
      Parses a series of failed_paths files, searching for points which had such paths.
      \param paramotopy_info - The current run info.
      \param temporal_flag - 
   */
  int find_failed_paths(runinfo paramotopy_info, int temporal_flag, int iteration);
  /** 
      Set the vector of failed folders.
      \param dir - The directory where the failed paths could exist in some subdirectory.
   */
  void get_folders_for_fail(boost::filesystem::path dir);

  /** 
      Write a new step1 file with different random points to run to the failed paths in the step 2 process.
      \param paramotopy_settings - The paramotopy user defined settings.
      \param paramotopy_info - The current run info.
   */
  std::string new_step_one(ProgSettings & paramotopy_settings,runinfo & paramotopy_info);
  /** Not quite sure what this function does . . . 
   Print the successful resolves and sort the fails and save them to the master_fails
  \param successful_resolves - a map of the successful resolves. The int is the parameter number and the point is the parameter point.
  */
  void make_master_from_recovered(std::map< int, point > successful_resolves);

  /**
     Write the failed paths
     \param paramotopy_info - The current run information. 
     \param iteration - The current iteration for path failure resolution.
  */
  void write_failed_paths(runinfo paramotopy_info, int iteration);
  void find_successful_resolves(runinfo & paramotopy_info);
  void write_successful_resolves(runinfo paramotopy_info);
  /** 
      Merge the successful of resolved failed paths with the rest of the parameter points that have been successful.
      \param current_successful_resolves - The current parameter points that had failed paths and are now resolved.
      \param relative_indices - The relative indices (corresponding to the parameter point) of the points in current_successful_resolves. 
      \param final_indices - The final indices of the points in current_successful_resolves.
  */
  void merge_successful_resolves(std::vector< point > current_successful_resolves, std::vector< int > relative_indices, std::vector< int > final_indices);
  
  /** 
      Copy the step 1 file.
      \param from_dir - The directory from which to copy the step1 file.
      \param to_dir - The directory to copy the step1 file.
      \param iteration - The current iteration number in the attempt to resolve path failures.
      \param startfilename - The file to copy.
   */
  void copy_step_one(boost::filesystem::path from_dir, boost::filesystem::path to_dir, int iteration, std::string startfilename);
  /** 
      Report the failed paths to the user.
      \param paramotopy_info - The current paramotopy run info.
   */
  void report_failed_paths(runinfo paramotopy_info);
  
  
};



// scans the data files for the failed paths




#endif


