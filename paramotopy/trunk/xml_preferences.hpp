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

#ifndef __XMLPREFS__
#define __XMLPREFS__

#define minprefversion = 101; //please increase this every time you modify the preferences function(s).  added 11.04.21 dab

#include "para_aux_funcs.hpp"

class setting {
	
public:
	
	int filenumber;
	double doubvalue;
	int intvalue;
	std::string strvalue;
	int type; //0 string 1 integer 2 double
	
	
	
	
	setting() {
		strvalue = "uninitialized_value";
		type = -1;
		intvalue = -10001;
		doubvalue = -10002.1;};
	
	setting(std::string newValue){
		strvalue = newValue;
		type = 0;};
	
	setting(int newValue){
		intvalue = newValue;
		type = 1;};
	
	setting(double newValue){
		doubvalue = newValue;
		type = 2;};
	
	std::string typestr(){
		std::stringstream ss;
		ss << type;
		return ss.str();};
	
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



typedef std::map<std::string, setting> settingmap;
typedef std::map<std::string, settingmap> categorymap;



class ProgSettings {
	
public:
	enum {NUMMANDATORY_SAVEFILES = 1, NUMPOSSIBLE_SAVEFILES = 7, TOTALFILES = 8};


	
    static const char * const possible_savefiles[NUMPOSSIBLE_SAVEFILES];  //set in the cpp file 
	static const char * const mandatory_savefiles[NUMMANDATORY_SAVEFILES];//set in the cpp file
	
	
	int version;

	categorymap settings;  //a map of maps, holding settings in various categories.

	
	
	void save();           //save the xml file
	void save(std::string pFilename){
		filename = pFilename;
		save();};
  void save(boost::filesystem::path speciallocation);
  
	void load(const char* pFilename);
	void load() {load(filename.c_str());};

	ProgSettings(std::string tempfilename) {filename = tempfilename;};
	~ProgSettings() {};


	void setValue(std::string categoryName, std::string settingName, std::string settingValue){
		settings[categoryName][settingName] = setting(settingValue);}
	
	void setValue(std::string categoryName, std::string settingName, double settingValue){
		settings[categoryName][settingName] = setting(settingValue);}
	
	void setValue(std::string categoryName, std::string settingName, int settingValue){
		settings[categoryName][settingName] = setting(settingValue);}

	// main program settings, parallelism
	void GetParallel();
	void GetArchitecture();
	void GetSaveProgress();
	void GetNewFileThresh();
	void GetTemporaryFileLocation();
	void GetStifle();
	void GetStepTwoLocation();
	void GetMachineFile(); 
	void GetNumProcs();
	void GetNumFilesTime();
	void GetDeleteTmpFiles();
	void GetWriteMCFileUserDef();
	void GetStartFileName();
	
	void GetDataFolderMethod();
	void GetNewRandomAtNewFolder();
	
	// path failure prefs
	void GetRandomMethod();
	void GetSecurityLevelPathFailure();
	void GetTightenTolerancesPathFailure();
	void GetNumIterations();
	void GetBufferSize();
	
	
	// which files to save
	void SetSaveFiles();
	void GetIndividualFileSave(std::string datafilename);
	bool CheckPrevSetFiles();
	
	// make sure have the step2, bertini program
	void GetProgramLocationManual(std::string program_name, std::string category_name, std::string setting_name);
	void FindProgram(std::string program_name, std::string category_name, std::string setting_name);
	
	// menu functions
	void MainMenu();
	void StepOneMenu();
	void StepTwoMenu();
	void PathFailureMenu();
	void SaveFilesMenu();
	void ParallelismMenu();
	void GeneralMenu();
	
	void ManagePathFailureBertini();
	
	//set defaults
	void default_main_values();
	void default_basic_bertini_values_stepone();
	void default_basic_bertini_values_steptwo();
	void default_basic_bertini_values_pathfailure();
	void default_path_failure_settings();
	
	//display all settings in a given category
	void DisplayCurrentSettings(std::string category_name);
	
	//change, add, and remove settings from a category
	void ChangeSetting(std::string category_name);
	void AddSetting(std::string category_name);
	void RemoveSetting(std::string category_name);
	
	

	
	
	
	std::string WriteConfigStepOne();
	std::string WriteConfigStepTwo();
	std::string WriteConfigFail();

	
	void set_path_failure_settings();
	void set_path_failure_settings_from_steptwo();
	void tightentolerances();
	
	
private:
  std::string filename;  //the name of the preferences file
	bool setRequiredValues();
  bool haveSetting(std::string category_name, std::string setting_name){
		return settings[category_name].find(setting_name) != settings[category_name].end();};
	
  
	void RequiredSettingsSwitcharoo( int settingcase );//integers indicating switch cases found in .cpp file
	
  
  
  int SaveCategoryToXml(std::string catname, settingmap curr_settings , TiXmlElement* root);
	
	int ReadCategoryFromXml(std::string catname, TiXmlHandle hroot);
	
	
protected:
	
	
};

#endif

