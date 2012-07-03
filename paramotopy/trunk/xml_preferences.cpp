#include "xml_preferences.h"
 
const char * const ProgSettings::possible_savefiles[NUMPOSSIBLE_SAVEFILES] = 
{ "real_solutions", "nonsingular_solutions", "singular_solutions", "raw_data", "raw_solutions","main_data","midpath_data"   };

const char * const ProgSettings::mandatory_savefiles[NUMMANDATORY_SAVEFILES] = 
{ "failed_paths" };



void ProgSettings::default_basic_bertini_values_stepone(){
	
	
	
	setValue("Step1Settings","TRACKTOLBEFOREEG",1e-4);
	setValue("Step1Settings","TRACKTOLDURINGEG",1e-5);
	setValue("Step1Settings","FINALTOL",1e-6);
	setValue("Step1Settings","PRINTPATHMODULUS",20);
	setValue("Step1Settings","IMAGTHRESHOLD",1e-4);
	setValue("Step1Settings","SECURITYLEVEL",0);
	ProgSettings::save();
	return;

}


void ProgSettings::default_basic_bertini_values_steptwo(){
	
	
	setValue("Step2Settings","TRACKTOLBEFOREEG",1e-4);
	setValue("Step2Settings","TRACKTOLDURINGEG",1e-5);
	setValue("Step2Settings","FINALTOL",1e-6);
	setValue("Step2Settings","PRINTPATHMODULUS",20);
	setValue("Step2Settings","IMAGTHRESHOLD",1e-4);
	setValue("Step2Settings","SECURITYLEVEL",0);
	ProgSettings::save();
	return;
	
	
}



//locates the step2program.
void ProgSettings::FindProgramStep2(){
	
	bool found_step2_program = false;
	struct stat filestatus;
	

	if (haveSetting("MainSettings","step2location")) {
		std::string stored_location = settings["MainSettings"]["step2location"].value();
		stored_location.append("/");
		if (stat(stored_location.append("step2").c_str(),&filestatus) == 0){
			found_step2_program = true;
		}
	}

	if (found_step2_program == false ){
		if (stat( "./step2", &filestatus ) ==0){  // if have the step2 program in current directory, set location of it to here.
			setValue("MainSettings","step2location",".");
			found_step2_program = true;
		}
		else{  //if step2 is not in the current directory, then scan the path for it.
			
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
						
						setValue("MainSettings","step2location",path_to_detect);
						found_step2_program = true;
						break;
						
					}  
					else{
						//did not find it yet...	
					}
				}// re:while
			}//re: if temp_path!=null
			else{ //the path variable was null.  WTF?
				std::cout << "unable to scan $PATH for step2.\n";
				
			}
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
		setValue("MainSettings","step2location",path_to_detect);
	}
	
	std::cout << "step2 program is located at '" << settings["MainSettings"]["step2location"].value() << "'\n";
	return;
}//    re: find_program_step2







//gets files to save from user.  also sets to save all mandatory files.
void ProgSettings::SetSaveFiles(){
	
	for (int j = 0; j < NUMPOSSIBLE_SAVEFILES; ++j) {
		setValue("SaveFiles",possible_savefiles[j],0);
	}
	
	std::cout << "Select the files to save.\n";
	int selection = -1;
	while (selection!=0){
		std::cout << 0 << ") Done.\n";
		for (int j = 0; j < NUMPOSSIBLE_SAVEFILES;++j){
			if (settings["SaveFiles"][possible_savefiles[j]].intvalue != 1){//!FilePrefVector[j]
				std::cout << j+1 << ")" << possible_savefiles[j] << "\n";
			}
		}

		selection = get_int_choice("Select the file you would like to save : ",0,NUMPOSSIBLE_SAVEFILES);
		
		
		if (selection!=0){ 
			std::cout << "Selection = " << selection
			<< ", setting saved to true.\n";
			setValue("SaveFiles",possible_savefiles[selection-1],1);
		}
	}
	//done setting files to save for this run.

	
	for (int j=0; j<NUMMANDATORY_SAVEFILES; ++j) {
		setValue("SaveFiles",mandatory_savefiles[j],1);
	}
	
};


