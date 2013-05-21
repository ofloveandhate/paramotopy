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
#include "datagatherer.hpp"
#include "xml_preferences.hpp"
//#include "preferences.h"
#include "tinyxml.h"
#include <mpi.h>
#include "runinfo.hpp"






/**
\brief This is the main function for the paramotopy program.  It acts as the user interface, and provides * the commands for calling bertini for step1, * the wrapper around the step2 program, * failed path functionality, *bertini settings.
 
 main will load settings from previous session, load the input file, and provide the user with a numeric input selection.
*/
int main(int argC, char *args[]){
	
	
	
	// Data members used throughout the program  
	std::string homedir = getenv("HOME");
	OPTIONS currentChoice = Start;
	std::string path_to_inputfile;
	bool parsed = false;
	std::ifstream finconfig;

	
	runinfo paramotopy_info;  //holds all the info for the run 
	failinfo fail_info;
  
	std::vector< std::pair< std::pair< double, double >, std::pair< double, double > > > RandomRanges; 
	

	std::string mytemp;
	std::ifstream fin3;
	std::ofstream foutdir;
	std::string tmpbase;
	std::string tmpstr;
	
	std::string dirfilename;   

	// to allow for the choice from the user
	// to determine if 
	// a parameter continuation run should be done
	// versus a standard bertini run
	//	bool standardstep2 = true; // should be a choice that changes
	                           // only on the particular run
                                   // and should be a user-choice
                                   // as runs will be dependent on system
	
	

	

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

	
	
	
	std::string prefdir = homedir;
	prefdir.append("/.paramotopy");
	mkdirunix(prefdir);
	

	ProgSettings paramotopy_settings;
	paramotopy_settings.set_name(paramotopy_settings.make_settings_name(paramotopy_info.inputfilename));
	paramotopy_settings.load();
	
	//splash the bertini intro's for the library and the executable.
	BertiniSplashScreen(paramotopy_settings.settings["system"]["bertinilocation"].value());
	
	
	
	
	
	paramotopy_info.AutoScanData(paramotopy_settings.settings["files"]["previousdatamethod"].intvalue);//sets the base_dir
	
	
	if (!paramotopy_info.GetPrevRandom()){
		paramotopy_info.MakeRandomValues();
		std::cout << "made new random values" << std::endl;
	}
	
	
	
	// Make the appropriate Directory to place the input file in
	paramotopy_info.mkdirstep1();
	

	
	
	
	parsed=true;
	
	ParamotopySplashScreen();
	
	
	if (paramotopy_info.test_if_finished()){
		std::cout	<< "\t*** * * * * * * * * * * * * ***\n"
					<< "\t***this run's step2 appears finished***\n"
					<< "\t*** * * * * * * * * * * * * ***\n";
	}

	int intChoice=-1;	
	


	while(currentChoice!=Quit){
		
		intChoice = ParamotopyMainMenu();  // gets the choice from the user.
		
		switch (intChoice){
			case 1 :
				
				currentChoice = Input;

				paramotopy_info.GetInputFileName();
				paramotopy_info.ParseData();


				paramotopy_info.AutoScanData(paramotopy_settings.settings["files"]["previousdatamethod"].intvalue);

				if (!paramotopy_info.GetPrevRandom()){
					paramotopy_info.MakeRandomValues();
					std::cout << "made new random values" << std::endl;
				}
				paramotopy_info.mkdirstep1();
				paramotopy_info.CopyUserDefinedFile();
				parsed=true;
				
				
				paramotopy_settings.set_name(paramotopy_settings.make_settings_name(paramotopy_info.inputfilename));
				paramotopy_settings.load();
				
				
				break;
				
				
				
			case 2:
				//data management
				DataManagementMainMenu(paramotopy_info);
				mkdirunix(paramotopy_info.base_dir);
				paramotopy_info.mkdirstep1();
				std::cout << "file directory is now " << paramotopy_info.base_dir << "\n";
				paramotopy_info.GetPrevRandom();  //if didn't find previous values to load, make new ones (but only if use wants to
				if ( (paramotopy_info.RandomValues.size()==0) || ((paramotopy_info.made_new_folder) && (paramotopy_settings.settings["files"]["newrandom_newfolder"].intvalue == 1)) )
				{
					paramotopy_info.MakeRandomValues();
					std::cout << "made new random values" << std::endl;
				}
				
				paramotopy_info.CopyUserDefinedFile();
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
								 paramotopy_info);
				
				paramotopy_info.UpdateAndSave();
				
				
				break;
				
				
				
			case 7:  //run step 2

			  std::cout << "\n\n***Step2***\n\n";

			  currentChoice = Step2;
			  paramotopy_info.location = paramotopy_info.base_dir;
			  paramotopy_info.steptwomode = 2;
			  
			  
			  steptwo_case(paramotopy_settings,
				       paramotopy_info);
			  
			  
			  
			  paramotopy_info.UpdateAndSave();
			  break;
			  
				
			case 8: //do failed path whatnot.
				currentChoice = FailedPaths;
				paramotopy_info.steptwomode = 3;
				fail_info.MainMenu(paramotopy_settings,paramotopy_info);
				paramotopy_info.UpdateAndSave();
				break;
				
				
				
			case 99:  //change preferences
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
