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


#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/progress.hpp>
#include <boost/regex.hpp>

#include "tinyxml.h"


#ifndef __RUNINFO_H__
#define __RUNINFO_H__


#include "random.hpp"
#include "para_aux_funcs.hpp"

#include "xml_preferences.hpp"


/** The input file parser. 
 * \class runinfo
 * \brief A class for the input file parser.
 */

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

  /** The number of functions. */
  int numfunct;
  /** The number of variables. */
  int numvariables;
  /** The number of variable groups. */
  int numvargroup;
  
  int numparam; /** The number of parameters. */
	
  /** The number of constants. */
  int numconsts;
	
	//this may well be changed/ renamed/ deleted
  /**  The current working directory ? as we move in the directory structure. */
  boost::filesystem::path location;
	
	/** bfiles_filename */
  boost::filesystem::path fundamental_dir;
	
  /** The bfiles_filename base directory. */
  boost::filesystem::path base_dir;
  
  /** The run folder. */
  int run_number;
  /** The start time. */
  time_t initiated;
  /** Any time during the run program.  To compare against initiated. */
  time_t updated;
  
  /** A string that stores the entire paramotopy input file in memory.  */
	std::string paramotopy_file;
	
  /** The paramotopy input file name. */
	boost::filesystem::path inputfilename;
  /** The directory prefix.  */
  boost::filesystem::path prefix;
  
  /** Indicates during the step 2 runs to do step2 or pathfailure bertini settings. */
  int steptwomode;// indicates to step2 whether to use step2 or pathfailure bertini settings.
  
  /** The functions.  */
  std::vector<std::string> Functions;
  /** The variable groups. */
  std::vector<std::string> VarGroups;
  /** The parameters. */
  std::vector<std::string> Parameters; 
  /** The parameter names. */
  std::vector<std::string> ParameterNames;
  /** The constants. */
  std::vector<std::string> Constants;
  /** The constant names. */
  std::vector<std::string> ConstantNames;
  /** The custom lines. */
  std::string CustomLines;
  
  /** The parameter values. */
  std::vector< std::vector< std::pair<double,double> > > Values;
  
	std::vector< std::pair<double,double> > BoundsLeft;
	std::vector< std::pair<double,double> > BoundsRight;
	
  /** Flag for user-defined monte carlo file. True if yes, false if no. */
  bool userdefined;
  
  /** The monte carlo file name. */
  std::string mcfname;  
  /** If a user-defined mesh, then this stores the number of mesh points in each dimension in regards to the parameter. */
  std::vector< int > NumMeshPoints;
  
  // random initial values
  // -- vector size is the number of parameters
  // -- pair is the real (first) and imaginary (second)
  /** Random initial values.  In the pair, real is first, imaginary is second. */
  std::vector< std::pair<double,double> > RandomValues;
  
	
	
	
	
	
	
	
	
	////////////
	//////////
	////////
	///////
	//////
	//            methods
	//////
	///////
	/////////
	///////////
	/////////////
	
  /** The default paramotopy constructor. */
  runinfo(){
    steptwomode = -1;
  };//default constructor
  
  /** Write the bertini step 1 input file.
      \param paramotopy_settings - The program settings to be used in the input file. */
  std::string WriteInputStepOne(ProgSettings paramotopy_settings);
  /** Write the bertini step 2 input file.
      \param tmprandomvalues - The starting (?) random values of the input file.
      \param paramotopy_settings - The program settings to be used in the input file. 
      \param failstep2 -- Flag to indicate if writing an input file to conduct failed path analysis
   */
  std::string WriteInputStepTwo(std::vector<std::pair<double, double> > tmprandomvalues,
                                ProgSettings paramotopy_settings, bool failstep2);
  
  /** Get the original paramotopy input file. */
  void GetOriginalParamotopy();
  /** Get the name of the paramotopy input file. */
  void GetInputFileName();
  /** Get the name of the paramotopy input file with the supplied filename.
      \param suppliedfilename - The supplied filename.
   */
  void GetInputFileName(boost::filesystem::path suppliedfilename);
  

  /** Make the base directory name.*/
  void make_base_dir_name();
  
  /** Make the step 1 directory. */
  void mkdirstep1(){
		boost::filesystem::path tempstr = base_dir;
    boost::filesystem::create_directories(tempstr);
    tempstr /= "step1";
    boost::filesystem::create_directories(tempstr);
  };
  
  /** Clear all of the vectors that hold information for the current paramotopy run. */
  void clear(){
    Functions.clear(); VarGroups.clear(); 
    Parameters.clear(); ParameterNames.clear(); 
    Constants.clear(); ConstantNames.clear(); 
    Values.clear(); NumMeshPoints.clear();
    CustomLines.clear();
    RandomValues.clear(); 
    numfunct = 0; numvariables = 0; numvargroup = 0; numparam = 0; numconsts = 0; 
  };
  
  /** Parse the paramotopy input file that is stored in memory. */
  void ParseData();
  /** Parse data during the step2 run.
   \param dir - the base directory in comparison to the directory paramotopy was initially called.  */
  void ParseData(boost::filesystem::path dir);
  
  /** Parse the paramotopy input file and store values into memory.
      \param fin - The input filestream that contains the paramotopy input file.
   */
  void ParseDataGuts(std::ifstream & fin);
  
  /** 
      Read the custom lines in the paramotopy input file.
      \param fin - The input filestream of the paramotopy input file.
  */
  void ReadCustomLines(std::ifstream & fin);
  
  /** 
      Flag to indicate if a new folder was created or not.
   */
  bool made_new_folder;
  
  /** Display the random value menu. */
  void RandomMenu();
  /** Save the random values to a file. */
  void SaveRandom();
  /** Load the random values. */
  void LoadRandom();
  /** Set the random values. */
  void SetRandom();
  /** Get the previous random values. */
  bool GetPrevRandom();
  /** Set the location of what exactly ?  */
  void SetLocation();
  /** Test if the run is finished. */
  bool test_if_finished();
  /** Display the random values. */
  void DisplayAllValues();
  
  /** Copy the user defined file to the run folder. */
  void CopyUserDefinedFile();
  
  /** Make the random values.  */
  std::vector<std::pair<double, double> > MakeRandomValues(int garbageint);
  
	
	/**
	 * Reads in the random values to be used in this solve.
	 * \param base_dir Directory in which to look for the random values file.
	 * \param RandomValues Mutable vector in which to store the random values read in by this function.
	 */
	void GetRandomValues();

	
	
  /** Write the original paramotopy file in the bfiles_filename/run# folder. */
  void WriteOriginalParamotopy(boost::filesystem::path dir);
  /** Write a modified paramotopy file in the corresponding iteration folder used in the path failure analysis. */
  void WriteModifiedParamotopy(boost::filesystem::path dir, int iteration);
  /** Write the random values to a file. ? ? */
  void WriteRandomValues();
  
  
  //	ManageData
  /** Set the topmost base directory. */
  void SetBaseDirZero();
  /** Set the base directory to a new run folder manually. 
   \param found_runs - The directory that contains all the runs. */
  void SetBaseDirManual(std::vector< boost::filesystem::path > found_runs);
  /** Set the base directory of the most recent run. 
   \param found_runs - The directory that contains all the runs. */
  void SetBaseDirMostRecent(std::vector< boost::filesystem::path > found_runs);
  /** Set the base directory to a new run folder. 
   \param found_runs - The directory that contains all the runs. 
  */
  void SetBaseDirNew(std::vector< boost::filesystem::path > found_runs);
  /**  Scan what runs have been done. If none, create a a new folder. .
   \param preferred_behaviour - Determine what type of behaviour to use.
                                1 for most recent.
                                2 for new.
                                3 for manual.
  */
  void AutoScanData(const int preferred_behaviour);
  /** Scan if a run has been done. If not create a new run rolder. */
  void ScanData();
  /** Save the run info data to the xml file. */
  void save();
  /** Load the paramotopy input file.
   \param filename - The filename to load. 
  */
  void load(boost::filesystem::path filename);
  /**  Get the xml information of the current run.
   \param filename - The xml file name.
   \param run - The run number.
   \param wheninitiated - Time the run began.
   \param whenupdated - Time of the last update of the run. */
  void get_run_xml(boost::filesystem::path filename, int & run, time_t  & wheninitiated, time_t & whenupdated);
  /** Set the last updated time for the run and save to the xml file.  */
  void UpdateAndSave();
  
  /** Make the random values. */
  void MakeRandomValues();
  
  /** Check if step 1 has been run or not. */
  bool CheckRunStepOne();
  
  /** Not used in runinfo.cpp ? */
  void GatherData();
  

  /** Not used in runinfo.cpp ? */
  boost::filesystem::path GetAvailableRuns();
  
  /** Not used in runinfo.cpp ? */
  void CollectSpecificFiles(boost::filesystem::path file_to_gather,
														std::vector < boost::filesystem::path > folders_with_data,
														boost::filesystem::path run_to_analyze,
														int parser_index, bool mergefailed);
	
  /** Not used in runinfo.cpp ?  */
  std::vector< boost::filesystem::path > GetFoldersForData(boost::filesystem::path dir);
  /** Not used in runinfo.cpp ?  */
  void IncrementOutputFolder(boost::filesystem::path & output_folder_name,
														 boost::filesystem::path base_output_folder_name,
														 int & output_folder_index);
  /** Not used in runinfo.cpp  */
  void MergeFolders(std::string file_to_gather, std::string left_folder, 
		    std::string right_folder, std::string output_folder_name, int parser_index);
  /** Not used in runinfo.cpp */
  void finalize_run_to_file(boost::filesystem::path file_to_gather,
														boost::filesystem::path source_folder,
														boost::filesystem::path base_output_folder_name,
														int parser_index, bool mergefailed);
  
  /** Not used in runinfo.cpp */
  void rest_of_files(std::ifstream & datafile, std::string & output_buffer, 
		     std::ofstream & outputfile, std::vector < boost::filesystem::path > filelist,
										 int file_index, int parser_index);
  /** Not used in runinfo.cpp */
	
