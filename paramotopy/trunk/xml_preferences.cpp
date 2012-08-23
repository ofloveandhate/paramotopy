#include "xml_preferences.hpp"
 
const char * const ProgSettings::possible_savefiles[NUMPOSSIBLE_SAVEFILES] = 
{ "real_solutions", "nonsingular_solutions", "singular_solutions", "raw_data", "raw_solutions","main_data","midpath_data"   };

const char * const ProgSettings::mandatory_savefiles[NUMMANDATORY_SAVEFILES] = 
{ "failed_paths" };



//related to path failure analysis
void ProgSettings::tightentolerances(){
	settings["PathFailureBertini"]["TRACKTOLBEFOREEG"].doubvalue = 0.1*settings["PathFailureBertini"]["TRACKTOLBEFOREEG"].doubvalue;
	settings["PathFailureBertini"]["TRACKTOLDURINGEG"].doubvalue = 0.1*settings["PathFailureBertini"]["TRACKTOLDURINGEG"].doubvalue;
	settings["PathFailureBertini"]["FINALTOL"].doubvalue = 0.1*settings["PathFailureBertini"]["FINALTOL"].doubvalue;
	return;
}



//sets path failure settings from the step2 settings
void ProgSettings::set_path_failure_settings(){
	
	settings["PathFailureBertini"].clear();
	settingmap::iterator iter;
	
	for (iter=settings["Step2Settings"].begin(); iter != settings["Step2Settings"].end(); iter++){
		std::string setting_name = (*iter).first;
		switch (settings["Step2Settings"][setting_name].type) {
			case 0:
				setValue("PathFailureBertini",setting_name,settings["Step2Settings"][setting_name].value());
				break;
			case 1:
				setValue("PathFailureBertini",setting_name,settings["Step2Settings"][setting_name].intvalue);
				break;
			case 2:
				setValue("PathFailureBertini",setting_name,settings["Step2Settings"][setting_name].doubvalue);
				break;
			default:
				break;
		}
	}

	
	return;
}



//writes a config header for a bertini input file to the configname file.
//this is based entirely off data stored in the preferences, eliminating the need for the user to have to 
//carry around their own config files.
std::string ProgSettings::WriteConfigStepOne(){
	
	std::stringstream config;
	//note that bertini parses right over white space.  input files can also be commented.
	config << "CONFIG\n%this config automatically generated by paramotopy\n\n";
	settingmap::iterator iter;
	for (iter=settings["Step1Settings"].begin(); iter != settings["Step1Settings"].end(); iter++){
		std::string setting_name = (*iter).first;
		config << setting_name << ": "
			<< settings["Step1Settings"][setting_name].value() << ";\n";
	}
	config << "USERHOMOTOPY: 0;\n"
		<< "END;\n\n";
	return config.str();
}



//writes a config header for a bertini input file to the configname file.
//this is based entirely off data stored in the preferences, eliminating the need for the user to have to 
//carry around their own config files.
std::string ProgSettings::WriteConfigStepTwo(){
	
	
	std::stringstream config;
	//note that bertini parses right over white space.  input files can also be commented with percent signs.
	config << "CONFIG\n%this config automatically generated by paramotopy\n\n";
	
	settingmap::iterator iter;
	
	for (iter=settings["Step2Settings"].begin(); iter != settings["Step2Settings"].end(); iter++){
		std::string setting_name = (*iter).first;
		config << setting_name << ": "
		<< settings["Step2Settings"][setting_name].value() << ";\n";
	}
	
	config << "USERHOMOTOPY: 1;\n"
	<< "END;\n\n";
	
	return config.str();
}