//gives false bool if didn't have all the files previously noted for saving state.  true if did.
bool ProgSettings::CheckPrevSetFiles(){
	bool had_all = true;
	for (int j=0; j<NUMPOSSIBLE_SAVEFILES; ++j) {
		if (settings["SaveFiles"].find( possible_savefiles[j] ) == settings["SaveFiles"].end()) {
			had_all = false;
		}
	}
	return had_all;
};


//gets the save state for individual file, determined by user through the console
void ProgSettings::GetIndividualFileSave(std::string datafilename){
	std::stringstream ss;
	ss << "Would you like to save data contained in files named '" << datafilename << "'?\n0 no, 1 yes\n:";
	setValue("SaveFiles",datafilename, get_int_choice(ss.str(),0,1));	
}


//switching for calling specific functions to get required settings.  i did this to try to avoid a mess of if then statements.
void ProgSettings::RequiredSettingsSwitcharoo(int settingcase){

	switch (settingcase) {
		case 1:
			ProgSettings::GetArchitecture();
			break;
		case 2:
			ProgSettings::GetParallel();
			break;
			
		case 5:
			ProgSettings::GetSaveProgress();
			break;
		case 6:
			ProgSettings::GetNewFileThresh();
			break;
		case 7:
			ProgSettings::GetDevshm();
			break;
		case 8:
			ProgSettings::GetStifle();
			break;
		case 9:
			ProgSettings::GetStepTwoLocation();
			break;

		default:
			std::cout << "GetPref code not yet written for case " << settingcase << "\n"; 
			break;
	}
	return;
};


//checks whether have a set of required settings
void ProgSettings::setRequiredValues(){
	std::stringstream menustream;
	std::map<std::string,int>::iterator iter;
	
	static std::map< std::string,int > main_required_values;
	main_required_values["architecture"] = 1;
	main_required_values["parallel"] = 2;
	main_required_values["saveprogresseverysomany"] = 5;
	main_required_values["newfilethreshold"] = 6;
	main_required_values["devshm"] = 7;
	main_required_values["stifle"] = 8;
	main_required_values["step2location"] = 9;

//adding a required value here requires adding a ProgSettings::Get___() function, and adding an option to switch
	
	
//	example: checking whether a std::map has an entry of a given name: 
//	cars.find(name) != cars.end();  if name is in map cars, this is true
	for (iter=main_required_values.begin(); iter != main_required_values.end(); iter++) {
		if (settings["MainSettings"].find( (*iter).first) == settings["MainSettings"].end() ) {
			ProgSettings::RequiredSettingsSwitcharoo( (*iter).second );
		}
	}
	return;
};


//gets the parallel settings
void ProgSettings::GetParallel(){//case 2
	std::stringstream menustream;
	std::string valstr;
	menustream << "Run Parallel?\n"
	<< "0) No.\n"
	<< "1) Yes.\n"
	<<  "Enter choice : ";
	int parselect = get_int_choice(menustream.str(), 0, 1);
	
	if (parselect==0)
		setValue("MainSettings","parallel",0);
	else
		setValue("MainSettings","parallel",1);
	
	if (parselect==1){
		ProgSettings::GetMachineFile();
		ProgSettings::GetNumProcs();		
		ProgSettings::GetNumFilesTime();
	}//re: if parselect
	else{
		setValue("MainSettings","numfilesatatime",1);
	}//re: else
	return;
}


//changing/setting the archtecture for parallel calling step2 and bertini.  eventually this will be deprecated
void ProgSettings::GetArchitecture(){ //case 1
	std::string valstr;
	std::stringstream menustream;
	
	menustream << "Select your command for running parallel processes:\n"
	<< "0) mpiexec\n"
	<< "1) aprun\n"
	<< "2) other... (you will enter your own command [sadly not calling pattern yet :( ])\n:";		
	int val = get_int_choice(menustream.str(), 0, 2);
	
	switch (val) {
		case 0:
			valstr = "mpiexec";
			break;
		case 1:
			valstr = "aprun";
			break;
		case 2:
			std::cout << "enter your own command:\n";
			std::cin >> valstr;
			break;
		default:
			//how did you get here?
			break;
	}
	
	setValue("MainSettings","architecture",valstr);	
	
	return;
};

