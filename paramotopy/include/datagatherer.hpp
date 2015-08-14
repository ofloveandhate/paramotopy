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

#include "runinfo.hpp"
#include "xml_preferences.hpp"
#include "timing.hpp"



/**
 *  @class "fileinfo"
 *
 * stores data related to a gathered file, such as the number of found solutions, the parser switch index, the file text and size, and the current number of the file.
 *
 * \brief Class for caching/storing gathered temp output files.
 **/


class fileinfo {
	
	
public:
	
	/** default constructor */
	fileinfo(){
		filesize = filecount = num_found_solns = 0;
		parser_index = -11;
		runningfile = "";
	}
	
	int filesize; ///< number of characters.
	int filecount; ///<  current index of file.  like 0 in real_solutions0
	int num_found_solns; ///<  the most recent number of found solutions.
	
	std::string runningfile;  ///<  file text
	
	int parser_index;  ///<  the parser switch index.
	
		
	
	
};


/**
 *  @class "datagatherer"
 *  
 * contains methods for gathering data from the bfiles_filename/run0/step2/DataCollected/c0 folders (or in another respective location).  
 * 
 * \brief Class for gathering data, from failed path analysis, and from completed runs.
 **/


class datagatherer {

	
public:
	
	
	
	/** default constructor */
	datagatherer(){
		fundamental_dir = "";
		numvariables = 0;
		base_dir = "";
	};
	
	/** constructor with instantiation of base_dir, fundamental_dir, and numvar.  intended for use in the collection of data for a complete run, including merging of failed path successes. */
	datagatherer(boost::filesystem::path bdir, boost::filesystem::path fundir, int numvar){
		this->base_dir = bdir;
		this->fundamental_dir = fundir;
		this->numvariables = numvar;
		this->run_to_analyze = "";
	};
	
	/** constructor with instantiation of numvariables, and run_to_analyze.  intended for use in failed path analysis */
	datagatherer(int numvar, boost::filesystem::path loc){
		this->fundamental_dir = "";
		this->numvariables = numvar;
		this->run_to_analyze = loc;
	};
	

	
		//data members
	
	
	/** fundamental directory -- bfiles_filename */
	boost::filesystem::path fundamental_dir;
	/** base directory, including the run number. -- bfiles_filename/run0 */
	boost::filesystem::path base_dir;
	/** number of variables in the problem */
	int numvariables;
	
	/** the run to collect data from */
	boost::filesystem::path run_to_analyze;
	
	/** names of files to gather */
	std::vector < std::string > gather_savefiles;
	/** index for parser choice.  different filetypes use different parsers */
	std::vector < int > gather_parser_indices;
	

	std::vector<std::string> ParamNames;
	int numfiles;
	boost::filesystem::path DataCollectedbase_dir;
	int buffersize;
	int newfilethreshold;
	int myid;
	std::string real_filename;
	std::map< std::string, fileinfo> slavemap;  // a map between file names and fileinfo (as defined above)
	
	
	int num_reals(){
			return this->slavemap[this->real_filename].num_found_solns;
	}
	
	void slave_init(){
		ParamNames.clear();
		slavemap.clear();
		this->numfiles = 0;
		this->buffersize = this->newfilethreshold = -1;
		this->myid = -1;
		this->DataCollectedbase_dir = "";
	}
	
	
	
	
	
	void add_file_to_save(std::string filename){
	
		fileinfo blankinfo;

		this->slavemap[filename] = blankinfo;
		this->numfiles ++;
		
		this->slavemap[filename].parser_index = GetFileParserIndex(filename);
		
		
	}
	
	boost::filesystem::path MakeTargetFilename(std::string filename);
	
	
	
	
	void SlaveSetup(ProgSettings & paramotopy_settings,
									runinfo & paramotopy_info,
									int myid,
									boost::filesystem::path called_dir);
	
	
	
