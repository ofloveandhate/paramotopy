//contains the runinfo class

#include "runinfo.h"






void runinfo::make_base_dir_name(){
	
	base_dir = "";
	
	std::string remainder = inputfilename;
	size_t found;
	found = remainder.find('/');
	while (found!=std::string::npos) {  //if found the delimiter '/'
		
		base_dir.append(remainder.substr(0,found+1));  //
		std::cout << base_dir << "\n";
		remainder = remainder.substr(found+1,remainder.length()-found);    // the remainder of the path.  will scan later.
		found = remainder.find('/');                             // get the next indicator of the '/' delimiter.
		
		
		
	}// re:while
	base_dir.append("bfiles_");
	base_dir.append(remainder);
	
	return;
}







bool runinfo::test_if_finished(){
	bool finished = false;
	
	std::string finishedfile = base_dir;
	finishedfile.append("/finished");
	struct stat filestatus;
	
	if (stat( finishedfile.c_str(), &filestatus ) ==0){  // if it can see the 'finished' file
		finished = true;
	}
	return finished;
}


void runinfo::SetLocation(){
	
	location = "unsetlocation";
	std::cout << "todo: setlocation whatnot\n";
}






std::string runinfo::WriteInputStepOne(){

	std::stringstream inputfilestream;

	inputfilestream << "\nINPUT\n";
	runinfo::MakeVariableGroups(inputfilestream);
	runinfo::MakeDeclareConstants(inputfilestream);
	runinfo::MakeDeclareFunctions(inputfilestream);
	runinfo::MakeConstants(inputfilestream);
	runinfo::MakeFunctions(inputfilestream);
	inputfilestream << "\nEND;\n";
	
	return inputfilestream.str();
}


std::string runinfo::WriteInputStepTwo(std::vector<std::pair<double, double> > tmprandomvalues){
	
	std::stringstream inputfilestream;
	
	
	inputfilestream << "\nINPUT\n\n";
	// Variable Group portion
	inputfilestream << "variable ";  
	for (int i = 0; i < int(VarGroups.size());++i){
		inputfilestream << VarGroups[i] 
		<< ( i != numvargroup-1? ",":";\n\n" );    
		
		
	}
	// constant portion for here and rand in the parameter homotopy
	inputfilestream << "constant ";
	for (int i = 0; i < numparam;++i){
		inputfilestream << "rand" << ParameterNames[i]
		<< (i!= numparam-1?",":";\n");
	}
	inputfilestream << "constant ";
	for (int i = 0; i < numparam;++i){
		inputfilestream << "here" << ParameterNames[i]
		<< (i!= numparam -1?",":";\n");
	}
	
	
	// user given constants
	if (Constants.size()!=0){
		inputfilestream << Constants[0] << "\n";
	}
	
	// Define Path Variable and the parameter homotopies
	
	inputfilestream << "pathvariable t;\n"
	<< "parameter ";
	for (int i =0;i < numparam;++i){
		inputfilestream << ParameterNames[i] << (i!= numparam-1?",":";\n");
	}
	
	// Declare Functions
	runinfo::MakeDeclareFunctions(inputfilestream);
	runinfo::MakeConstantsStep2(tmprandomvalues,inputfilestream);
	runinfo::MakeFunctions(inputfilestream);
	inputfilestream << "END;\n";
	
	return inputfilestream.str();
}




//for getting file name
void runinfo::GetInputFileName(){
	
	bool finfound = false;
	std::string filename;
	
	while (!finfound) {
		
		std::cout << "Enter the input file's name.\n: ";
		std::cin >> filename;
		
		struct stat filestatus;
		
		if (stat(filename.c_str(), &filestatus) ){
			finfound = true;
		}
		else {
			std::cout << "could not open a file of that name... try again.\n";
		}
	}
	runinfo::make_base_dir_name();
	return;
}

void runinfo::GetInputFileName(std::string suppliedfilename){
	struct stat filestatus;
	if (stat( suppliedfilename.c_str(), &filestatus ) ==0){  // if it can see the 'finished' file
		inputfilename = suppliedfilename;
	}
	else{
		std::cout << "Unable to find file of supplied name.\n";
		runinfo::GetInputFileName();
	}
	runinfo::make_base_dir_name();
	return;
}