void ProgSettings::GetMachineFile(){
	std::string valstr;
	int valint = get_int_choice("Does your machine use a machinefile?\n0) No.\n1) Yes.\n: ",0,1);
	setValue("MainSettings","usemachine",valint);
	if (valint==1) {
		
		
		std::cout << "Enter the machine file to be used, relative to your home directory: ";
		std::cin >> valstr;
		
		setValue("MainSettings","machinefile",valstr);		
	}
	return;
};

void ProgSettings::GetNumProcs(){
	
	setValue("MainSettings","numprocs",get_int_choice( "Enter the number of processes to be run: ",0,9000000));
	return;	
};

void ProgSettings::GetNumFilesTime(){
		
	int valint = get_int_choice("Enter step2 number of files at a time per processor: ",0,9000000);		
	setValue("MainSettings","numfilesatatime",valint);
	return;	
};

void ProgSettings::GetSaveProgress(){
	
	std::stringstream menustream;
	
	setValue("MainSettings","saveprogresseverysomany",1);
	return;	
};

void ProgSettings::GetNewFileThresh(){

	std::stringstream menustream;
	int val = get_int_choice("Threshold in bytes for new data file \n (press 0 for default of 67108864 = 2^26): ",0,2147483647);
	if (val == 0) {
		setValue("MainSettings","newfilethreshold",67108864);
	}	
	else{
		setValue("MainSettings","newfilethreshold",val);
	}
	return;	
};


//detects whether /dev/shm is a location, and if so, offers to use /dev/shm for the temporary files.
void ProgSettings::GetDevshm(){
	std::stringstream menustream;
	
	struct stat filestatus;
	if (stat( "/dev/shm", &filestatus ) ==0){
		setValue("MainSettings","devshm", get_int_choice("Would you like to use /dev/shm for temp files?\n0) No.\n1) Yes.\n:",0,1));		
	}
	else
	{
		setValue("MainSettings","devshm",0);
		//std::cout << "/dev/shm not detected.  specify another location?";
	}
	return;	
};


//offers to use /dev/null to redirect step2 output
void ProgSettings::GetStifle(){
	std::stringstream menustream;
	
	setValue("MainSettings","stifle",get_int_choice("Stifle step2 output (to /dev/null) ?\n0) No.\n1) Yes.\n:",0,1));
	return;	
};


//
void ProgSettings::GetStepTwoLocation(){
	std::stringstream menustream;
	
	
	setValue("MainSettings","step2location",".");
	return;	
};


//the main function to save the preferences to a xml file.
void ProgSettings::save(){
	
	TiXmlDocument* doc = new TiXmlDocument;  
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );  
	
	doc->LinkEndChild( decl );

	TiXmlElement * root = new TiXmlElement("paramotopy_preferences");  
	categorymap::iterator iter;
	
	//save each catergory
	for (iter=settings.begin(); iter!= settings.end(); iter++) {
		ProgSettings::SaveCategoryToXml( (*iter).first , (*iter).second ,root);
	} 

	//wrap it up
	doc->LinkEndChild( root ); 
	
	//save it
	if(doc->SaveFile(filename.c_str())){  //filename is a data member of ProgSettings class
//		std::cout << "preferences saved\n";
	}
	else{
		//couldn't save for some reason.  this may be a problem on queued systems?
		std::cout << "preferences *failed* to save to " << 	filename << "!\n";
	};  

}



