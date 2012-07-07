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
#include "mtrand.h"
#include "step1_funcs.h"
#include "random.h"
#include "step2_funcs.h"
#include "para_aux_funcs.h"
#include "paramotopy_enum.h"
#include "menu_cases.h"
#include "failed_paths.h"
#include "xml_preferences.h"
//#include "preferences.h"
#include "tinyxml.h"
#include <mpi.h>
#include "runinfo.h"


#define prefversion = 100; //please increase this every time you modify the preferences function(s).  added 11.04.21 dab



int main(int argC, char *args[]){
	
	
	
	// Data members used throughout the program  
	std::string homedir = getenv("HOME");
	OPTIONS currentChoice = Start;
	std::string path_to_inputfile, base_dir = "";
	bool parsed = false;
	std::ifstream fin, finconfig;

	
	runinfo paramotopy_info;  //holds all the info for the run 
	
  
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
	

	parsed=true;
	
//	base_dir=make_base_dir_name(paramotopy_info.inputfilename);
//	
	
	if (paramotopy_info.test_if_finished()){
		std::cout	<< "\t*** * * * * * * * * * * * * ***\n"
					<< "\t***this run appears finished***\n"
					<< "\t*** * * * * * * * * * * * * ***\n";
	}
	int iteration = 0;
	int intChoice=-1;	
	
	// Make the appropriate Directory to place the input file in
	paramotopy_info.mkdirstep1();
	paramotopy_info.DisplayAllValues();
	while(currentChoice!=Quit){
		
		
		
		
		intChoice = GetUserChoice();
		
		
		switch (intChoice){
			case 1 :
				
				currentChoice = Input;

				paramotopy_info.GetInputFileName();
				paramotopy_info.ParseData();
				paramotopy_info.SetLocation();

				parsed=true;
				break;
				
				
				
			case 2:
				
				currentChoice = SetRandom;
				paramotopy_info.SetRandom();	
				break;
				
				
				
			case 3: // Save Random points
				
				currentChoice = SaveRandom;
				paramotopy_info.SaveRandom();
				break;
				
				
				
			case 4: // Load Random points
				
				currentChoice = LoadRandom;
				paramotopy_info.LoadRandom();
				break; 
				
				
				
				
			case 5: // Write Step1
				currentChoice=WriteStepOne;

				WriteStep1(paramotopy_settings,
						   paramotopy_info);
				break;
				
				
				
				
			case 6: //run step 1
				
				currentChoice = RunStepOne;

				WriteStep1(paramotopy_settings,
						   paramotopy_info);
				
				
				CallBertiniStep1(paramotopy_settings, 
								 paramotopy_info);//base_dir);

				break;
				
				
				
			case 7:  //run step 2
				
				currentChoice = Step2;

			


				
				steptwo_case(paramotopy_settings,
							 paramotopy_info);
				

				break;
				
				
			case 8: //do failed path whatnot.
				currentChoice = FailedPaths;
				
				iteration = 0;

				//iteration=0 here because first iteration of failed path analysis
				failedpaths_case(paramotopy_info, 
								 paramotopy_settings,
								 iteration);  // should this return any info to paramotopy?  q posed may 7 2012
				
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
