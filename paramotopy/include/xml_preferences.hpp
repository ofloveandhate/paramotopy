#include <iostream>
#include <ios>
#include <fstream>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string>
#include <map>
#include <sstream>
#include <set>
#include "tinyxml.h"

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/progress.hpp>
#include <boost/regex.hpp>
#include <boost/function.hpp>
#ifndef __XMLPREFS__
#define __XMLPREFS__

#define minprefversion = 103; //please increase this every time you modify the preferences function(s).  added 11.04.21 dab

//#include "runinfo.hpp"
#include "para_aux_funcs.hpp"

/**
   @brief A class that stores the information of a setting as used in an xml file.

 */




class menu_option {
	
private:
	std::string name;
	boost::function<void()> call;
};

class setting {
	
public:
	
  /** \param filenumber - deprecated ? */ 
	int filenumber;
  /** \param doubvalue - The value of the double value to store. */
	double doubvalue;
  /** \param intvalue - The value of the int value to store. */
	int intvalue;
  /** \param strvalue - The value of the string value to store. */
	std::string strvalue;
  /** \param type - Integer to indicate the type of setting.  
      0 for string, 1 for inter, 2 for double. */
	int type; //0 string 1 integer 2 double
	
	
	
  /** \brief - Default constructor for setting class. */
	setting() {
		strvalue = "uninitialized_value";
		type = -1;
		intvalue = -10001;
		doubvalue = -10002.1;};
  /**  \param newValue - The string to store in strvalue. */
	setting(std::string newValue){
		strvalue = newValue;
		type = 0;};
  /** \param newValue - The int to store in intvalue. */
	setting(int newValue){
		intvalue = newValue;
		type = 1;};
  /** \param newValue - The double to store in doubvalue. */
	setting(double newValue){
		doubvalue = newValue;
		type = 2;};
  /** \return string - Returns the type of the setting as a string. "0", "1", or "2". */
	std::string typestr(){
		std::stringstream ss;
		ss << type;
		return ss.str();};
  /** \return string - Returns the value stored as a string. */
	std::string value(){
		std::string val;
		std::stringstream ss;
				
		switch (type) {
			case 0:
				val = strvalue;				
				break;
			case 1:				
				ss << intvalue;
				val = ss.str();				
				break;
			case 2:
				ss << doubvalue;
				val = ss.str();
				break;
				
			default:
				val = "badtype";
				break;
		};
		return val;
	};
};


/* Create a typedef with a string as the value type and the setting as the key type */

typedef std::map<std::string, setting> settingmap;

/* Create a typedef with a string as a value type and the settingmap (prior typedef) as the key type */ 

typedef std::map<std::string, settingmap> categorymap;

/** \brief A class that stores the general program settings of Paramotopy.  */

class ProgSettings {
	
public:
  
  /* An enum to store constant values.  These could be constants as they don't change within the program */ 
  enum {NUMMANDATORY_SAVEFILES = 1, NUMPOSSIBLE_SAVEFILES = 7, TOTALFILES = 8};
  
  static const char * const possible_savefiles[NUMPOSSIBLE_SAVEFILES];  //set in the cpp file 
  static const char * const mandatory_savefiles[NUMMANDATORY_SAVEFILES];//set in the cpp file
  
  
  /** The current version. */
  int version;
  /** A map of maps, holding settings for the various categories. */
  categorymap settings;  //a map of maps, holding settings in various categories.
  
  
  /** Save the xml file to the hard disk. */
  void save(){
			save(this->filename);
	};           //save the xml file
  /** Save the xml file to the hard disk.
      \param pFilename - The file to where the xml file is to be saved. */
	
  void save(std::string save_filename);
  /** Save the xml file to the hard disk.  This function uses the boost library directory and path structures.
      \param speciallocation - The file to where the xml file is to be saved. 
  */
  void save(boost::filesystem::path speciallocation);
	
	
	/** find existing preferences file, based on input
   \param pFilename - The preference file name. 
	 \return int found_a_file whether found a file to load or not.  0 if no file, even default.  1 if found desired file.  2 if found defaults.
	 \return bool load_filename the string which is the name of the file. */
	
	int check_for_existing_prefs_auto(std::string & load_filename, const char* pFilename);
	