//saves one category of settings to the xml file.  returns to calling function save() for next category or finalization.
int ProgSettings::SaveCategoryToXml(std::string catname, settingmap curr_settings , TiXmlElement* root){
	
	TiXmlText* setting_name;
	TiXmlText* setting_value;
	TiXmlText* setting_type;
	
	std::stringstream ss;
	

	TiXmlElement * category_name = new TiXmlElement( catname.c_str() );  
	
	settingmap::iterator iter;
	for (iter=curr_settings.begin(); iter != curr_settings.end(); iter++){
		TiXmlElement* setting_elmnt = new TiXmlElement("setting");
		
		const std::string & key=(*iter).first;
		const std::string & type=(*iter).second.typestr();
		const std::string & value=(*iter).second.value();
		
//		std::cout << "writing setting " << key << ", " 
//										<< value << ", " 
//										<< type << "\n";
		
		
		setting_name = new TiXmlText(key.c_str());
		setting_value = new TiXmlText(value.c_str());
		setting_type = new TiXmlText(type.c_str());
				
		
		TiXmlElement* name_elmnt = new TiXmlElement("name");
		name_elmnt->LinkEndChild(setting_name);
		TiXmlElement* value_elmnt = new TiXmlElement("value");
		value_elmnt->LinkEndChild(setting_value);	
		TiXmlElement* type_elmnt = new TiXmlElement("type");
		type_elmnt->LinkEndChild(setting_type);
		
		setting_elmnt->LinkEndChild(name_elmnt ); 
		setting_elmnt->LinkEndChild(value_elmnt ); 
		setting_elmnt->LinkEndChild( type_elmnt);
		
		
		category_name->LinkEndChild(setting_elmnt); 
		
		
		}
		root->LinkEndChild(category_name);
	
	return 0;
}

		
		


//attempts to open the pFilename.  if cannot, checks for required values.  also goes through reset if xml file is broken.
void ProgSettings::load(const char* pFilename){

	filename = pFilename;  //changes this data member of the ProgSettings variable
	std::cout << "loading preferences from " << filename << "\n";
	TiXmlDocument* doc = new TiXmlDocument(pFilename);
	
	settings["MainSettings"].clear();
	settings["Step1Settings"].clear();
	settings["Step2Settings"].clear();
	settings["PathFailure"].clear();
	settings["SaveFiles"].clear();
	
	
	bool doc_loaded = doc->LoadFile();
	if (!doc_loaded){
		std::cout << "no prefs file: " << pFilename << " found.  setting preferences to defaults.\n";
		ProgSettings::default_basic_bertini_values_stepone();
		ProgSettings::default_basic_bertini_values_steptwo();
		ProgSettings::default_path_failure_settings();
	}
	else {
		TiXmlHandle hDoc(doc);
		TiXmlElement* pElem;
		TiXmlHandle hRoot(0);
		

		pElem=hDoc.FirstChildElement().Element();
		// should always have a valid root but handle gracefully if it doesn't

		if (!pElem) {
			std::cout << "bad xml file for prefs.  :( todo: check for backup\n";
		}
		else{
			std::string main_name =pElem->Value();  //unused?
			
			hRoot=TiXmlHandle(pElem);  // the handle for the data we will be reading

			TiXmlElement* catElem=hRoot.FirstChild().Element();
			for (catElem; catElem; catElem=catElem->NextSiblingElement()) {
//				std::cout << "reading prefs category: " << catElem->Value() << "\n";
				ProgSettings::ReadCategoryFromXml(catElem->Value(),hRoot);
			}
		}
	}
	
	ProgSettings::setRequiredValues();  //check for required settings, and set them if not found already.
	if (!ProgSettings::CheckPrevSetFiles()) {
		ProgSettings::SetSaveFiles();
	}
	ProgSettings::FindProgramStep2();

}

