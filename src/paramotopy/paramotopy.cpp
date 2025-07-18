#include "paramotopy/paramotopy.hpp"





/**
\brief This is the main function for the paramotopy program.  It acts as the user interface, and provides * the commands for calling bertini for step1, * the wrapper around the step2 program, * failed path functionality, *bertini settings.
 
 main will load settings from previous session, load the input file, and provide the user with a numeric input selection.
*/
int main(int argC, char *args[]){
	
	
	
	// Data members used throughout the program  
	std::string homedir = getenv("HOME");
	
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
	
	

	
	if (argC==1) {
		paramotopy_info.GetInputFileName();
	}
	else{
		std::stringstream commandss;
		commandss << args[1];
		
		std::string suppliedfilename = commandss.str();
		
		paramotopy_info.GetInputFileName(suppliedfilename);
	}
	paramotopy_info.ParseData();

	
	
	
	std::string prefdir = homedir;
	prefdir.append("/.paramotopy");
	boost::filesystem::create_directories(prefdir);
	

	ProgSettings paramotopy_settings;
	paramotopy_settings.set_name(paramotopy_settings.make_settings_name(paramotopy_info.inputfilename));
	paramotopy_settings.load();
	
	//splash the bertini intro's for the library and the executable.
	BertiniSplashScreen(paramotopy_settings.settings["system"]["bertinilocation"].value());
	
	
	
	
	
	paramotopy_info.AutoScanData(paramotopy_settings.settings["files"]["previousdatamethod"].intvalue);//sets the base_dir
	
	
	if (!paramotopy_info.GetPrevRandom()){
		paramotopy_info.MakeRandomValues();
		std::cout << "couldn't find previous random values, made new random values" << std::endl;
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


	OPTIONS currentChoice = Start;
	
	while(currentChoice!=Quit){
		
		int intChoice = ParamotopyMainMenu();  // gets the choice from the user.
		
		switch (intChoice){
			case 1 :
				
				currentChoice = Input;

				SetNewInput(paramotopy_info, paramotopy_settings);
				
				parsed=true;
				
				break;
				
				
				
			case 2:
				//data management
				DataManagementMainMenu(paramotopy_info);


				boost::filesystem::create_directories(paramotopy_info.base_dir);
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




void SetNewInput(runinfo & paramotopy_info, ProgSettings & paramotopy_settings)
{
	paramotopy_info.GetInputFileName();
	paramotopy_info.ParseData();


	paramotopy_info.AutoScanData(paramotopy_settings.settings["files"]["previousdatamethod"].intvalue);

	if (!paramotopy_info.GetPrevRandom()){
		paramotopy_info.MakeRandomValues();
		std::cout << "made new random values" << std::endl;
	}
	paramotopy_info.mkdirstep1();
	paramotopy_info.CopyUserDefinedFile();
	
	paramotopy_settings.set_name(paramotopy_settings.make_settings_name(paramotopy_info.inputfilename));
	paramotopy_settings.load();
}










//the main choice function for paramotopy.
int ParamotopyMainMenu(){
	
  std::stringstream menu;
	
  menu << "\n\nYour choices : \n\n"
       << "1) Parse an appropriate input file. \n"
       << "2) Data Management. \n"
       << "3)   -unprogrammed- \n"
       << "4) Manage start point (load/save/new). \n"
       << "5) Write Step 1.\n"
       << "6) Run Step 1.\n"
       << "7) Run Step 2.\n"
       << "8) Failed Path Analysis.\n"
       << "\n"
       << "99) Preferences.\n"
       << "*\n"
       << "0) Quit the program.\n\n"
       << "Enter the integer value of your choice : ";
  
	std::cout << menu.str();
	
  int intChoice = -1;
	while (  (intChoice>=9 && intChoice<=98) || (intChoice >=100 || intChoice < 0) ) {
		intChoice = get_int_choice("", 0, 99);
	}
  
	
	
  return intChoice;
}  //   re: getuserchoice