	/** get the name of the default paramotopy settings xml file
		
	 */
	std::string default_name();
	
	/**
	 gets ~/.paramotopy/prefsfilename.xml
	 */
	std::string make_settings_name(std::string basename);
	
	/**
	 sets the .filename field to whatever you put in
	 */
	void set_name(std::string newfilename){
		this->filename = newfilename;
	};
	
	/**
	 an interactive way to choose a set of settings to load.
	 */
	void load_interactive();
	
  /** Load the xml preference file.
   \param pFilename - The preference file name. */
  void load(const char* pFilename);
  /** Load the xml preference file with the current saved filename. */
  void load() {load(this->filename.c_str());};
  
	
	ProgSettings() {};
  /** \param tempfilename - Set the filename data member to tempfilename. */ 
  ProgSettings(std::string tempfilename) {filename = tempfilename;};
  /** Default Deconstructor. */
  ~ProgSettings() {};
  
  /** A function to set the value of a given category and setting.
      \param categoryName - The category name in which to search for the settingName. 
      \param settingName - The setting name in which to store settingValue.
      \param settingValue - The value to store as a string.
  */

  void setValue(std::string categoryName, std::string settingName, std::string settingValue){
    settings[categoryName][settingName] = setting(settingValue);}
  /** A function to set the value of a given category and setting.  
      \param categoryName - The category name in which to search for the settingName.          
      \param settingName - The setting name in which to store settingValue.   
      \param settingValue - The value to store as a double.               
  */
  void setValue(std::string categoryName, std::string settingName, double settingValue){
    settings[categoryName][settingName] = setting(settingValue);}
  /** A function to set the value of a given category and setting.  
      \param categoryName - The category name in which to search for the settingName.          
      \param settingName - The setting name in which to store settingValue.   
      \param settingValue - The value to store as an int.               
  */  
  
  void setValue(std::string categoryName, std::string settingName, int settingValue){
    settings[categoryName][settingName] = setting(settingValue);}
  
  // main program settings, parallelism
  /** Get the parallel setting. */
  void GetParallel();
  /** Get the architecture setting. */
  void GetArchitecture();
  /** Get the save progress setting. */
  void GetSaveProgress();
  /** Get the new file threshold setting. */
  void GetNewFileThresh();
  /** Get the temporary file location. */
  void GetTemporaryFileLocation();
  /** Get the stifle output setting. */
  void GetStifle();
  /** Get the machine file location. */
  void GetMachineFile(); 
  /** Get the number of processors to be used. */
  void GetNumProcs();
  /** Get the number of files at a time to save. ? */
  void GetNumFilesTime();
  /** Get the delete temp file setting. */
  void GetDeleteTmpFiles();
  /** Get the write monte carlo file user defined setting. */
  void GetWriteMCFileUserDef();
  /** Get the start file name setting. */
  void GetStartFileName();
  /** Get the data folder method setting. */
  void GetDataFolderMethod();
  /** Get the data folder method setting. */
  void GetNewRandomAtNewFolder();
  
  // path failure prefs
  /** Get the random method setting. */
  void GetRandomMethod();
  /** Get the tighten tolerances in the path failure resolution settings. */
  void GetTightenTolerancesPathFailure();
  /** Get the number of iterations to be performed during path failure resolution. */
  void GetNumIterations();
  /** Get the buffer size of information to be stored in memory before writing to the disk. */
  void GetBufferSize();
  
  
  // which files to save
  /** Set the files from the bertini ouptut to save. */
  void SetSaveFiles();
  /** \param datafilename - Determine if datafilename is to be saved or not. */
  void GetIndividualFileSave(std::string datafilename);
  /** ? ? */
  bool CheckPrevSetFiles();
  
  // make sure have the step2, bertini program
  /** Display the location of the step2 and bertini binaries.*/
  void GetProgramLocationManual(std::string program_name, std::string category_name, std::string setting_name);
  /** Find the locaton of the step2 and bertini binaries. */
  void FindProgram(std::string program_name, std::string category_name, std::string setting_name);
  
  // menu functions
  /** Display paramotopy's main menu. */
  void MainMenu();
	