//writes a config header for a bertini input file to the configname file.
//this is based entirely off data stored in the preferences, eliminating the need for the user to have to 
//carry around their own config files.
std::string ProgSettings::WriteConfigFail(){
	
	
	std::stringstream config;
	//note that bertini parses right over white space.  input files can also be commented with percent signs.
	config << "CONFIG\n%this config automatically generated by paramotopy\n\n";
	
	settingmap::iterator iter;
	
	for (iter=settings["PathFailureBertini"].begin(); iter != settings["PathFailureBertini"].end(); iter++){
		std::string setting_name = (*iter).first;
		config << setting_name << ": "
		<< settings["PathFailureBertini"][setting_name].value() << ";\n";
	}
	
	config << "USERHOMOTOPY: 1;\n"
	<< "END;\n\n";
	
	return config.str();
}




void ProgSettings::default_main_values(){
	setValue("MainSettings","newrandom_newfolder",0);
	setValue("MainSettings","previousdatamethod",1);
}


void ProgSettings::default_basic_bertini_values_stepone(){
	
	
	
	setValue("Step1Settings","TRACKTOLBEFOREEG",1e-5);
	setValue("Step1Settings","TRACKTOLDURINGEG",1e-6);
	setValue("Step1Settings","FINALTOL",1e-11);
	setValue("Step1Settings","PRINTPATHMODULUS",20);
	setValue("Step1Settings","IMAGTHRESHOLD",1e-4);
	setValue("Step1Settings","SECURITYLEVEL",0);
	ProgSettings::save();
	return;
}


void ProgSettings::default_basic_bertini_values_steptwo(){
	
	
	setValue("Step2Settings","TRACKTOLBEFOREEG",1e-5);
	setValue("Step2Settings","TRACKTOLDURINGEG",1e-6);
	setValue("Step2Settings","FINALTOL",1e-11);
	setValue("Step2Settings","PRINTPATHMODULUS",20);
	setValue("Step2Settings","IMAGTHRESHOLD",1e-4);
	setValue("Step2Settings","SECURITYLEVEL",0);
	ProgSettings::save();
	return;
}


void ProgSettings::default_path_failure_settings(){
	
	setValue("PathFailure","newrandommethod",1);
	setValue("PathFailure","turnon_securitylevel1",0);
	setValue("PathFailure","tightentolerances",1);
	setValue("PathFailure","maxautoiterations",3);
	ProgSettings::save();
	return;
}

//locates the step2program.
void ProgSettings::FindProgram(std::string program_name, std::string category_name, std::string setting_name){
	
	bool found_program = false;
	struct stat filestatus;
	
	
	if (haveSetting(category_name,setting_name)) {
		boost::filesystem::path stored_location(settings[category_name][setting_name].value());
		stored_location /= program_name;
		if (boost::filesystem::exists(stored_location)){
			stored_location = boost::filesystem::canonical(boost::filesystem::absolute(stored_location));
			setValue(category_name,setting_name,stored_location.parent_path().string());
			found_program = true;
		}

		
	}
	
	if (found_program == false ){
		boost::filesystem::path here = "./";
		here /= program_name;
		
		if (boost::filesystem::exists(here)){  // if have the step2 program in current directory, set location of it to here.
			here = boost::filesystem::absolute(here);
			setValue(category_name,setting_name,here.parent_path().string());
			found_program = true;
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
					boost::filesystem::path path_to_detect(path.substr(0,found));  //the part of the path to scan for mystep2
					path = path.substr(found+1,path.length()-found);    // the remainder of the path.  will scan later.
					found = path.find(':');                             // get the next indicator of the ':' delimiter.
					path_to_detect /= program_name;
					if (boost::filesystem::exists(path_to_detect)){
						//found the mystep2 program!
						
						setValue(category_name,setting_name,path_to_detect.parent_path().string());
						found_program = true;
						break;
						
					}
					else{
						//did not find it yet...
					}
				}// re:while
			}//re: if temp_path!=null
			else{ //the path variable was null.  WTF?
				std::cerr << "unable to scan $PATH for step2.\n";
				
			}
		}
	}
	
	
	if (found_program == false){
		std::string path_to_detect;
		std::cout << program_name << " program not found in current directory or PATH.\nplease supply the path to " << program_name << ".\nabsolute path is best, but relative works, too.\n";
		std::cin >> path_to_detect;
		boost::filesystem::path temp_path(path_to_detect);
		while (! boost::filesystem::exists(temp_path)) {
			std::cout << program_name << " program not found at that location.  please supply the path:\n";
			std::cin >> path_to_detect;
			temp_path = boost::filesystem::path(path_to_detect);
		}
		setValue(category_name,setting_name,path_to_detect);
	}
	
	std::cout << program_name << " program is located at '" << settings[category_name][setting_name].value() << "'\n";
	return;
}//    re: find_program






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
		case 3:
			ProgSettings::GetDataFolderMethod();
			break;
		case 4:
			ProgSettings::GetNewRandomAtNewFolder();
			break;
		case 5:
			ProgSettings::GetSaveProgress();
			break;
		case 6:
			ProgSettings::GetNewFileThresh();
			break;
		case 7:
			ProgSettings::GetTemporaryFileLocation();
			break;
		case 8:
			ProgSettings::GetStifle();
			break;
		case 9:
			ProgSettings::GetStepTwoLocation();
			break;
		case 10:
			ProgSettings::GetBufferSize();
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
	main_required_values["previousdatamethod"]= 3;
	main_required_values["newrandom_newfolder"]= 4;
	main_required_values["saveprogresseverysomany"] = 5;
	main_required_values["newfilethreshold"] = 6;
	main_required_values["tempfilelocation"] = 7;
	main_required_values["stifle"] = 8;
	main_required_values["step2location"] = 9;
	main_required_values["buffersize"] = 10;