//for reading individual settings categories from the xml file.
int ProgSettings::ReadCategoryFromXml(std::string catname, TiXmlHandle hRoot){ 
	
	std::stringstream ss;
	int type;
	int intvalue;
	double doubvalue;
	
	TiXmlElement* pElem=hRoot.FirstChild( catname ).FirstChild().Element();
	for( pElem; pElem; pElem=pElem->NextSiblingElement())
	{
		TiXmlHandle setting_handle(pElem);  //get the handle for the setting
		
	
		TiXmlElement* qElem = setting_handle.FirstChild("name").Element();  //read the name
		const char *nameText=qElem->GetText();

		qElem = setting_handle.FirstChild("value").Element();  //read the value
		const char *valueText=qElem->GetText();
		
		qElem = setting_handle.FirstChild("type").Element();  //read the type
		const char *typeText=qElem->GetText();
		
		
		if (nameText && valueText && typeText)
		{ //setting is ok.  set the setting.
			
//			std::cout << "    " << catname << " setting " << nameText << " to value " << valueText << " with type " << typeText << "\n";

			ss << typeText;
			ss >> type;
			ss.clear();
			ss.str("");
			
			//according to type integer (0=string, 1=integer, 2=double), store in the setting map
			switch (type) {
				case 0: //string
					ProgSettings::setValue(catname,nameText,valueText);
					break;
					
				case 1: //integer
					ss << valueText;
					ss >> intvalue;
					ss.clear();
					ss.str("");
					ProgSettings::setValue(catname,nameText,intvalue);
					break;
										   
				case 2: //double
					ss << valueText;
					ss >> doubvalue;
					ss.clear();
					ss.str("");
					ProgSettings::setValue(catname,nameText,doubvalue);
				  break;
										  					  
				default:
					std::cout << "bad type set somehow, from xml prefs file.\n";
					break;
			}
			
		}
	}

	return 0;
}

void ProgSettings::MainMenu(){
	std::stringstream menu;
	int choice = -1;
	menu << "\n\n\nPreferences Main Menu:\n\n"
			  << "1) step1 bertini settings\n"
	          << "2) step2 bertini settings\n"
			  << "3) path failure resolution\n"
			  << "4) Parallelism\n"
			  << "5) data saving\n"
			  << "*\n0) return to paramotopy\n"
			  << "\n: ";
	
	while (choice!=0) {
		choice = get_int_choice(menu.str(),0,5);
		
		switch (choice) {
			case 0:
				std::cout << "returning to paramotopy\n\n\n";
				break;
				
			case 1:
				ProgSettings::StepOneMenu();
				break;
				
			case 2:
				ProgSettings::StepTwoMenu();
				break;
				
			case 3:
				ProgSettings::PathFailureMenu();
				break;
				
			case 4:
				ProgSettings::ParallelismMenu();
				break;
				
			case 5:
				ProgSettings::SaveFilesMenu();
				break;
				
			default:
				std::cout << "somehow an unacceptable entry submitted :(\n";
				break;
		}
		ProgSettings::save();
	}
	return;
}

//submenu for settings.  this one for settings related to parallel
void ProgSettings::ParallelismMenu(){
	
	std::stringstream menu;
	int choice = -10;
	
	menu << "\n\nParallelism:\n\n"
		<< "1) Switch Parallel On/Off, and consequential others\n"
		<< "2) Number of Files to send to workers at a time\n"
		<< "3) Machinefile\n"
		<< "4) Architecture / calling\n"
		<< "5) Number of processors used\n"
		<< "6) Stifle Step2 Output\n"
		<< "7) Use /dev/shm for temp files\n"
		<< "*\n"
		<< "0) go back\n"
		<< "\n: ";
	
	while (choice!=0) {
		ProgSettings::DisplayCurrentSettings("MainSettings");
		choice = get_int_choice(menu.str(),0,7);
		
		switch (choice) {
			case 0:
				break;
				
			case 1:
				ProgSettings::GetParallel();
				break;
				
			case 2:
				ProgSettings::GetNumFilesTime();
				break;
				
			case 3:
				ProgSettings::GetMachineFile();
				break;
				
			case 4:
				ProgSettings::GetArchitecture();
				break;
				
			case 5:
				ProgSettings::GetNumProcs();
				break;
				
			case 6:
				ProgSettings::GetStifle();
				break;
			case 7:
				ProgSettings::GetDevshm();
				break;
			default:
				std::cout << "somehow an unacceptable entry submitted :(\n";
				break;
		}
		ProgSettings::save();
	}
	return;
}

