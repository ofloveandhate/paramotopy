#include "preferences.h"


//int GetPrefVersion(){
//	std::ifstream versionstream;
//	std::string homedir = getenv("HOME");
//	std::string versionlocation;
//	versionlocation = homedir;
//	versionlocation.append("/.poly.prefsversion");
//	versionstream.open(versionlocation.c_str());
//	int version;
//	if (versionstream.is_open()) {
//		versionstream >> version;
//	}
//	else {
//		version = 0;
//	}
//	
//	versionstream.close();
//	return version;
//}
//
//void SetPrefVersion(int version){
//	std::ofstream versionstream;
//	std::string homedir = getenv("HOME");
//	std::string versionlocation;
//	versionlocation = homedir;
//	versionlocation.append("/.poly.prefsversion");
//	versionstream.open(versionlocation.c_str());
//	versionstream << version;
//	versionstream.close();
//	
//}


//bool DeterminePreferences(preferences *Prefs, bool rerun, std::vector< bool > & FilePrefVector){
//	bool parallel;
//	std::string tmp, preflocation;
//	std::stringstream ss, menustream;
//	std::ifstream prefstream;
//	std::string homedir = getenv("HOME");
//	preflocation = homedir;
//	preflocation.append("/.poly.prefs");
//	prefstream.open(preflocation.c_str());
//	
//	int numfilespossible = 7; //now 7; was 8.  changed when made failed paths mandatory	
//	std::vector< std::string > FileNames;
//	FileNames.resize(numfilespossible);
//	FileNames[0] = "real_solutions";
//	FileNames[1] = "nonsingular_solutions";
//	FileNames[2] = "singular_solutions";
//	FileNames[3] = "raw_data";
//	FileNames[4] = "raw_solutions";
//	FileNames[5] = "main_data";
//	FileNames[6] = "midpath_data";
//	//	FileNames[7] = "failed_paths";
//	
//	for (int i=0; i<numfilespossible; ++i) {
//		FilePrefVector[i] = false;
//	}	
//	
//	int makefreshprefs = 0;
//	
//	if (!prefstream.is_open() || rerun) {
//		makefreshprefs=1;
//	}
//	else {
//		getline(prefstream,tmp);
//		if (prefstream.eof()) {
//			makefreshprefs=1;
//		}
//	}
//	
//	if (makefreshprefs==1) {
//		prefstream.close();
//		std::ofstream outprefstream;
//		outprefstream.open(preflocation.c_str());
//		
//		// get info on whether to run parallel or not
//		
//		std::cout << "Specify preferences for parallel on this machine.\n"
//		<< "This creates a file at $HOME/.poly.prefs\n";
//		
//		parallel = false;
//		
//		
//		
//		menustream << "Select your command for running parallel processes:\n"
//		<< "0) mpiexec\n"
//		<< "1) aprun\n";
//		//<< "2) other... (not a functioning selection yet)\n:";		
//		
//		Prefs[0].architecture = get_int_choice(menustream.str(), 0, 1);
//		outprefstream << Prefs[0].architecture << "\n";
//		menustream.clear();
//		menustream.str("");
//		
//		
//		
//		menustream << "Run Parallel?\n"
//		<< "0) No.\n"
//		<< "1) Yes.\n"
//		<<  "Enter choice : ";
//		int parselect = get_int_choice(menustream.str(), 0, 1);
//		
//		if (parselect==0)
//			parallel=false;
//		else
//			parallel=true;
//		
//		
//		if (parallel){
//			Prefs[0].usemachine = get_int_choice("Does your machine use a machinefile?\n0) No.\n1) Yes.\n: ",0,1);
//			
//			if (Prefs[0].usemachine==1) {
//				
//				
//				std::cout << "Enter the machine file to be used, relative to your home directory: ";
//				std::cin >> Prefs[0].machinefile;
//			}
//			
//			Prefs[0].numprocs = get_int_choice( "Enter the number of processes to be run: ",0,9000000);
//		}
//		outprefstream << parselect << "\n";
//		if (parallel){
//			outprefstream << Prefs[0].usemachine << " " << Prefs[0].machinefile << "\n"
//			<< Prefs[0].numprocs << "\n";
//			
//			
//			Prefs[0].numfilesatatime = get_int_choice("Enter step2 number of files at a time per processor: ",0,9000000);
//			
//			outprefstream << Prefs[0].numfilesatatime << "\n";
//			
//			
//		}
//		else {
//			Prefs[0].numfilesatatime = 1;
//		}
//		
//		//put a test for /dev/shm here, to make the next input conditional
//		
//		struct stat filestatus;
//		if (stat( "/dev/shm", &filestatus ) ==0){
//			Prefs[0].devshm = get_int_choice("Would you like to use /dev/shm for temp files?\n0) No.\n1) Yes.\n:",0,1);		
//		}
//		else
//		{
//			//std::cout << "/dev/shm not detected.  specify another location?";
//			Prefs[0].devshm = 0;
//		}
//		
//		outprefstream << Prefs[0].devshm << "\n";
//		
//		Prefs[0].stifle = get_int_choice("Stifle step2 output (to /dev/null) ?\n0) No.\n1) Yes.\n:",0,1);	
//		outprefstream << Prefs[0].stifle << "\n";
//		
//		
//		std::cout << "Select the files to save.\n";
//		int selection = 0;
//		while (selection!=numfilespossible){
//			for (int j = 0; j < numfilespossible;++j){
//				if (!FilePrefVector[j]){
//					std::cout << j << ")" << FileNames[j] << "\n";
//				}
//			}
//			std::cout << numfilespossible << ") Done.\n";
//			
//			
//			selection = get_int_choice("Select the file you would like to save : ",0,numfilespossible);
//			
//			
//			if (selection!=numfilespossible){ 
//				std::cout << "Selection = " << selection
//				<< ", setting saved to true.\n";
//				FilePrefVector[selection]=true;
//			}
//		}
//		//done setting files to save for this run.
//		
//		
//		for (int i=0; i<numfilespossible; ++i) {
//			//			if (TheFiles[i].saved==true) {
//			outprefstream << (FilePrefVector[i]? 1 : 0) << (i != numfilespossible?" ":"\n");
//			//			}
//		}
//		
//		
//		//		std::cout << "Save progress every ? iterations: ";
//		Prefs[0].saveprogresseverysomany = 1;
//		
//		Prefs[0].newfilethreshold = get_int_choice("Threshold in bytes for new data file \n (press 0 for default of 67108864 = 2^26): ",0,2147483647);
//		if (Prefs[0].newfilethreshold == 0) {
//			Prefs[0].newfilethreshold = 67108864;
//		}
//		
//		outprefstream << "\n" << Prefs[0].newfilethreshold << "\n";
//		
//		outprefstream.close();
//		
//	}//re: if !prefstream.isopen
//	else {
//		
//		ss << tmp; //would be holding over from before the if statement.
//		ss >> Prefs[0].architecture;
//		ss.clear();
//		ss.str("");
//		
//		int parselect;
//		getline(prefstream,tmp);
//		ss << tmp;
//		ss >> parselect;
//		ss.clear();
//		ss.str("");
//		if (parselect==0) {
//			parallel=false;
//		}
//		else
//		{
//			parallel = true;
//			getline(prefstream,tmp);
//			ss << tmp;
//			ss >> Prefs[0].usemachine;
//			if (Prefs[0].usemachine==1) {
//				ss >> Prefs[0].machinefile;
//			}
//			ss.clear();
//			ss.str("");
//			getline(prefstream,tmp);
//			ss << tmp;
//			ss >> Prefs[0].numprocs;
//			ss.clear();
//			ss.str("");
//			
//			getline(prefstream,tmp);
//			ss << tmp;
//			ss >> Prefs[0].numfilesatatime;
//			ss.clear();
//			ss.str("");
//		}
//		
//		Prefs[0].saveprogresseverysomany = 1;
//		
//		getline(prefstream,tmp);
//		ss << tmp;
//		ss >> Prefs[0].devshm;
//		ss.clear();
//		ss.str("");
//		
//		getline(prefstream,tmp);
//		ss << tmp;
//		ss >> Prefs[0].stifle;
//		ss.clear();
//		ss.str("");
//		
//		getline(prefstream,tmp);
//		ss << tmp;
//		int tmpint;
//		std::cout << "saving these files: ";
//		
//		
//		for (int i=0; i<numfilespossible; ++i) {
//			ss >> tmpint;
//			if (tmpint==1){
//				FilePrefVector[i]=true;
//				std::cout << FileNames[i] << " "; 
//			}
//		}
//		std::cout << "\n";		
//		
//		ss.clear();
//		ss.str("");
//		getline(prefstream,tmp);
//		ss << tmp;
//		ss >> Prefs[0].newfilethreshold;
//		ss.clear();
//		ss.str("");
//		
//		prefstream.close();
//		
//		
//	}//re: if prefstream.isopen
//	
//	
//	
//	
//	
//	//a few things to wrap up preference reading.
//	
//	if (parallel) {
//		if (Prefs[0].usemachine) {
//			homedir.append("/");
//			homedir.append(Prefs[0].machinefile);
//			Prefs[0].machinefile = homedir;
//		}
//		std::cout << "running parallel.\nmachine file: " << Prefs[0].machinefile << "\n" 
//		<< "using /dev/shm: " << Prefs[0].devshm << "\n"
//		<< "num processors: " << Prefs[0].numprocs << "\n" 
//		<< "num files per proc: " << Prefs[0].numfilesatatime << "\n"
//		<< "newfilethreshold: " << Prefs[0].newfilethreshold << "\n\n\n\n";
//	}
//	else {
//		Prefs[0].numprocs = 1;
//		Prefs[0].numfilesatatime = 1;	
//		
//		std::cout <<"serial setup on this machine.\n" 
//		<< "using /dev/shm: " << Prefs[0].devshm << "\n"
//		<< "num processors, numfilesatatime set to 1.\n\n\n\n";
//	}
//	
//	
//	
//	
//	
//	
//	return parallel;
//}


