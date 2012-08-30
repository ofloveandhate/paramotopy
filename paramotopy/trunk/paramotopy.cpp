#include <iostream>
#include <ios>
#include <string>
#include <fstream>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <list>
#include <vector>
#include <map>
#include <sstream>
#include <cmath>
#include "mtrand.hpp"
#include "step1_funcs.hpp"
#include "random.hpp"
#include "step2_funcs.hpp"
#include "para_aux_funcs.hpp"
#include "paramotopy_enum.hpp"
#include "menu_cases.hpp"
#include "failed_paths.hpp"
#include "xml_preferences.hpp"
//#include "preferences.h"
#include "tinyxml.h"
#include <mpi.h>
#include "runinfo.hpp"







int main(int argC, char *args[]){
	
	
	
	// Data members used throughout the program  
	std::string homedir = getenv("HOME");
	OPTIONS currentChoice = Start;
	std::string path_to_inputfile;
	bool parsed = false;
	std::ifstream fin, finconfig;

	
	runinfo paramotopy_info;  //holds all the info for the run 
	failinfo fail_info;
  
	std::vector< std::pair< std::pair< double, double >, std::pair< double, double > > > RandomRanges; 
	

	std::string mytemp;
	std::ifstream fin3;
	std::ofstream foutdir;
	std::string tmpbase;
	std::string tmpstr;
	
	std::string dirfilename;   

	
	
	
	std::stringstream myssmc;
	myssmc << " ";
	
	

	std::cout << "\n*******************************\n Welcome to Paramotopy.\n\n";
	
	
	
	std::string prefdir = homedir;
	prefdir.append("/.paramotopy");
	mkdirunix(prefdir);
	
	
	std::string settingsfilename = homedir;
	settingsfilename.append("/.paramotopy/paramotopyprefs.xml");
	ProgSettings paramotopy_settings(settingsfilename);
	paramotopy_settings.load();

	

	
	
	
	
	

	std::string suppliedfilename;
	if (argC==1) {
		paramotopy_info.GetInputFileName();
	}
	else{
		std::stringstream commandss;
		for (int i=0;i<argC;++i){
			commandss << args[i] << " ";	
		}
		std::string garbage;
		commandss >> garbage;
		commandss >> suppliedfilename;
		paramotopy_info.GetInputFileName(suppliedfilename);
	}
	
	
	
	paramotopy_info.ParseData();
	fin.close();
	

	
	
	paramotopy_info.AutoScanData(paramotopy_settings.settings["MainSettings"]["previousdatamethod"].intvalue);//sets the base_dir
	
	
	if (!paramotopy_info.GetPrevRandom()){
		paramotopy_info.MakeRandomValues();
		std::cout << "made new random values" << std::endl;
	}
	
	paramotopy_info.CopyUserDefinedFile();
	
	

	
	
	
	parsed=true;
	
	
	if (paramotopy_info.test_if_finished()){
		std::cout	<< "\t*** * * * * * * * * * * * * ***\n"
					<< "\t***this run appears finished***\n"
					<< "\t*** * * * * * * * * * * * * ***\n";
	}

	int intChoice=-1;	
	
	// Make the appropriate Directory to place the input file in
	paramotopy_info.mkdirstep1();


	while(currentChoice!=Quit){
		
		intChoice = GetUserChoice();
		
		switch (intChoice){
			case 1 :
				
				currentChoice = Input;

				paramotopy_info.GetInputFileName();
				paramotopy_info.ParseData();

				paramotopy_info.mkdirstep1();
				paramotopy_info.AutoScanData(paramotopy_settings.settings["MainSettings"]["previousdatamethod"].intvalue);

				if (!paramotopy_info.GetPrevRandom()){
					paramotopy_info.MakeRandomValues();
					std::cout << "made new random values" << std::endl;
				}
				paramotopy_info.CopyUserDefinedFile();
				parsed=true;
				break;
				
				
				
			case 2:
				//data management
				paramotopy_info.DataManagementMainMenu();
				mkdirunix(paramotopy_info.base_dir);
				paramotopy_info.mkdirstep1();
				std::cout << "file directory is now " << paramotopy_info.base_dir << "\n";
				if (!paramotopy_info.GetPrevRandom()){  //if didn't find previous values to load, make new ones (but only if use wants to
					if (paramotopy_settings.settings["MainSettings"]["newrandom_newfolder"].intvalue == 1){
						paramotopy_info.MakeRandomValues();
						std::cout << "made new random values" << std::endl;
					}
				}

				paramotopy_info.UpdateAndSave();
				break;
				
				
				
			case 3: // for use later?
				std::cout << "unprogrammed\n";

				break;
				
				
				
			case 4: // Random point management
				

				paramotopy_info.RandomMenu();
				break; 
				
				
				
				
			case 5: // Write Step1
				currentChoice=WriteStepOne;
				paramotopy_info.location = paramotopy_info.base_dir;
				paramotopy_info.WriteOriginalParamotopy(paramotopy_info.base_dir);
				WriteStep1(paramotopy_settings,
						   paramotopy_info);
				paramotopy_info.UpdateAndSave();
				break;
				
				
				
				
			case 6: //run step 1
				
				currentChoice = RunStepOne;
				paramotopy_info.location = paramotopy_info.base_dir;
				
				

				
				if (!paramotopy_info.CheckRunStepOne()){
					break;
				}
				
				paramotopy_info.WriteOriginalParamotopy(paramotopy_info.base_dir);
				WriteStep1(paramotopy_settings,
						   paramotopy_info);
				
				paramotopy_info.WriteRandomValues();
				CallBertiniStep1(paramotopy_settings, 
								 paramotopy_info);//base_dir);
				
				paramotopy_info.UpdateAndSave();
				break;
				
				
				
			case 7:  //run step 2
				
				currentChoice = Step2;
				paramotopy_info.location = paramotopy_info.base_dir;
				paramotopy_info.step2mode = 2;
				steptwo_case(paramotopy_settings,
							 paramotopy_info);
				
				paramotopy_info.UpdateAndSave();
				break;
				
				
			case 8: //do failed path whatnot.
				currentChoice = FailedPaths;
				paramotopy_info.step2mode = 3;
				fail_info.MainMenu(paramotopy_settings,paramotopy_info);
				paramotopy_info.UpdateAndSave();
				break;
				
				
				
			case 9:  //change preferences
				currentChoice=DetPrefs;
				paramotopy_settings.MainMenu();
				break;
				
				
				
			case 0:  //quit
				
				currentChoice=Quit;	    
				std::cout << "Quitting\n\n";
				
				
		}//re: switch (intChoice)

	}//re: while(currentChoice!=Quit)

	return 0;
}//re: main function