	/**
	 * reads data from bertini output files, and writes it to files in DataCollected folder, if the threshold is big enough
	 * \param linenumber Parameter point index
	 * \param AllParams Current parameter values.
	 * \param paramotopy_settings the settings structure, passed by reference
	 * \param process_timer Timer object for collecting timing data.
	 */
	bool SlaveCollectAndWriteData(double *current_params,
																ProgSettings & paramotopy_settings,
																timer & process_timer);
	
	
	/**
	 * sorts the input file for positive real solutions
	 * creates a string to append read-in data to the running file.
	 * \param source_filename Name of the data file to be read.
	 * \param current_parameter_values Current parameter values.
	 * \param paramotopy_settings the settings structure, passed by reference
	 */
	void AppendOnlyPosReal(std::string source_filename,
												 double *current_params,
												 ProgSettings & paramotopy_settings);
	
	
	
	
	/**
	 * creates a string to append read-in data to the running file.
	 * \param source_filename Name of the data file to be read.
	 * \param current_parameter_values Current parameter values.
	 */
	void AppendData(std::string source_filename,
									double *current_params);
	
	
	
	
	
	void WriteAllData();
	
	
	
	/**
	 * Write data to disk.
	 * \param outstring The file to write
	 * \param target_file name of the file to write to.
	 */
	void WriteData(std::string outstring,
								 boost::filesystem::path target_file);
	

	
	
	
	
	
	
	
	
	
	
	
	/** outermost method for gathering data during failure analysis mode.
	 * keeps track of which points were successful (or not).
	 * \param terminal_fails vector containing the indices of the attempted resolves.
	 * \param successful_resolves Vector of points for holding the successful data.
	 */
	bool GatherDataForFails(std::vector< point > terminal_fails,
													std::vector< point > & successful_resolves);
	
	/** outermost method for gathering data from main menu. no input necessary*/
	void GatherDataFromMenu();
	
	/** 
	 * Compares initially run data to re-solved during failed path analysis.  Reads in data from the successful_resolves file.
	 * \return vector of successfully resolved points, complete with data
	 */
	std::vector< point > CompareInitial_Gathered(std::vector< point > terminal_fails,
																							 std::vector< point > current_data);
	
	/** gather the data from a completed, gathered, finalized step2, to memory.
	 * \param folder_with_data Path having data in it.
	 */
	std::vector < point > GatherFinalizedDataToMemory(boost::filesystem::path folder_with_data);
	
	/**
	 * write persistent fails to disk.
	 * \param successful_resolves Map of points and collected data.  If has no data, then persistently failed through failed path analysis.
	 */
	void WriteUnsuccessfulResolves(std::map< int, point> successful_resolves);
	
	/** Reads data produced by failed path analysis into memory. 
	 * \return A map of points, complete with collected data.
	 */
	std::map< int, point > ReadSuccessfulResolves();
	
	/** 
	 * \param file_to_gather The name of the file we are sorting and merging.
	 * \param next_index The index.  Used in lookup in the map successful_resolves.
	 * \param next_data A mutable string containing the data.  If successful_resolves has a point with the same index, and it was successfully resolved, we replace the next_data with that in the map.
	 * \param successful_resolves Map of points containing the data produced by failed path analysis
	 */
	void CheckForResolvedFailedPoint(std::string file_to_gather,
																	 int next_index,
																	 std::string & next_data,
																	 std::map< int, point> successful_resolves);
	
	/** 
	 * search for completed runs with this filename.
	 * \return path of choice for run to collect.  this is automatically determined if there is exactly one completed run with the filename. 
	 */
	boost::filesystem::path GetAvailableRuns();
	
	/** Main loop for gathering a particular file in a particular completed run.
	 * \param file_to_gather The name of the bertini output file to gather.
	 * \param folders_with_data Vector with the names of the folders containing the data.
	 * \param run_to_analyze The name of the run.
	 * \param parser_index The index corresponding to the filename.  Different file types have different parsers.
	 * \param mergefailed Bool determining whether to merge data from failed path analysis.  This is false if collecting from a run DURING failed path analysis (it would break things), but is true if doing a menu collection.
	 */
	void CollectSpecificFiles(std::string file_to_gather,
														std::vector < boost::filesystem::path > folders_with_data,
														boost::filesystem::path run_to_analyze,
														int parser_index,
														bool mergefailed);
	
	/** Read the folders containing the data.  The folder names are contained in a plain text file.
	 * \param dir String containing the name of the directory in which to look.
	 */
	std::vector< boost::filesystem::path > GetFoldersForData(boost::filesystem::path dir);
	
