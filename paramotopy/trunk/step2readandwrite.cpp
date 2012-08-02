#include "step2readandwrite.h"


/*
 the computeNumdenom function is in the bertini library.  it takes in a character array, and pointers which return the numerator
 and denominator of the number.  I pass the input by casting via (char *) blabla
 */
extern "C" {
	void computeNumDenom(char **numer, char **denom, char *s);
}

std::string AppendData(int runid, 
					   std::string orig_file, 
					   std::vector<std::string> ParamStrings,
					   std::vector<std::pair<double, double> > CValues){
	
	
	
	std::string cline = "";
	std::stringstream outstring;
	std::ifstream fin(orig_file.c_str());	
	
	if (!fin.is_open()) {
		std::cerr << "failed to open " << orig_file << " to read data" << std::endl;
		exit(-42);
	}
	
	
	outstring << runid << "\n";
	for (int i = 0; i < int(CValues.size());++i){
		outstring << CValues[i].first << " " << CValues[i].second << " ";
	}
	outstring << "\n";
	while(getline(fin,cline)){
		outstring << cline << "\n";
	}
	fin.close();


	return outstring.str();
}


void WriteData(std::string outstring,
			   std::string target_file,
			   std::vector<std::string> ParamStrings){
	
	
	std::ofstream fout;
	// test if file target file is open
	

		
	std::ifstream fouttarget(target_file.c_str());
	if (fouttarget.is_open()){
		fouttarget.close();
		fout.open(target_file.c_str(),std::ios::app);
	}
	else{
		fouttarget.close();
		fout.open(target_file.c_str());
		
		for (int i = 0; i < int(ParamStrings.size());++i){
			fout << ParamStrings[i] << (i != int(ParamStrings.size())-1? " ": "\n");
		}
	}
	if (!fout.is_open()) {
		std::cerr << "failed to open target file " << target_file << "\n";
	}
	
	fout << outstring;
	fout.close();
}



void GetStart(std::string dir,
					std::string & start){
	

	std::stringstream tempss;
	
	std::ifstream fin;
	//get start file in memory
	std::string copyme;
	std::string startstring=dir;
	startstring.append("/step1/nonsingular_solutions");
	fin.open(startstring.c_str());
	
	if (!fin.is_open()) {
		std::cout << "failed to open nonsingular solutions file: " << startstring <<"\n";
	}
	
	while (!fin.eof()) {
		getline(fin,copyme);
		tempss << copyme << "\n";
	}
	fin.close();
	//end get start in memory
	start = tempss.str();
}