	/** Chain into the bertini settings menus. */
	void BertiniMenu();
  /** Display the step one options menu. */
  void StepOneMenu();
  /** Display the step two options menu. */
  void StepTwoMenu();
  /** Display the path failure options menu. */
  void PathFailureMenu();
  /** Display the saved files options menu. */
  void SaveFilesMenu();
  /** Display the parallelism options menu. */
  void ParallelismMenu();		       
  /** Display the general options menu. */
  void GeneralMenu();
	
	/** main switch for solver modes. */
	void SetSearchMode();
	
	/** get the number of iterations for a pos real search */
	void SetSearchIterations();
	
	/** switch between accumulating a specific number of solutions, or a number of points with a desired number of positive real solutions */
	void SetSearchSubmode();
	
	/** get the threshold for number of positive real solutions occurring at any point */
	void SetSearchPosRealThresh();
	
	
	/** get the number of desired solutions for a pos real search */
	void SetSearchDesiredNumber();
	
	
	/** menu for search mode. */
	void SearchMenu();
	
	
	/** Menu for Solver modes. */
	void SolverModeMenu();
	/** Menu for system interaction. */
	void SystemMenu();
	/** Menu for memory management. */
	void FileMenu();
	
  /** Display the path failure options for bertini menu. */
  void ManagePathFailureBertini();
  /**  perform meta-settings actions */
	void MetaSettingsMenu();
  //set defaults
  /** Set the default values of the program. */
  void default_main_values();
  /** Set the default bertini step 1 values. */
  void default_basic_bertini_values_stepone();
  /** Set the default bertini step 2 values. */
  void default_basic_bertini_values_steptwo();
  /** Set the default path failure settings to be used in bertini. */
  void default_basic_bertini_values_pathfailure();
  /** Set the default path failure settings. */
  void default_path_failure_settings();
  
  //display all settings in a given category
  /** Display all the settings of the given category.
   \param category_name - The given category. */
  void DisplayCurrentSettings(std::string category_name);
  
  //change, add, and remove settings from a category
  /** Change the setting of the given category. 
   \param category_name - The given category. */
  void ChangeSetting(std::string category_name);
  /** Add a setting of the given category. 
   \param category_name - The given category. */
  void AddSetting(std::string category_name);
  /** Remove a setting of the given category.
   \param category_name - The given category. */
  void RemoveSetting(std::string category_name);
  
  /** Write the bertini configuration portion of the bertini step 1 run. */
  std::string WriteConfigStepOne();
  /** Write the bertini configuration portion of the bertini step 2 run. */
  std::string WriteConfigStepTwo();
  /** Write the bertini configuration portion of the failed paths runs run. */
  std::string WriteConfigFail();
  
  /** Set the path failure settings (for step 1 ? ). */
  void set_path_failure_settings();
  /** Set the path failure settings from (for? - rename function?) step 2. */
  void set_path_failure_settings_from_steptwo();
  /** Set whether or not to automatically tighten the tolerances on the path failure settings. */
  void tightentolerances();
  
  /**
     Ask the user to enter the standard step2 option.
   */
  void SetStandardStep2();
	
private:
  /** The name of the preferences file */
  std::string filename;  
  /** Set the required values. */
  bool setRequiredValues();
  /** Test if a category of category_name has a setting of setting_name.
      \param category_name - The category.
      \param setting_name - The setting.
      \return bool - True if the setting_name was found within that category. False, otherwise.
  */
  bool haveSetting(std::string category_name, std::string setting_name){
    return settings[category_name].find(setting_name) != settings[category_name].end();};
  
  
  void RequiredSettingsSwitcharoo( int settingcase );//integers indicating switch cases found in .cpp file
  
  
  /** Save a category with the current settings to xml. 
   \param catname - The category name.
   \param curr_settings - The current settings. 
   \param root - A pointer to the root of the TiXmlElement (of catname ? ). */
  int SaveCategoryToXml(std::string catname, settingmap curr_settings , TiXmlElement* root);
  
  /** Read a category from xml. 
   \param catname - The category name.
   \param hroot - The handle of the root of catname. */
  int ReadCategoryFromXml(std::string catname, TiXmlHandle hroot);
  
  
protected:
	
	
};

#endif

