#include "para_aux_funcs.h"



//  use this function to get inout fro the user and ensure it is an interger (actually fails to detect non-integer numeric inputs
int get_int_choice(std::string display_string,int min_value,int max_value){
	
	int userinput = min_value - 1;
	
	while (  (std::cout << display_string) && 
		   ( !(std::cin >> userinput) || userinput < min_value || userinput > max_value))
	{
		std::cout << "Invalid entry -- try again:\n";
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
	return userinput;
}


std::string make_base_dir_name(std::string filename){
	
	std::string base_dir = "";
	
	std::string remainder = filename;
	size_t found;
	found = remainder.find('/');
	while (found!=std::string::npos) {  //if found the delimiter '/'
		
		base_dir.append(remainder.substr(0,found+1));  //the part of the path to scan for mystep2
		std::cout << base_dir << "\n";
		remainder = remainder.substr(found+1,remainder.length()-found);    // the remainder of the path.  will scan later.
		found = remainder.find('/');                             // get the next indicator of the '/' delimiter.
		
		

	}// re:while
	base_dir.append("bfiles_");
	base_dir.append(remainder);
		
	return base_dir;
}


//locates the step2program.
int Find_Program_Step2(preferences *Prefs){
	
	
	bool found_step2_program = false;
	struct stat filestatus;
	
	if (stat( "./step2", &filestatus ) ==0){  // if have the mystep2 program in current directory, set location of it to here.
		Prefs[0].step2location = ".";
		found_step2_program = true;
	}
	else{  //if mystep2 is not in the current directory, then scan the path for it.
		
		char * temp_path;
		temp_path = getenv ("PATH");
		if (temp_path!=NULL){
			std::string path = std::string(temp_path);
			//have the path variable.  will scan for the mystep2 program.
			
			
			
			size_t found;
			found = path.find(':');
			while (found!=std::string::npos) {  //if found the delimiter ':'
				std::string path_to_detect = path.substr(0,found);  //the part of the path to scan for mystep2
				path = path.substr(found+1,path.length()-found);    // the remainder of the path.  will scan later.
				found = path.find(':');                             // get the next indicator of the ':' delimiter.
				
				
				if ( stat( path_to_detect.append("/step2").c_str() , &filestatus)==0){
					//found the mystep2 program!
					
					Prefs[0].step2location = path_to_detect;
					found_step2_program = true;
					break;
					
				}  
				else{
					//did not find it yet...	
				}
			}// re:while
		}//re: if temp_path!=null
		else{ //the path variable was null.  WTF?
			
			
		}
		
		
	}
	
	if (found_step2_program == false){
		std::string path_to_detect;
		std::cout << "step2 program not found in current directory or PATH.\nplease supply the path to step2.\nabsolute path is best, but relative works, too.\n";
		std::cin >> path_to_detect;
		std::string temp_path = path_to_detect;
		while (stat(temp_path.append("/mystep2").c_str(),&filestatus) !=0) {
			std::cout << "step2 program not found at that location.  please supply the path to step2:\n";
			std::cin >> path_to_detect;	
			temp_path = path_to_detect;
		}
		Prefs[0].step2location = path_to_detect;
	}
	
	std::cout << "step2 program is located at '" << Prefs[0].step2location << "'\n";
	return 0;
}//    re: find_program_step2








//the main choice function for paramotopy.
int GetUserChoice(){
	
	std::stringstream menu;
	
	menu << "\n\nYour choices : \n\n"
	<< "1) Parse an appropriate input file. \n"
	<< "2) Randomize start points. \n"
	<< "3) Save random start points. \n"
	<< "4) Load random start points. \n"
	<< "5) Write Step 1.\n"
	<< "6) Write and Run Step 1.\n"
	<< "7) Run Step 2.\n"
    << "8) Gather Failed Path Data.\n"
	<< "\n"
	<< "9) Preferences.\n"
	<< "*\n"
	<< "0) Quit the program.\n\n"
	<< "Enter the integer value of your choice : ";
	
	int intChoice = get_int_choice(menu.str(), 0, 9);//= -1;
	
	return intChoice;
}  //   re: getuserchoice







void TryToRecoverPrevRun(std::vector< std::pair<double,double> > & RandomValues,
						 std::string filename,
						 std::string base_dir,
						 preferences *Prefs,
						 OPTIONS & currentChoice,
						 std::vector<std::string> ParamStrings){

bool finished;
std::ifstream fin;
//check out previous runs...
std::string blank;
std::stringstream commandss;

std::string lastoutfilename0 = base_dir;
lastoutfilename0.append("/step2/lastnumsent0");	  
// query whether had run or not yet. 
fin.open(lastoutfilename0.c_str());
std::vector< int > lastnumsent0;
lastnumsent0.push_back(0);
int tmpint;
if (fin.is_open()){
	int lastoutcounter=1;
	for (int i=1; i<Prefs[0].numprocs; ++i) {
		getline(fin,blank);
		if (blank=="") {
			break;
		}
		commandss << blank;
		commandss >> tmpint;
		lastnumsent0.push_back(tmpint);
		lastoutcounter++;
		commandss.clear();
		commandss.str("");
	}
	
}
fin.close();

std::string lastoutfilename1 = base_dir;
lastoutfilename1.append("/step2/lastnumsent1");	  
// query whether had run or not yet. 
fin.open(lastoutfilename1.c_str());
std::vector< int > lastnumsent1;
lastnumsent1.push_back(0);
if (fin.is_open()){
	int lastoutcounter=1;
	for (int i=1; i<Prefs[0].numprocs; ++i) {
		getline(fin,blank);
		if (blank=="") {
			break;
		}
		commandss << blank;
		commandss >> tmpint;
		lastnumsent1.push_back(tmpint);
		lastoutcounter++;
		commandss.clear();
		commandss.str("");
	}
	
}
fin.close();	



	finished = test_if_finished(base_dir);

//	std::string finishedfile = "bfiles_";
//	finishedfile.append(filename);
//	finishedfile.append("/finished");
//	fin.open(finishedfile.c_str());
//	if (fin.is_open()){
//		finished = true;
//	}
//	fin.close();
//	
//	
if (finished) {
	int tmpint = -1;
	
	while ( (std::cout << "\n\n\nthis run appears done.  proceed anyway?  1 yes, 0 no.\n: ") &&
		   ( !(std::cin >> tmpint) || tmpint < 0 || tmpint > 1) ){
		
		std::cout << "Not a valid choice -- try again please:";
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
	
	if (tmpint==0) {
		currentChoice=Quit;
		finished = true;
	}
	else {
		finished = false;
	}
	
	
}		


//if a previous run is recoverable...
if ( (( int(lastnumsent1.size()) ==Prefs[0].numprocs) || ( int(lastnumsent0.size())==Prefs[0].numprocs))  && !finished ) {
	
	std::cout << "detected a previous run, which is recoverable.\nload previous random values?\n1 yes, 0 no.\n: ";
	int loadiemcloadster = -1;
	while ((loadiemcloadster<0) || (loadiemcloadster>1)) {
		std::cin >> loadiemcloadster;
	}
	
	if (loadiemcloadster==1) {
		std::string randfilename=base_dir;
		randfilename.append("/randstart");
		
		fin.open(randfilename.c_str());
		
		int ccount=0;
		std::cout << "\n\n";
		std::string mytemp;
		while(getline(fin,mytemp)){
			std::stringstream myss;
			myss << mytemp;
			double crandreal;
			double crandimaginary;
			myss >> crandreal;
			myss >> crandimaginary;
			RandomValues[ccount].first = crandreal;
			RandomValues[ccount].second = crandimaginary;
			std::cout << ParamStrings[ccount]
			<< " = "
			<< RandomValues[ccount].first 
			<< " + "
			<< RandomValues[ccount].second
			<< "*I\n";
			++ccount;
		}
		std::cout << "\n";
		fin.close();
	}
}

}


bool test_if_finished(std::string base_dir){
	
	bool finished = false;
	
	std::string finishedfile = base_dir;
	finishedfile.append("/finished");
	struct stat filestatus;
	
	if (stat( finishedfile.c_str(), &filestatus ) ==0){  // if it can see the 'finished' file
		finished = true;
	}
	return finished;
}


	