int GetLastNumSent(std::string base_dir,
					std::vector< int > & lastnumsent,
					int numprocs){
	
	std::ifstream fin;
	std::string blank;
	std::stringstream commandss;
	// query whether had run or not yet. 
	std::string lastoutfilename0 = base_dir;
	lastoutfilename0.append("/step2/lastnumsent0");	
	fin.open(lastoutfilename0.c_str());
	std::vector< int > lastnumsent0;
	lastnumsent0.push_back(0);
	int tmpint, smallest = 0;
	if (fin.is_open()){
		int lastoutcounter=1;
		for (int i=1; i<numprocs; ++i) {
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
	fin.open(lastoutfilename1.c_str());
	std::vector< int > lastnumsent1;
	lastnumsent1.push_back(0);
	if (fin.is_open()){
		int lastoutcounter=1;
		for (int i=1; i<numprocs; ++i) {
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
	
	
	int vectortosave = -1;
	if ( (int(lastnumsent1.size())<numprocs) && int(lastnumsent0.size())<numprocs ) {//failed overall...suck.
		return 0;
		//both vectors fail.  initialize fresh run.
	}
	else if (int(lastnumsent0.size())<numprocs){//failed during writing of 0?
		vectortosave = 1;
	}
	else if (int(lastnumsent1.size())<numprocs){//failed during writing of 1?
		vectortosave = 0;
	}
	else {//not possible for exactly one of the two following things to happen, given the previous 3 failed
		for (int i=1; i<numprocs; ++i) {
			if (lastnumsent0[i]<lastnumsent1[i]) {
				vectortosave = 1;
				break;
			}
			else if (lastnumsent1[i]<lastnumsent0[i]){
				vectortosave = 0;
				break;
			}
		}
	}
	
	//not possible for them to be the same.  vectortosave==(-1) if neither size matched.  fresh run if -1
	lastnumsent.push_back(0);
	if (vectortosave==0) {
		smallest = lastnumsent0[0];
		for (int i=1; i<numprocs; ++i) {
			lastnumsent.push_back(lastnumsent0[i]);
			if (lastnumsent0[i] < smallest) {
				smallest = lastnumsent0[i];
			}
		}	

	}
	else if (vectortosave==1){
		smallest = lastnumsent1[0];
		for (int i=1; i<numprocs; ++i) {
			lastnumsent.push_back(lastnumsent1[i]);
			if (lastnumsent1[i] < smallest) {
				smallest = lastnumsent1[i];
			}
		}
		
	}
	return smallest;
}





//read the random values from the step1.
void GetRandomValues(std::string base_dir,
					 std::vector< std::pair<double,double> > & RandomValues){
	
	std::stringstream myss;
	std::ifstream fin;
	std::string randfilename = base_dir;
	randfilename.append("/randstart");
	fin.open(randfilename.c_str());
	
	if (!fin.is_open()) {
		std::cout << "failed to open randfilename: " << randfilename << "\n";
	}
	
	int ccount=0;
	std::string mytemp;
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
		std::cout << "Parameter" << ccount
		<< " = "
		<< RandomValues[ccount].first 
		<< " + "
		<< RandomValues[ccount].second
		<< "*I\n";
		++ccount;
	}
	std::cout << "\n";
	fin.close();
	//end get random values
	
}


void ReadDotOut(std::vector<std::string> & Numoutvector, 
				std::vector<std::string> & arroutvector,
				std::vector<std::string> & degoutvector,
				std::vector<std::string> & namesoutvector,
				std::vector<std::string> & configvector,
				std::vector<std::string> & funcinputvector){
	//read in Num.out file
	
	
	std::ifstream fin;
	std::string copyme;
	
	
	fin.open("num.out");

	while (!fin.eof()) {
		getline(fin,copyme);
		Numoutvector.push_back(copyme);
	}
	fin.close();
	//end read num.out
	
	
	//read in arr.out file
	fin.open("arr.out");

	while (!fin.eof()) {
		getline(fin,copyme);
		arroutvector.push_back(copyme);
	}
	fin.close();
	//end read arr.out
	
	
	//read in deg.out file
	fin.open("deg.out");

	while (!fin.eof()) {
		getline(fin,copyme);
		degoutvector.push_back(copyme);
	}
	fin.close();
	//end read deg.out
	
	//read in names.out file
	fin.open("names.out");

	while (!fin.eof()) {
		getline(fin,copyme);
		namesoutvector.push_back(copyme);
	}
	fin.close();
	//end read names.out
	
	//read in names.out file
	fin.open("config");

	while (!fin.eof()) {
		getline(fin,copyme);
		configvector.push_back(copyme);
	}
	fin.close();
	//end read names.out	
	
	
	//read in names.out file
	fin.open("func_input");

	while (!fin.eof()) {
		getline(fin,copyme);
		funcinputvector.push_back(copyme);
	}
	fin.close();
	//end read names.out	
}




void WriteDotOut(std::vector<std::string> & arroutvector,
				 std::vector<std::string> & degoutvector,
				 std::vector<std::string> & namesoutvector,
				 std::vector<std::string> & configvector,
				 std::vector<std::string> & funcinputvector){
	
	std::ofstream fout;
	
	
	fout.open("arr.out");
	for (int ii = 0; ii< int(arroutvector.size())-1; ii++) {
		fout << arroutvector[ii] << "\n";
	}
	fout.close();
	
	fout.open("deg.out");
	for (int ii = 0; ii< int(degoutvector.size())-1; ii++) {
		fout << degoutvector[ii] << "\n";
	}
	fout.close();
	fout.open("names.out");
	for (int ii = 0; ii< int(namesoutvector.size())-1; ii++) {
		fout << namesoutvector[ii] << "\n";
	}
	fout.close();
	
	fout.open("config");
	for (int ii = 0; ii< int(configvector.size())-1; ii++) {
		fout << configvector[ii] << "\n";
	}
	fout.close();
	
	fout.open("func_input");
	for (int ii = 0; ii< int(funcinputvector.size())-1; ii++) {
		fout << funcinputvector[ii] << "\n";
	}
	fout.close();
}







void WriteNumDotOut(std::vector<std::string> Numoutvector,
					std::vector<std::pair<double,double> > AllParams,
					int numparam){
	
	//modified 2012.02.19 to make a single write call. db.
	
	
	char *numer = NULL, *denom = NULL;
	//	char *passto_computeNumDenom = NULL;
	
	
	std::string convertmetochar;
	std::stringstream ss, writess;
	std::ofstream fout;
	fout.open("num.out");
	
	
	
	
	//write the first two lines
	//fout << Numoutvector[0] << "\n" << Numoutvector[1] << "\n";
	writess << Numoutvector[0] << "\n" << Numoutvector[1] << "\n";

	//write the random values associated with this run:
	for (int ii=0; ii<2*numparam; ++ii) {
		//fout << Numoutvector[ii+2] << "\n";
		writess << Numoutvector[ii+2] << "\n";

	}
	
	//write the parameter lines
	for (int ii = 0; ii<numparam; ++ii) {
		if (AllParams[ii].first==0) {
			//fout << "0/1 ;\n";
			writess << "0/1 ;\n";
		}
		else {
			ss << AllParams[ii].first;
			ss >> convertmetochar;
			ss.clear();
			ss.str("");
			
			//make a call from the bertini library 
			computeNumDenom(&numer, &denom, (char *) convertmetochar.c_str());
			
			//fout << numer << "/" << denom << " ;\n";
			writess << numer << "/" << denom << " ;\n";
		}
		
		
		if (AllParams[ii].second==0) {
			//fout << "0/1 ;\n";
			writess << "0/1 ;\n";
		}
		else {
			ss << AllParams[ii].second; 
			ss >> convertmetochar;
			ss.clear();
			ss.str("");
			
			computeNumDenom(&numer, &denom, (char *) convertmetochar.c_str());
			//fout << numer << "/" << denom << " ;\n";
			writess << numer << "/" << denom << " ;\n";
		}
		
	}
	
	for (int ii = (2+4*numparam); ii< int(Numoutvector.size())-1; ++ii) {
		//fout << Numoutvector[ii] << "\n";
		writess << Numoutvector[ii] << "\n";
	}
	
	
	fout << writess.str();
	fout.close();
	
}



int GetMcNumLines(std::string base_dir, int numparam){
	std::string mcfname = base_dir;
	mcfname.append("/mc");
	int num_lines, num_words;
	std::string command = "wc ";
	command.append(mcfname);
	command.append(" > wc.out");
	
	system(command.c_str());
	
	std::string wcname = "wc.out";

	
	std::string temp;
	std::ifstream fin(wcname.c_str());
	std::stringstream myss;
	
	getline(fin,temp);
	fin.close();
	system("rm wc.out");
	
	myss << temp;
	myss >> num_lines;
	myss >> num_words;
	
	int checking;
	checking = num_words/(2*numparam);  //there is a pair of real/complex values for each parameter value.
	
	if (checking > num_lines) {
		return checking;
	}
		else {
			return num_lines;
		}

	
	
}