bool endoffile_stuff(std::ifstream & datafile, int & file_index, std::vector < boost::filesystem::path > filelist);
  
  ///function for parsing ***_solutions files, as output from bertini.
  /** Not used in runinfo.cpp */
  bool ReadPoint(std::ifstream & fin, int & next_index, std::string & data, int parser_index);
  
  /** Not used in runinfo.cpp */
  std::string ParseSolutionsFile(std::ifstream & fin);
  
  /** Not used in runinfo.cpp */
  std::vector< std::vector < std::pair< std::string, std::string > > > ParseSolutionsFile_ActualSolutions(std::ifstream & fin);
  

  /// function for parsing the failed_paths file (just one instance of it)
  //gets called repeatedly to see if any points had failed paths (each point has one failed_paths file.
  /** For parsing the failed_paths file.  This function gets called repeatedly to see if any point has failed paths (each point has one failed_paths file).
      \param fin - The inputfile stream of the failed_paths file. 
   */
  std::string ParseFailedPaths(std::ifstream & fin);
  /** Print the random values. */
  void PrintRandom();
private:
  /** Get the number of variables. */
  void GetNumVariables();
  /** An auxiliary function to call ReadParameters, ReadFunctions, ReadVarGroups, and ReadConstants
      \param fin - The input filestream of the paramotopy input file.
  */
  void ReadSizes(std::ifstream & fin);
  /** Read the number of parameters in the paramotopy input file.
      \param fin - The input filestream of the paramotopy input file.
  */
  void ReadParameters(std::ifstream & fin);
  /** Read the number of functions in the paramotopy input file.
      \param fin - The input filestream of the paramotopy input file.
  */  
  void ReadFunctions(std::ifstream & fin);
  /** Read the number of variable groups in the paramotopy input file.
      \param fin - The input filestream of the paramotopy input file.
  */
  void ReadVarGroups(std::ifstream & fin);
  /** Read the number of constants in the paramotopy input file.
      \param fin - The input filestream of the paramotopy input file.
  */
  void ReadConstants(std::ifstream & fin);
  /** Read the constant string from the paramotopy input file.
      \param fin - The input filestream of the paramotopy input file.
  */
  void ReadConstantStrings(std::ifstream & fin);
  /** Make the names parameters to be used in the bertini input file.
   */
  void MakeParameterNames();
  /** Define the variable groups to be used in the bertini input file.
      \param fout - The stringstream used to place everything into the bertini input file.
      \param paramotopy_settings - The paramotopy program settings.
  */
  void MakeVariableGroups(std::stringstream & fout, ProgSettings paramotopy_settings);
  /** Place the declaration of the constants onto the stringstream. 
      \param fout - The stringstream used to place everything into the bertini input file.
   */
  void MakeDeclareConstants(std::stringstream & fout);
  /** Place the definition of the constants onto the stringstream.
      \param fout - The stringstream used to place everything into the bertini input file. 
   */
  void MakeConstants(std::stringstream & fout);
  /** Make the definition of the constants and add to the stringstream. 
      \param CurrentValues - The values that are the constants. 
      \param inputfilestream - The stringstream used to place everything into the bertini step 2 input file. 
  */
  void MakeConstantsStep2(std::vector<std::pair<double, double> > CurrentValues, std::stringstream & inputfilestream, bool standardstep2);
  /** Make custom lines to add to the stringstream. 
      \param inputstringstream - The stringstream used to place everything into the bertini input file.
  */
  void MakeCustomLines(std::stringstream & inputfilestream);
  /** Place the declaration of functions on the stringstream. 
   \param inputstringstream - The stringstream used to place everything into the bertini input file. 
  */
  void MakeDeclareFunctions(std::stringstream & inputstringstream);
  /** Place the functions on the stringstream. 
  \param inputstringstream - The stringstream used to place everything into the bertini input file.
  */
  void MakeFunctions(std::stringstream & inputstringstream);
  /** Make the random values for the step 1 run.
   \param RandomRanges - The range in which the random values can be chosen for each parameter. */
  void MakeRandomValues(std::vector< std::pair< std::pair< double, double >, 
			std::pair< double, double > > > RandomRanges);

  
  /** Make the parameter values for the step 2 runs. 
   \param fin - The input filestream from which the values can be pulled from next. */
  void MakeValues(std::ifstream & fin);
  /** Make the parameter values for the step 2 runs. */
  void MakeValues();
	
	/** Set the right and left bounds for the interval in question. */
  void MakeBounds();
	
  /** ? */
  void SetDataFirst();
  /** ? */
  void SetDataMostRecent();
  /** ? */
  void SetDataInteractive();
  /** Write the data date started folder. */
  void WriteDataDateStarted();
  /** Delete the run folder. */
  void DeleteRunFolder();


	
protected:

	
};



#endif


