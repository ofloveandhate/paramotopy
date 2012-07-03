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



#define prefversion = 15; //please increase this every time you modify the preferences function(s).  added 11.04.21 dab



int main(int argC, char *args[]){
	
	
	
	
	
	
	
	//	get_curr_dir_name();
	
	
	
	
	// Data members used throughout the program  
	std::string homedir = getenv("HOME");
	OPTIONS currentChoice = Start;
	std::string filename, path_to_inputfile;
	std::string base_dir ="";
	bool parsed = false;
	std::ifstream fin, finconfig;
	std::ofstream fout;
	
	int numfunct, numvariables, numvargroup, numparam, numconsts;
	//  MTRand drand(time(0));
	
	
	
	
	std::vector<std::string> FunctVector; 
	std::vector<std::string> VarGroupVector; 
	std::vector<std::string> ParamVector; 
	std::vector<std::string> ParamStrings;
	std::vector<std::string> Consts;
	std::vector<std::string> ConstantStrings;
	// values of points -- two different ways storing the data
	// whether or not we are user-defined or not
	// if doing a mesh, the data is contained as follows
	// -- first vector size is the number of parameters
	// -- second vector size the number of mesh points for the given param
	// -- pair is the real (first) and imaginary (second)
	
	// if not doing a mesh, the data is contained as follows
	// -- first vector size is number of sample n-ples)
	// -- second vector size is n
	// -- pair is the real (first) and imaginary (second)
	
	
	std::vector< std::vector< std::pair<double,double> > > Values;
	
	// random initial values
	// -- vector size is the number of parameters
	// -- pair is the real (first) and imaginary (second)
	std::vector< std::pair<double,double> > RandomValues;  
	std::vector< std::pair< std::pair< double, double >, std::pair< double, double > > > RandomRanges; 
	
	bool userdefined;
	std::string mytemp;
	std::ifstream fin3;
	std::ofstream foutdir;
	std::string tmpbase;
	std::string tmpstr;
	
	std::string dirfilename;   
	std::string mcfname;  
	std::vector< int > NumMeshPoints;//added Nov16,2010 DAB
	
	
	
	std::stringstream myssmc;
	myssmc << " ";
	
	
	
	std::cout << "\n*******************************\n"
	<< "Welcome to the Paramotopy main program.\n\n";
	
//	int numfilespossible = 8; // 
//	std::vector< bool >  FilePrefVector;
//	FilePrefVector.resize(numfilespossible-1);
//	
	
	
	std::string prefdir = homedir;
	prefdir.append("/.paramotopy");
	mkdirunix(prefdir);
	
	std::string settingsfilename = homedir;
	settingsfilename.append("/.paramotopy/paramotopyprefs.xml");
	ProgSettings paramotopy_settings(settingsfilename);
	

	
	paramotopy_settings.load();

	

	
	
	
	
	
	bool finopened = false;//for parsing input file.
	bool suppliedfilename = true;
	bool alreadytried = false;//for testing file openness if user supplied command-line filename
	
	
	if (argC==1) {
		suppliedfilename = false;
		std::cout << "\n\nBefore you begin any real work, you must choose an input file to parse.\n";
	}
	
	
	//open file
	while (!finopened) {
		if ( (!suppliedfilename) || alreadytried) {
			std::cout << "Enter the input file's name : ";
			std::cin >> filename;
		}
		else{
			std::stringstream commandss;
			for (int i=0;i<argC;++i){
				commandss << args[i] << " ";	
			}
			std::string garbage;
			commandss >> garbage;
			commandss >> filename;
		}
		
		
		
		
		fin.open(filename.c_str());
		if (fin.is_open()){
			finopened = true;
		}
		else {
			std::cout << "could not open a file of that name... try again.\n";
			alreadytried=true;
		}
	}
	finopened = false;//reset
	//end open file 
	
	ParseData(numfunct,numvargroup,numparam,numconsts,FunctVector,VarGroupVector,ParamVector,ParamStrings,Consts,ConstantStrings,Values,RandomValues,userdefined,fin,NumMeshPoints,filename);
	fin.close();
	numvariables = GetNumVariables(numvargroup, VarGroupVector);
	
	std::cout << numvariables << " total variables detected.\nDone parsing.\n";
	parsed=true;
	base_dir=make_base_dir_name(filename);
	
	PrintRandom(RandomValues,ParamStrings);
	
	if (test_if_finished(base_dir)){
		std::cout	<< "\t*** * * * * * * * * * * * * ***\n"
					<< "\t***this run appears finished***\n"
					<< "\t*** * * * * * * * * * * * * ***\n";
	}
	int iteration = 0;
	int intChoice=-1;	
	while(currentChoice!=Quit){
		
		
		
		
		intChoice = GetUserChoice();
		
		
		switch (intChoice){
			case 1 :
				
				currentChoice = Input;
				finopened = false;
				while (!finopened) {
					
					
					std::cout << "Enter in the input file's name : ";
					std::cin >> filename;
					fin.close();
					fin.open(filename.c_str());
					if (fin.is_open()) {
						finopened = true;
					}
				}
				finopened = false;
				ParseData(numfunct,numvargroup,
						  numparam,numconsts,FunctVector,
						  VarGroupVector,ParamVector,
						  ParamStrings,
						  Consts,
						  ConstantStrings,
						  Values,
						  RandomValues,
						  userdefined,
						  fin,
						  NumMeshPoints,
						  filename);
				fin.close();
				
				parsed=true;
				numvariables = GetNumVariables(numvargroup, VarGroupVector);
				base_dir=make_base_dir_name(filename);

				PrintRandom(RandomValues,ParamStrings);
				break;
			case 2:
				
				currentChoice = SetRandom;
				RandomValues = random_case(RandomValues, ParamStrings);			
				break;
				
				
				
			case 3: // Save Random points
				
				currentChoice = SaveRandom;
				save_random_case(RandomValues, fout, numparam);
				break;
				
				
				
			case 4: // Load Random points
				
				currentChoice = LoadRandom;
				load_random_case(fin3, RandomValues,ParamStrings);
				break; 
				
				
			case 5:         // Write Step1
				currentChoice=WriteStepOne;
				
				
				WriteStep1(filename, 
						   "config1", 
						   FunctVector,
						   ParamStrings, 
						   VarGroupVector, 
						   Consts, 
						   ConstantStrings,
						   RandomValues);
				
				
				
				std::cout << "Writing Step 1 done ... \n";
				
				
				break;
				
				
				
				
			case 6: 
				currentChoice = RunStepOne;
				WriteStep1(filename, 
						   "config1", 
						   FunctVector,
						   ParamStrings, 
						   VarGroupVector, 
						   Consts, 
						   ConstantStrings,
						   RandomValues);
				
				
					CallBertiniStep1(paramotopy_settings, base_dir);

				break;
				
				
				
			case 7:
				
				
				currentChoice = Step2;

				
				steptwo_case(filename,
							 paramotopy_settings,
							 numparam, 
							 RandomValues,
							 ParamStrings);
				

				break;
				
				
			case 8: 
				currentChoice = FailedPaths;
				
				iteration = 0;
				//                                                         0 here because first iteration of failed path analysis
				failedpaths_case(numparam, numvariables, base_dir,filename,iteration);  // should this return any info to paramotopy?  q posed may 7 2012
				
				break;
				
			case 9:
				currentChoice=DetPrefs;

				paramotopy_settings.MainMenu();
				
				break;
				
			case 0:
				
				currentChoice=Quit;	    
				std::cout << "Quitting\n\n";
				
				
		}//re: switch (intChoice)
		
	}//re: while(currentChoice!=Quit)

	return 0;
}//re: main function