	/** A lower-level function.  Increases the output folder index by 1, during a merge.
	 * \param output_folder_name MUTABLE string to hold the new folder name.
	 * \param base_output_folder_name The string from which we build output_folder_name
	 * \param output_folder_index MUTABLE integer, which herein is incremented, and contatenated to base_... to make the output_folder_name.
	 */
	void IncrementOutputFolder(boost::filesystem::path & output_folder_name,
														 boost::filesystem::path base_output_folder_name,
														 int & output_folder_index);
	
	/** A lower-level function.  Actually merges two folders, left and right, into the output_folder_name, using the parser determined by parser_index.
	 * \param file_to_gather The name of the bertini output file we are gathering.
	 * \param left_folder The name of the left folder currently being merged.
	 * \param output_folder_name The name of the folder into which to place the merged data.
	 * \param parser_index The index of the parser to use on this filetype.
	 */
	void MergeFolders(std::string file_to_gather,
										boost::filesystem::path left_folder,
										boost::filesystem::path right_folder,
										boost::filesystem::path output_folder_name,
										int parser_index);
	
	/**
	 * Write the total collected data to a single file, and verify that the data is in order.
	 * \param file_to_gather The name of the Bertini output file we are gathering currently.
	 * \param source_folder The name of the source folder.
	 * \param base_output_folder_name.  The name of the bfiles_filename/run#/gathered_data.  We append with /finalized.
	 * \param parser_index The parser choice to use.  Corresponds to file_to_gather.
	 * \param mergefailed Boolean indicating whether the data checks out.
	 */
	void finalize_run_to_file(std::string file_to_gather, boost::filesystem::path source_folder,
														boost::filesystem::path base_output_folder_name, int parser_index, bool mergefailed);
	
	/**
	 * While merging, we will get to a point when there is no more sorting to do.  The rest of the data is in set of files, and in order, so all that remains is to copy the rest of the file in to the output file.  This function does that.
	 * \param datafile The stream for reading the data.
	 * \param output_buffer The string we read into.
	 * \param outputfile The output stream we write to.
	 * \param filelist Vector containing the names of the files to be read.
	 * \param file_index Index integer of the file currently being read.
	 * \param parser_index The integer index of the parser being used.
	 */
	void rest_of_files(std::ifstream & datafile, std::string & output_buffer, std::ofstream & outputfile,
										 std::vector < boost::filesystem::path > filelist, int file_index, int parser_index);
	
	/**
	 *  Reads the remainder of a file into memory, after a point at which we are certain there is not more sorting to be done.
	 * \param datafile The data file being read.
	 * \param file_index The integer index of the file being read.
	 * \param filelist The vector containing the list of the files to read.
	 */
	bool endoffile_stuff(std::ifstream & datafile, int & file_index, std::vector < boost::filesystem::path > filelist);
	
	
	/** function for parsing ***_solutions files, as output from bertini.
	 * \param fin The file handle.
	 * \param data The string to which to read the data.
	 * \param parser_index The parser to use.
	 */
	bool ReadPoint(std::ifstream & fin, std::string & data, int parser_index);
	/** function for parsing ***_solutions files, as output from bertini.
	 * \param fin The file handle.
	 * \param next_index The index of the point just read.
	 * \param data The string to which to read the data.
	 * \param parser_index The parser to use.
	 */
	
	bool ReadPoint(std::ifstream & fin, int & next_index, std::string & data, int parser_index);
	
	/** function for parsing ***_solutions files, as output from bertini.
	 * \param fin The file handle.
	 * \return A string with the data
	 */
	std::string ParseSolutionsFile(std::ifstream & fin);
	
	/**
	 * Function for getting the solutions and parameter values, for sorting
	 * \param fin
	 * \return vector of data
	 */
	std::vector< std::vector < std::pair< std::string, std::string > > > ParseSolutionsFile_ActualSolutions(std::ifstream & fin);
	
	/** function for parsing the failed_paths file (just one instance of it)
	 * gets called repeatedly to see if any points had failed paths (each point has one failed_paths file)
	 * \param fin The handle to the file.
	 * \return The data in string form.
	 */
	std::string ParseFailedPaths(std::ifstream & fin);
	

	
	
private:
	
	
	
};


/** Display the data management main menu.  */
void DataManagementMainMenu(runinfo & paramotopy_info);



#endif