//adding a required value here requires adding a ProgSettings::Get___() function, and adding an option to switch
	
	
//iterate over the main setting map, for each of the above settings.
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

void ProgSettings::GetBufferSize(){
	std::stringstream menustream;
	int val = get_int_choice("How many KB should step2 buffer before writing data to disk? (max 65536K = 64MB)\n (press 0 for default of 64K): ",0,65536);
	if (val == 0) {
		ProgSettings::setValue("MainSettings","buffersize",65536);
	}
	else{
		ProgSettings::setValue("MainSettings","buffersize",val*1024);
	}
	return;
}


//detects whether /dev/shm is a location, and if so, offers to use /dev/shm for the temporary files.
void ProgSettings::GetTemporaryFileLocation(){
	std::stringstream menustream;
	size_t found;
	std::vector< boost::filesystem::path > tmpfile_location_possibilities;
	
	std::vector< boost::filesystem::path > standard_places_to_look;
	standard_places_to_look.push_back("/dev/shm");
	standard_places_to_look.push_back("/tmp");
	
	if (get_int_choice("Would you like to use a ramdisk for temp files?\n0) No.\n1) Yes.\n: ",0,1)==1){
		
		setValue("MainSettings","useramdisk",1);
		for (int ii=0; ii<int(standard_places_to_look.size()); ++ii) {
			if (boost::filesystem::exists(standard_places_to_look[ii])){
				tmpfile_location_possibilities.push_back(standard_places_to_look[ii]);
			}
		}

		if (tmpfile_location_possibilities.size()>0){ //if actually found places
			std::cout << "Found these possibilities:\n\n";
			
			for (int ii = 0; ii< int(tmpfile_location_possibilities.size()); ++ii) {
				std::cout << ii << ": " << tmpfile_location_possibilities[ii].string() << std::endl;
			}
			std::cout << "-or-\n" << tmpfile_location_possibilities.size() << ": specify your own location. (will not check it exists or even works) todo: perform these checks\n\n";
			
			int choice=get_int_choice(": ",0,tmpfile_location_possibilities.size());
			if (choice==tmpfile_location_possibilities.size()) {
				std::string tmplocation;
				std::cout << "where should the root directory for temp files be?\n: " << std::endl;
				std::cin >> tmplocation;
				setValue("MainSettings","tempfilelocation",tmplocation);
			}
			else{
				setValue("MainSettings","tempfilelocation",standard_places_to_look[choice].string());
			}
		}
		else{//found none of the standard places to look
			std::string tmplocation;
			std::cout << "Did not find any of the standard places to look for shared memory access :(\n\n";
			std::cout << "where should the root directory for temp files be?  (% cancels, and chooses not to use ramdisk)\n: " << std::endl;
			std::cin >> tmplocation;
			found=tmplocation.find('%');
			if ( (int(found)==0) ) {
				setValue("MainSettings","useramdisk",0);
				setValue("MainSettings","tempfilelocation",".");
			}
			else{
				setValue("MainSettings","tempfilelocation",tmplocation);
			}
		}
		
			
		
	}
	else {
		setValue("MainSettings","useramdisk",0);
		setValue("MainSettings","tempfilelocation",".");
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
		ProgSettings::default_main_values();
		ProgSettings::GetParallel();
		//ProgSettings::SetSaveFiles(); no need.  do at end of function;
	}
	else {
		TiXmlHandle hDoc(doc);
		TiXmlElement* pElem;
		TiXmlHandle hRoot(0);
		

		pElem=hDoc.FirstChildElement().Element();
		// should always have a valid root but handle gracefully if it doesn't

		if (!pElem) {
			std::cerr << "bad xml file for prefs.  :( todo: check for backup\n";
		}
		else{
			std::string main_name =pElem->Value();  //unused?
			
			hRoot=TiXmlHandle(pElem);  // the handle for the data we will be reading

			TiXmlElement* catElem=hRoot.FirstChild().Element();
			for (catElem; catElem; catElem=catElem->NextSiblingElement()) {
				ProgSettings::ReadCategoryFromXml(catElem->Value(),hRoot);
			}
		}
	}
	
	
	ProgSettings::setRequiredValues();  //check for required settings, and set them if not found already.
	if (!ProgSettings::CheckPrevSetFiles()) {
		ProgSettings::SetSaveFiles();
	}
	
	ProgSettings::FindProgram("step2","MainSettings","step2location");
	ProgSettings::FindProgram("bertini","MainSettings","bertinilocation");
	
	ProgSettings::save();
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
			  << "1) Step1 bertini settings\n"
	          << "2) Step2 bertini settings\n"
			  << "3) Path failure resolution\n"
			  << "4) Parallelism\n"
			  << "5) Set files to save\n"
			  << "6) General Settings\n"
			  << "*\n0) return to paramotopy\n"
			  << "\n: ";
	
	while (choice!=0) {
		choice = get_int_choice(menu.str(),0,6);
		
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
				
			case 6:
				ProgSettings::GeneralMenu();
				break;
				
			default:
				std::cout << "somehow an unacceptable entry submitted :(\n";
				break;
		}
		ProgSettings::save();
	}
	return;
}