//submenu, for step 1 solve settings.
void ProgSettings::StepOneMenu(){
	std::stringstream menu;
	
	menu << "\n\nBasic Step1 Settings:\n\n"
	<< "1) Change Setting\n"
	<< "2) Remove Setting\n"
	<< "3) Add Setting\n"
	<< "4) Reset to Default Settings\n"
	<< "*\n"
	<< "0) Go Back\n"
	<< "\n: ";
	int choice = -1001;
	while (choice!=0) {
		ProgSettings::DisplayCurrentSettings("Step1Settings");
		
		choice = get_int_choice(menu.str(),0,4);
		
		switch (choice) {
			case 0:
				break;
				
			case 1:
				ProgSettings::ChangeSetting("Step1Settings");
				break;
				
			case 2:
				ProgSettings::RemoveSetting("Step1Settings");
				break;
				
			case 3:
				ProgSettings::AddSetting("Step1Settings");
				break;
				
			case 4:
				settings["Step1Settings"].clear();
				ProgSettings::default_basic_bertini_values_stepone();
				break;
				
			default:
				std::cout << "somehow an unacceptable entry submitted :(\n";
				break;
		}
		ProgSettings::save();
	}
	
	
	return;
}


//sets options for step2 bertini whatnot
void ProgSettings::StepTwoMenu(){
	
	std::stringstream menu;
	menu << "\n\nBasic Step2 Settings:\n\n"
	
	<< "1) Change Setting\n"
	<< "2) Remove Setting\n"
	<< "3) Add Setting\n"
	<< "4) Reset to Default Settings\n"
	<< "*\n"
	<< "0) Go Back\n"
	<< "\n: ";
	int choice = -1001;
	while (choice!=0) {
		ProgSettings::DisplayCurrentSettings("Step2Settings");
		
		choice = get_int_choice(menu.str(),0,4);
		
		switch (choice) {
			case 0:
				break;
				
			case 1:
				ProgSettings::ChangeSetting("Step2Settings");
				break;
				
			case 2:
				ProgSettings::RemoveSetting("Step2Settings");
				break;
				
			case 3:
				ProgSettings::AddSetting("Step2Settings");
				break;
				
			case 4:
				settings["Step2Settings"].clear();
				ProgSettings::default_basic_bertini_values_steptwo();
				break;
				
			default:
				std::cout << "somehow an unacceptable entry submitted :(\n";
				break;
		}
		ProgSettings::save();
	}
	return;	
}



	
//sets the saving of output files from bertini in the step2 process.
void ProgSettings::SaveFilesMenu(){
	
	std::stringstream menu;
	menu << "\n\nPath Failure:\n\n"
	<< "1) Change Files to Save\n"
	<< "*\n"
	<< "0) Go Back\n"
	<< "\n: ";
	int choice = -1001;
	while (choice!=0) {
		ProgSettings::DisplayCurrentSettings("SaveFiles");
		choice = get_int_choice(menu.str(),0,1);
		switch (choice) {
			case 0:
				break;
				
			case 1:
				ProgSettings::SetSaveFiles();
				break;
				
			default:
				break;
		}
	}
	

	ProgSettings::save();
	return;
}


void ProgSettings::ChangeSetting(std::string category_name){
	//first, get setting name
	std::string setting_name = "neverusethisname";
	
	while (!haveSetting(category_name,setting_name) ) {
		std::cout << "what is the exact name (caps,spelling) of setting?\n: ";
		std::cin >> setting_name;
	}
	
	int typeint = get_int_choice("what is the type of the setting? \n string: 0\n integer / 0-1 bool: 1\n float/double: 2\n\n:",0,2);
	std::cout << "new value\n:";
	std::string newvalue;
	std::cin >> newvalue;
	std::stringstream converter;
	int intie;
	double doubie;
	switch (typeint) {
		case 0:
			setValue(category_name,setting_name,newvalue);
			break;
		case 1:
			converter << newvalue;
			converter >> intie;
			setValue(category_name,setting_name,intie);
			break;
		case 2:
			converter << newvalue;
			converter >> doubie;
			setValue(category_name,setting_name,doubie);
			break;
		default:
			break;
	}
	ProgSettings::save();
}