void runinfo::SaveRandom(){
	std::ofstream fout;
	
	std::string randpointfilename;
	std::cout << "Enter the filename you want to save the random start points to : ";
	std::cin >> randpointfilename;
	
	fout.open(randpointfilename.c_str());
	
	for (int i = 0; i < numparam; ++i){
		fout << RandomValues[i].first << " "
		<< RandomValues[i].second << "\n";
	}
	fout.close();
	
	return;	
}



void runinfo::LoadRandom(){
	std::string randfilename;
	int cancelthis = 0;
	
	std::ifstream fin;
	
	
	while ( !fin.is_open() ) {
		std::cout << "Enter the filename of the random points you"
		<< " want to load (% to cancel): ";
		std::cin >> randfilename;
		
		size_t found=randfilename.find('%');
		if (found!=std::string::npos) {
			if ( int(found) == 0) {
				std::cout << "canceling load.\n" << std::endl;
				cancelthis = 1;
				break;
			}
			
		}
		
		
		fin.open(randfilename.c_str());
	}
	
	if (!cancelthis) {
		
		std::string mytemp;
		int ccount=0;
		std::cout << "\n\n";
		while(getline(fin,mytemp)){
			std::stringstream myss;
			myss << mytemp;
			double crandreal;
			double crandimaginary;
			myss >> crandreal;
			myss >> crandimaginary;
			RandomValues[ccount].first = crandreal;
			RandomValues[ccount].second = crandimaginary;
			std::cout << ParameterNames[ccount]
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
	
	
	runinfo::PrintRandom();//in random.cpp
	
	return;
}


void runinfo::SetRandom(){

	MTRand drand(time(0));
	int randchoice;
	std::cout << "1) Default range [0 to 1)\n"
	<< "2) User-specified range\n"
	<< "Enter in if you would like to use the standard \n"
	<< "default range for random floats or if you would \n"
	<< "like to specify the range : ";
	std::cin >> randchoice;
	while (randchoice != 1 && randchoice != 2){
		std::cout << "Please enter 1 for the default range or 2 for"
		<< " a user-specified range : ";
		std::cin >> randchoice;
	}
	if (randchoice == 1){
		MakeRandomValues();
	}
	if (randchoice == 2){
		
		int paramrandchoice = 1;	
		while (paramrandchoice != 0){ 
			std::cout << "0 - Done specifying random values\n";
			for (int i = 0; i < numparam;++i){
				std::cout << i+1 << " - " << ParameterNames[i]
				<< "\n";
			}
			std::cout << "Enter the parameter you want to rerandomize : ";
			std::cin >> paramrandchoice;
			while (paramrandchoice < 0 || paramrandchoice > int(ParameterNames.size())){
				
				std::cout << "0 - Done specifying random values\n";
				for (int i = 0; i < int(ParameterNames.size());++i){
					std::cout << i+1 << " - " << ParameterNames[i]
					<< "\n";
				}
				std::cout << "Enter the parameter you want to rerandomize : ";
				std::cin >> paramrandchoice;
			}
			if (paramrandchoice !=0){
				std::cout << "Enter a low range followed "
				<< "by a high range for the"
				<< " real and imaginary parts "
				<< "of the chosen parameter."
				<< "\n";
				
				double creallow;
				double crealhigh;
				double cimaginarylow;
				double cimaginaryhigh;
				std::cout << "\n" << "Real Low : ";
				std::cin >> creallow;
				std::cout << "Real High : ";
				std::cin >> crealhigh;
				std::cout << "Imaginary Low : ";
				std::cin >> cimaginarylow;
				std::cout << "Imaginary High : ";
				std::cin >> cimaginaryhigh;	  
				double crandreal = drand();
				double crandimaginary = drand();
				crandreal*=(crealhigh-creallow);
				crandimaginary*=(cimaginaryhigh-cimaginarylow);
				crandreal+=creallow;
				crandimaginary+=cimaginarylow;
				RandomValues[paramrandchoice-1].first=crandreal;
				RandomValues[paramrandchoice-1].second=crandimaginary;
			}
		}
	}
	runinfo::PrintRandom();//in random.cpp
	
	
}




void runinfo::PrintRandom(){
	//  std::cout << "RandomValues.size() = " << RandomValues.size();
	//  std::cout << "\nParameterNames.size() = " << ParameterNames.size();
	// std::cout << "\n";
	std::cout << "The random initial values for the parameters are : \n";
	for (int i = 0; i < int(ParameterNames.size());++i){
		std::cout << ParameterNames[i] << " = " << RandomValues[i].first
		<< " + " << RandomValues[i].second << "*I\n";
		
	}
	std::cout << "\n";
	return;
}



					 
					 
void runinfo::ParseData(){
	
	std::ifstream fin;
	fin.open(inputfilename.c_str());
	
	runinfo::ReadSizes(fin);
	runinfo::ReadFunctions(fin);
	runinfo::ReadVarGroups(fin);
	
	if (numconsts!=0){
		runinfo::ReadConstants(fin);
	}
	
	runinfo::ReadConstantStrings(fin);
	
	std::string tmp;
	std::stringstream ss;
	getline(fin,tmp);
	ss << tmp;
	ss >> userdefined;
	if (userdefined){
		std::string paramfilename;
		
		getline(fin, paramfilename);
		runinfo::ReadParameters(fin);
		runinfo::MakeParameterNames();
		
		// copy user defined file to bfiles_filename/mc
		runinfo::make_base_dir_name();
		std::string fileloc = base_dir;
		mkdirunix(fileloc.c_str());
		fileloc.append("/mc");
		std::string thecommand = "cp ";
		std::cout << "Paramfilename (i.e. location of user-defined mcfile = "
		<< paramfilename << "\n";
		thecommand.append(paramfilename);
		thecommand.append(" ");
		thecommand.append(fileloc);
		system(thecommand.c_str());
	}
	else{
		runinfo::ReadParameters(fin);
		runinfo::MakeParameterNames();
		runinfo::MakeValues();
	}
	
	fin.close();  
	// temporary output of the parsed data
	runinfo::MakeRandomValues();
	
	
	
	std::cout << "The functions are : \n\n";
	for (int i = 0; i < int(Functions.size());++i){
		std::cout << Functions[i] << "\n";
	}
	std::cout << "\nThe variable groups are : \n\n";
	for (int i = 0; i < int(VarGroups.size());++i){
		std::cout << VarGroups[i] << "\n";
	}
	std::cout << "\nThe parameters are :\n\n";
	for (int i = 0; i < int(Parameters.size());++i){
		std::cout << Parameters[i] << "\n";
	}
	
	runinfo::GetNumVariables();
	std::cout << numvariables << " total variables detected.\nDone parsing.\n";
	
	
	
}


//part of the suite of parse input functions, reads the top line of paramotopy input file.
void runinfo::ReadSizes(std::ifstream & fin){
	numfunct = 0;
	numvargroup = 0;
	numparam = 0;
	numconsts = 0;
	
	std::string tmp;
	getline(fin,tmp);
	std::stringstream ss;
	ss << tmp;
	ss >> numfunct;
	ss >> numvargroup;
	ss >> numparam;
	ss >> numconsts;
}



//part of the suite of parse input functions, reads the parameter declarations.
void runinfo::ReadParameters(std::ifstream & fin){
	Parameters.clear();
	for (int i = 0; i < numparam; ++i){
		std::string tmp;
		getline(fin,tmp);
		Parameters.push_back(tmp);
	}
	return;
}



void runinfo::MakeRandomValues(){
	RandomValues.clear();
	MTRand drand(time(0));
	for (int i = 0; i < numparam;++i){
		double creal = double(drand());
		double cimaginary = double(drand());
		RandomValues.push_back(std::pair<double,double>(creal,cimaginary));
	}
	
	
	return;
}


void runinfo::MakeRandomValues(std::vector< std::pair< std::pair< double, double >, 
				 std::pair< double, double > > > RandomRanges){
	
	RandomValues.clear();
	
	MTRand drand(time(0));
	for (int i = 0; i < int(RandomRanges.size()); ++i){
		double crandreal = drand();
		double crandimaginary = drand();
		crandreal*=(RandomRanges[i].first.second 
					- RandomRanges[i].first.first);
		crandreal+=RandomRanges[i].first.first;
		crandimaginary*=(RandomRanges[i].second.second
						 -RandomRanges[i].second.first);
		crandimaginary+=RandomRanges[i].second.first;
		RandomValues.push_back(std::pair<double,double>(crandreal,
														crandimaginary));
		
	}
	
	return;
}


std::vector<std::pair<double, double> > runinfo::MakeRandomValues(int garbageint){
	std::vector<std::pair<double, double> > BlaRandomValues;
	MTRand drand(time(0));
	for (int i = 0; i < numparam;++i){
		double creal = double(drand());
		double cimaginary = double(drand());
		BlaRandomValues.push_back(std::pair<double,double>(creal,cimaginary));
	}
	
	
	return BlaRandomValues;
	
}


//part of the suite of parse input functions, reads the functions from the paramotopy input file
void runinfo::ReadFunctions(std::ifstream & fin){
	Functions.clear();
	for (int i = 0; i < numfunct; ++i){
		std::string tmp;  
		getline(fin,tmp);
		Functions.push_back(tmp);
	}
	return;
}



//part of the suite of parse input functions, reads the variable groups from paramotopy input file
void runinfo::ReadVarGroups(std::ifstream & fin){
	VarGroups.clear();
	for (int i = 0; i < numvargroup; ++i){
		std::string tmp;
		getline(fin,tmp);
		VarGroups.push_back(tmp);
	}
	return;
}



//part of the suite of parse input functions, reads the constants
void runinfo::ReadConstants(std::ifstream & fin){
	Constants.clear();
	std::string tmp;
	getline(fin,tmp);
	Constants.push_back(tmp);
	return;
}


//part of the suite of parse input functions, reads the constants
void runinfo::ReadConstantStrings(std::ifstream & fin){
	ConstantNames.clear();
	std::cout << "Number of constants = "  << numconsts << "\n";
	for (int i = 0; i < numconsts;++i){
		std::string tmp;
		getline(fin,tmp);
		ConstantNames.push_back(tmp);
	}
	return;
}


void runinfo::GetNumVariables(){
	
	int count = 0;
	size_t comma_found;
	
	for (int i=0; i<numvargroup; ++i) {
		count++;
		comma_found = VarGroups[i].find(",");
		while ( comma_found != std::string::npos ) {
			count++;
			comma_found = VarGroups[i].find(",",comma_found+1,1);
		}	
	}
	numvariables = count;
	return;
}



void runinfo::MakeFunctions(std::stringstream & inputfilestream){
	inputfilestream << "\n";
	for (int i = 0; i < numfunct;++i){
		inputfilestream << "f" << i+1
		<< " = "
		<< Functions[i]
		<< ";\n";
		
	}
}



//writes variable groups to stringstream for bertini input file.
void runinfo::MakeVariableGroups(std::stringstream & fout){

	for (int i = 0; i < numvargroup;++i){
		fout << "\n";
		fout << "variable_group ";
		fout << VarGroups[i];
		fout << ";";    
	}
}


//writes constant declarations to stringsteam, for writing to file
void runinfo::MakeDeclareConstants(std::stringstream & fout){
	fout << "\n";
	fout << "constant ";
	for (int i = 0; i < int(ParameterNames.size());++i){
		fout << ParameterNames[i]
			<< (i != int(ParameterNames.size())-1?",":";\n");
		
	}
	if (Constants.size()!=0){
		fout << Constants[0]
		<< "\n";
	}
}


//writes the function declaration to mutable stringstream, for writing to file for bertini later.
void runinfo::MakeDeclareFunctions(std::stringstream & inputfilestream){
	inputfilestream << "function ";
	for (int i = 1; i <=numfunct;++i){
		inputfilestream << "f" << i;
		if (i != numfunct){
			inputfilestream << ", ";
		}
		else{
			inputfilestream << ";\n";
		}    
	}
	return;
}


void runinfo::MakeConstants(std::stringstream & fout){
	
	for (int i = 0; i < int(RandomValues.size());++i){
		fout << ParameterNames[i]
		<< " = "
		<< RandomValues[i].first
		<< " + "
		<< RandomValues[i].second
		<< "*I;\n";
	}
	for (int i = 0; i < int(ConstantNames.size());++i){
		fout << "\n" << ConstantNames[i];
	}
	fout << "\n";
}


void runinfo::MakeConstantsStep2(std::vector<std::pair<double, double> > CValues, std::stringstream & inputfilestream){
	
	for (int i = 0; i < int(ParameterNames.size()); ++i){
		inputfilestream << "rand" 
		<< ParameterNames[i]
		<< " = "
		<< RandomValues[i].first << " + " << RandomValues[i].second
		<< "*I;\n";
	}
	
	for (int i = 0; i < int(ParameterNames.size()); ++i){
		inputfilestream << "here" 
		<< ParameterNames[i]
		<< " = "
		<< CValues[i].first << " + " << CValues[i].second
		<< "*I;\n";
	}
	for (int i = 0; i < int(ConstantNames.size());++i){
		inputfilestream << ConstantNames[i]
		<< "\n";
	}
	
	for (int i = 0; i < int(ParameterNames.size()); ++i){
		inputfilestream << ParameterNames[i]
		<< "= t*rand"
		<< ParameterNames[i]
		<< " + (1-t)*here"
		<< ParameterNames[i]
		<< ";\n";
	}
	return;
}






void runinfo::MakeValues(std::ifstream & fin){
	
	
	Values.clear();
	
	std::string temp;
	int lcount = 0;
	while(getline(fin,temp)){
		std::vector< std::pair<double, double> > CValue;
		std::stringstream ss;
		ss << temp;
		for (int i = 0; i < numparam;++i){
			double creal;
			double cimaginary;
			ss >> creal;
			ss >> cimaginary;
			CValue.push_back(std::pair<double, double>(creal,cimaginary));
		}
		++lcount;
		
		Values.push_back(CValue);
	}
	std::cout << "line count = " << lcount << "\n\n";
	return;
}


//sets the Values and NumMeshPoints variables in the runinfo class
void runinfo::MakeValues(){
	
	NumMeshPoints.clear();
	Values.clear();
//	std::vector< std::vector< std::pair<double,double> > > MeshValues;
	
	// parse the parameter string per line
	for (int i = 0; i < int(Parameters.size());++i){
		std::string temp;
		//    bool userdefined;
		
		std::stringstream ss;
		ss << Parameters[i];
		ss >> temp;
		//   ss >> userdefined;
		//   if (!userdefined){ 
		// values given by lep, rep, and # of mesh points
		double lepr; // left end point real
		double lepi; // right end point real
		double repr; // 
		double repi;
		int meshpoints;
		std::vector< std::pair<double,double> > currentmesh;
		ss >> lepr;
		ss >> lepi;
		ss >> repr;
		ss >> repi;
		ss >> meshpoints;
		NumMeshPoints.push_back(meshpoints);
		double stepr = (repr - lepr)/(meshpoints-1);
		double stepi = (repi - lepi)/(meshpoints+1);
		double currentr = lepr;
		double currenti = lepi;
		for (int j = 0; j < meshpoints;++j){
			currentmesh.push_back(std::pair<double,double>(currentr,currenti));
			currentr+=stepr;
			currenti+=stepi;
		}
		Values.push_back(currentmesh);
		
		
		// fin.close();
		
	}
	return; 
}


//given a vector of strings to parse, makes and returns a vector of parameter names
void runinfo::MakeParameterNames(){
	
	ParameterNames.clear();
	for (int i = 0; i < numparam;++i){
		std::stringstream ss;
		ss << Parameters[i];
		std::string paramname;
		ss >> paramname;
		ParameterNames.push_back(paramname);
	}
	
	return;
}



void runinfo::DisplayAllValues(){

	std::cout << "nfunc " << numfunct << "\n";
	for (int ii=0; ii<numfunct; ++ii) {
		std::cout << Functions[ii] << "\n";
	}
	
	std::cout << "nvargrp " << numvargroup << "\n";
	for (int ii=0; ii<numvargroup; ++ii) {
		std::cout << VarGroups[ii] << "\n";
	}
	
	std::cout << "nparam " << numparam << "\n";
	for (int ii=0; ii<numparam; ++ii) {
		std::cout << Parameters[ii] << "\n";
	}
	for (int ii=0; ii<numparam; ++ii) {
		std::cout << "name " << ParameterNames[ii] << "\n";
	}
	
	
	std::cout << "nconst " << numconsts << "\n";
	for (int ii=0; ii<numconsts; ++ii) {
		std::cout  << ConstantNames[ii] << "\n";
	}
	std::cout  << Constants[0] << "\n";

	
	
	
	std::cout << "loc " << location
		<< "base_dir " << base_dir
		<< "fname " << inputfilename << "\n";
	
	std::cout << "nvar " << numvariables << "\n";
	std::cout << mcfname << "\n";
	return;
}