void ProgSettings::GeneralMenu(){
	std::stringstream menu;
	int choice=-10;
	
	menu << "\n\nGeneral Settings:\n\n"
		<< "1) Load Data Folder method\n"
		<< "2) Generation of random values at new folder (during program)\n"
		<< "*\n"
		<< "0) go back\n"
		<< "\n: ";
	while (choice!=0) {
		choice = get_int_choice(menu.str(),0,1);
		
		switch (choice) {
			case 0:
						
				break;
				
			case 1:
				ProgSettings::GetDataFolderMethod();
				break;
				
			case 2:
				ProgSettings::GetNewRandomAtNewFolder();
				break;
				
			default:
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
		<< "7) Use ramdisk for temp files\n"
		<< "8) Change Buffer Size\n"
		<< "9) Max Data File Size\n"
		<< "*\n"
		<< "0) go back\n"
		<< "\n: ";
	
	while (choice!=0) {
		ProgSettings::DisplayCurrentSettings("MainSettings");
		choice = get_int_choice(menu.str(),0,9);
		
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
				ProgSettings::GetTemporaryFileLocation();
				break;
				
			case 8:
				ProgSettings::GetBufferSize();
				break;
				
			case 9:
				ProgSettings::GetNewFileThresh();
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
	size_t found;
	
	while ( 1 ) {
		std::cout << "what is the exact name (caps,spelling) of setting?   % cancels\n: ";
		std::cin >> setting_name;
		found=setting_name.find('%');
		if ( (haveSetting(category_name,setting_name)) ||  (int(found)==0) ) {
			break;
		}
	}
	
	if (int(found)==0) {
		std::cout <<  "canceling\n";
		return;
	}
	
	
	
//	int typeint = get_int_choice("what is the type of the setting? \n string: 0\n integer / 0-1 bool: 1\n float/double: 2\n\n:",0,2);
	std::cout << "new value\n:";
	std::string newvalue;
	std::cin >> newvalue;
	std::stringstream converter;
	int intie;
	double doubie;
	switch (settings[category_name][setting_name].type) {
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
	

	std::cout << "what is the exact name (caps,spelling) of setting?     % cancels\n: ";
	std::cin >> setting_name;

	size_t found=setting_name.find('%');
	if ( int(found)==0 ) {
		std::cout << "canceling\n";
		return;
	}
	
	
	
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
	
	size_t found;
	
	while (1) {
		
		std::cout << "what is the exact name (caps,spelling) of setting?    % cancels\n";
		std::cin >> setting_name;
		if ( (haveSetting(category_name,setting_name)) ||  (int(found)==0) ) {
			break;
		}
	}
	
	if (int(found)==0) {
		std::cout <<  "canceling\n";
		return;
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
	menu << "\n\nPath Failure Settings:\n\n"
	<< "1) Choose random-start-point Method\n"
	<< "2) Change security level\n"
	<< "3) Tolerance Tightening\n"
	<< "4) Set Num Iterations\n"
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
				
			case 4:
				ProgSettings::GetNumIterations();
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





//setValue("PathFailure","",5);

void ProgSettings::GetDataFolderMethod(){
	int choice = -10;
	std::stringstream menu;
	menu << "\n\n"
		<< "What to do at startup for folders?\n\n"
		<< "1) Use most recently used folder\n"
		<< "2) Make new folder\n"
		<< "3) Prompt if found previous folder\n"
		<< "\n: ";
	
	choice = get_int_choice(menu.str(),1,3);
	setValue("MainSettings","previousdatamethod",choice);
	ProgSettings::save();
	return;
		
}

void ProgSettings::GetNewRandomAtNewFolder(){

	int choice = -11;
	std::stringstream menu;
	menu << "\n\n"
		<< "If you make a new folder with program running do you want\n"
		<< "to automatically create new random values, or keep previous?\n"
		<< "0) no  (keep old)\n"
		<< "1) yes (make new)\n"
		<< "\n: ";
	
	choice = get_int_choice(menu.str(),0,1);
	
	setValue("MainSettings","newrandom_newfolder",choice);
	ProgSettings::save();
	return;
}


void ProgSettings::GetRandomMethod(){
	int choice = get_int_choice("use a new random start point for each resolve?  0 no, 1 yes\n: ",0,1);
	setValue("PathFailure","newrandommethod",choice);
	ProgSettings::save();
	return;
}

void ProgSettings::GetSecurityLevelPathFailure(){
	//	
	int choice = get_int_choice("Turn on security level 1 for all path failure solves?  0 no, 1 yes\n: ",0,1);
	setValue("PathFailure","turnon_securitylevel1",choice);
	ProgSettings::save();
	return;
}

void ProgSettings::GetTightenTolerancesPathFailure(){
	int choice = get_int_choice("Tighten tolerance by 10x each iteration?  0 no, 1 yes\n: ",0,1);
	setValue("PathFailure","tightentolerances",choice);
	ProgSettings::save();
}

void ProgSettings::GetNumIterations(){
	int choice = get_int_choice("How many iterations to perform automatically?  0 performs none\n: ",0,99999);
	setValue("PathFailure","maxautoiterations",choice);
	ProgSettings::save();
	return;
}