// adds a setting to category_name.  prompts the user for setting name, type, and value.
void ProgSettings::AddSetting(std::string category_name){
	//first, get setting name
	std::string setting_name = "neverusethisname";
	

	std::cout << "what is the exact name (caps,spelling) of setting?\n: ";
	std::cin >> setting_name;

	std::stringstream prompt;
	prompt << "what is the type of"
		<< setting_name
		<< "?\n 0 = string\n 1 = integer / 0-1 bool\n 2= float/double\n\n:";
	int typeint = get_int_choice(prompt.str(),0,2);
	
	std::cin.clear();
	std::cin.ignore( 1000, '\n' );
	
	std::cout << "enter setting value" << std::endl <<  ": ";
	
	char newvalue[256];
	std::cin.getline(newvalue,256,'\n');

	std::stringstream converter;
	converter << newvalue;
	int intie;
	double doubie;
	switch (typeint) {
		case 0:
			setValue(category_name,setting_name,converter.str());
			break;
		case 1:
			converter >> intie;
			setValue(category_name,setting_name,intie);
			break;
		case 2:
			converter >> doubie;
			setValue(category_name,setting_name,doubie);
			break;
		default:
			break;
	}
	ProgSettings::save();
	return;
}


//removes a setting from a category.  
void ProgSettings::RemoveSetting(std::string category_name){
	std::string setting_name = "neverusethisname";
	
	while (!haveSetting(category_name,setting_name) ) {
		
		std::cout << "what is the exact name (caps,spelling) of setting?";
		std::cin >> setting_name;
	}
	
	settings[category_name].erase(setting_name);
	
	ProgSettings::save();
	return;
}


//for a category_name, writes setting names and values to the screen

void ProgSettings::DisplayCurrentSettings(std::string category_name){
	
	settingmap::iterator iter;
	std::cout << "\n\n" << category_name << " current settings:\n"
		<< "-name-";
	for (int i=0; i<24; ++i) {
		std::cout << " ";
	}
	std::cout << "-value-\n";
	for (iter=settings[category_name].begin(); iter != settings[category_name].end(); iter++){
		std::string setting_name = (*iter).first;
		std::cout << setting_name;
		for (int i=0; i<(30-int(setting_name.length())); ++i) {
			std::cout << " ";
		}
		std::cout << settings[category_name][(*iter).first].value() << "\n";
	}

	return;
}




///////////////////////////////////////
//
//     path failure
//
//////////////////////////////////////

//for choosing options for dealing with path failure
void ProgSettings::PathFailureMenu(){
	std::stringstream menu;
	menu << "\n\nPath Failure:\n\n"
	<< "1) Choose random-start-point Method\n"
	<< "2) Use security level 1\n"
	<< "3) Tolerance Tightening\n"
	<< "7) Reset to Default Settings\n"
	<< "*\n"
	<< "0) Go Back\n"
	<< "\n: ";
	int choice = -1001;
	while (choice!=0) {
		ProgSettings::DisplayCurrentSettings("PathFailure");
		choice = get_int_choice(menu.str(),0,7);
		
		switch (choice) {
			case 0:
				std::cout << "going back\n";
				break;
				
			case 1:
				ProgSettings::GetRandomMethod();
				break;
				
			case 2:
				ProgSettings::GetSecurityLevelPathFailure();
				break;
				
			case 3:
				ProgSettings::GetTightenTolerancesPathFailure();
				break;
				
			case 7:
				settings["PathFailure"].clear();
				ProgSettings::default_path_failure_settings();
				break;
			default:
				std::cout << "todo: finish this section\n";
				break;
		}
	}
	
	
	ProgSettings::save();
	return;
}

void ProgSettings::default_path_failure_settings(){
	
	setValue("PathFailure","newrandommethod",1);
	setValue("PathFailure","turnon_securitylevel1",0);
	setValue("PathFailure","tightentolerances",1);
	
	ProgSettings::save();
	return;
}

void ProgSettings::GetRandomMethod(){

}

void ProgSettings::GetSecurityLevelPathFailure(){

}

void ProgSettings::GetTightenTolerancesPathFailure(){

}

