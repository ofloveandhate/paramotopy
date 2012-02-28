#include "step1.h"
#include "random.h"
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <sstream>
#include <fstream>
#include <string>


void WriteShell1(int architecture,int usemachine){
	const char* fname = "callbertinistep1.sh";
	std::ofstream fout(fname);
	fout << "#!/bin/bash\n\ncd $1/step1 bertini";
	fout.close();
	chmod((const char*) fname,0755);
}

void WriteShell1Parallel(int architecture,int usemachine){
	const char* fname = "callbertinistep1parallel.sh";
	std::ofstream fout(fname);
	switch (architecture) {
		case 0:
			if (usemachine==1) {

				fout << "#!/bin/bash\n\ncd $1/step1 \nmpirun -machinefile $2 -np $3 $HOME/bertiniparallel";
			}
			else {
				fout << "#!/bin/bash\n\ncd $1/step1 \nmpirun -np $2 $HOME/bertiniparallel";

			}

			
			break;
		case 1:
			if (usemachine==1) {
				
				fout << "#!/bin/bash\n\ncd $1/step1 \naprun -machinefile $2 -np $3 $HOME/lustrefs/./bertiniparallel";
			}
			else {
				fout << "#!/bin/bash\n\ncd $1/step1 \naprun -n $2 /mnt/lustre_server/users/GRAD511/dbrake/bertiniparallel";
				
			}			
			
			break;
	}
	fout.close();
	chmod((const char*) fname,0755);
}

//void WriteShell2(){
//	const char* fname = "cpstartstep2.sh";
//	std::ofstream fout(fname);
//	fout << "#!/bin/bash\n\ncp ./$1/step1/nonsingular_solutions ./$2/start \n";
//    //cd ./$2 \n";
//	//fout << "if [ ! -e nonsingular_solutions ] \nthen \nbertini \nfi \nrm start";
//	fout.close();
//	chmod((const char*) fname,0777);
//}

void WriteShell3(int architecture){
	
	std::string home;


	if (architecture==0){
		home = ".";
		home.append("/callbertinistep2.sh");
	}
	else {
		home = ".";
		home.append("/callbertinistep2.sh");
	}
	
	const char* fname = home.c_str();
//	std::cout << "writing callbertinistep2 to " << fname<< "\n";
//	
	std::ofstream fout(fname);
	switch (architecture) {
		case 0:
			fout << "#!/bin/bash\n\n cd ./$1 \n";
			
			fout << "$HOME/bertini";//$HOME/
			break;
		case 1:
			fout << "#!/bin/bash\n\n cd ./${1} \n";
			
			fout << "$HOME/bertini";
			break;
	}
	//  fout << "if [ ! -e nonsingular_solutions ] \nthen \nbertini \nfi";
	//\nrm start";
	fout.close();
	chmod((char*) fname,0755);
}


void CallBertiniStep1(std::string base_dir){
   
  std::string mystr = "./callbertinistep1.sh";

  mystr.append(" ");
  mystr.append(base_dir);
  system(mystr.c_str());
}

void CallBertiniStep1Parallel(std::string base_dir,std::string machinefile, int numprocs){
	std::stringstream ss;
	std::string mystr = "./callbertinistep1parallel.sh";
	std::string tmp;
	mystr.append(" ");
	mystr.append(base_dir);
	mystr.append(" ");
	mystr.append(machinefile);
	mystr.append(" ");
	ss << numprocs;
	ss >> tmp;
	mystr.append(tmp);
	std::cout << mystr << "\n";
	
	system(mystr.c_str());
	
	
#ifdef verbosestep1
	std::cout << mystr << "\n";
#endif
}

int GetPrefVersion(){
	std::ifstream versionstream;
	std::string homedir = getenv("HOME");
	std::string versionlocation;
	versionlocation = homedir;
	versionlocation.append("/.poly.prefsversion");
	versionstream.open(versionlocation.c_str());
	int version;
	if (versionstream.is_open()) {
		versionstream >> version;
	}
	else {
		version = 0;
	}
	
	versionstream.close();
	return version;
}

void SetPrefVersion(int version){
	std::ofstream versionstream;
	std::string homedir = getenv("HOME");
	std::string versionlocation;
	versionlocation = homedir;
	versionlocation.append("/.poly.prefsversion");
	versionstream.open(versionlocation.c_str());
	versionstream << version;
	versionstream.close();

}


bool DeterminePreferences(preferences *Prefs, bool rerun, std::vector< bool > & FilePrefVector){
	bool parallel;
	std::string tmp, preflocation;
	std::stringstream ss;
	std::ifstream prefstream;
	std::string homedir = getenv("HOME");
	preflocation = homedir;
	preflocation.append("/.poly.prefs");
	prefstream.open(preflocation.c_str());
	
	int numfilespossible = 7; //now 7; was 8	
	std::vector< std::string > FileNames;
	FileNames.resize(numfilespossible);
	FileNames[0] = "real_solutions";
	FileNames[1] = "nonsingular_solutions";
	FileNames[2] = "singular_solutions";
	FileNames[3] = "raw_data";
	FileNames[4] = "raw_solutions";
	FileNames[5] = "main_data";
	FileNames[6] = "midpath_data";
//	FileNames[7] = "failed_paths";
	
	for (int i=0; i<numfilespossible; ++i) {
		FilePrefVector[i] = false;
	}	
	
	int makefreshprefs = 0;
	
	if (!prefstream.is_open() || rerun) {
		makefreshprefs=1;
	}
	else {
		getline(prefstream,tmp);
		if (prefstream.eof()) {
			makefreshprefs=1;
		}
	}
	
	if (makefreshprefs==1) {
		prefstream.close();
		std::ofstream outprefstream;
		outprefstream.open(preflocation.c_str());

		// get info on whether to run parallel or not
		
		std::cout << "Specify preferences for parallel on this machine.\n"
		<< "This creates a file at $HOME/.poly.prefs\n";
		
		parallel = false;
		
		int parselect = -1;
		
		std::cout << "Select your command for running parallel processes:\n"
				<< "0) mpirun\n"
				<< "1) aprun\n"
				<< "2) other... (not a functioning selection yet)\n:";
		std::cin >> Prefs[0].architecture;
		outprefstream << Prefs[0].architecture << "\n";
		
		while(parselect < 0 || parselect > 1){
			std::cout << "Run Parallel?\n"
			<< "0) No.\n"
			<< "1) Yes.\n"
			<<  "Enter choice : ";
			std::cin >> parselect;
			if (parselect==0)
				parallel=false;
			else
				parallel=true;
		}
		

		if (parallel){
			std::cout << "Does your machine use a machinefile?\n0) No.\n1) Yes.\n: ";
			std::cin >> Prefs[0].usemachine;
			
			if (Prefs[0].usemachine==1) {
				
				std::cout << "Enter the machine file to be used, relative to your home directory: ";
				std::cin >> Prefs[0].machinefile;
			}

			std::cout << "Enter the number of processes to be run: ";
			std::cin >> Prefs[0].numprocs;
		}
		outprefstream << parselect << "\n";
		if (parallel){
			outprefstream << Prefs[0].usemachine << " " << Prefs[0].machinefile << "\n"
			<< Prefs[0].numprocs << "\n";
		
		
			std::cout << "Enter step2 number of folders per processor: ";
			std::cin >> Prefs[0].numfilesatatime;

			outprefstream << Prefs[0].numfilesatatime << "\n";
		
		
		}
		else {
			Prefs[0].numfilesatatime = 1;
		}
	

		std::cout << "Select the files to save.\n";
		int selection = 0;
		while (selection!=numfilespossible){
			for (int j = 0; j < numfilespossible;++j){
				if (!FilePrefVector[j]){
					std::cout << j << ")" << FileNames[j] << "\n";
				}
			}
			std::cout << numfilespossible << ") Done.\n";
			std::cout << "Select the file you would like to save : ";
			std::cin >> selection;
			while (selection < 0 || selection > numfilespossible){
				std::cout << "Invalid selection.  Select again : ";
				std::cin >> selection;
			}
			if (selection!=numfilespossible){ 
				std::cout << "Selection = " << selection
				<< ", setting saved to true.\n";
				FilePrefVector[selection]=true;
//				TheFiles[selection].saved=true;
			}
		}
		//done setting files to save for this run.
		
		
		for (int i=0; i<numfilespossible; ++i) {
//			if (TheFiles[i].saved==true) {
			outprefstream << (FilePrefVector[i]? 1 : 0) << (i != numfilespossible?" ":"\n");
//			}
		}
		
		
		std::cout << "Save progress every ? iterations: ";
		std::cin >> Prefs[0].saveprogresseverysomany;
		outprefstream << "\n" << Prefs[0].saveprogresseverysomany << "\n";
		
		outprefstream.close();
		
	}//re: if !prefstream.isopen
	else {
		
		ss << tmp; //would be holding over from before the if statement.
		ss >> Prefs[0].architecture;
		ss.clear();
		ss.str("");
		
		int parselect;
		getline(prefstream,tmp);
		ss << tmp;
		ss >> parselect;
		ss.clear();
		ss.str("");
		if (parselect==0) {
			parallel=false;
		}
		else
		{
			parallel = true;
			getline(prefstream,tmp);
			ss << tmp;
			ss >> Prefs[0].usemachine;
			if (Prefs[0].usemachine==1) {
				ss >> Prefs[0].machinefile;
			}
			ss.clear();
			ss.str("");
			getline(prefstream,tmp);
			ss << tmp;
			ss >> Prefs[0].numprocs;
			ss.clear();
			ss.str("");
			
			getline(prefstream,tmp);
			ss << tmp;
			ss >> Prefs[0].numfilesatatime;
			ss.clear();
			ss.str("");
		}
		
		getline(prefstream,tmp);
		ss << tmp;
		int tmpint;
		std::cout << "saving these files: ";
		
		
		for (int i=0; i<numfilespossible; ++i) {
			ss >> tmpint;
			if (tmpint==1){
				FilePrefVector[i]=true;
				std::cout << FileNames[i] << " "; 
			}
		}
		std::cout << "\n";		
		
		ss.clear();
		ss.str("");
		getline(prefstream,tmp);
		ss << tmp;
		ss >> Prefs[0].saveprogresseverysomany;
		ss.clear();
		ss.str("");
		
		prefstream.close();
	}//re: if prefstream.isopen
	
	
	
	
	
	//a few things to wrap up preference reading.
	
	if (parallel) {
		if (Prefs[0].usemachine) {
			homedir.append("/");
			homedir.append(Prefs[0].machinefile);
			Prefs[0].machinefile = homedir;
		}
		std::cout << "running parallel.\nmachine file: " << Prefs[0].machinefile << "\n" 
		<< "num processors: " << Prefs[0].numprocs << "\n" 
		<< "num folders per proc: " << Prefs[0].numfilesatatime << "\n"
		<< "saveprogressevery: " << Prefs[0].saveprogresseverysomany << "\n\n\n\n";
	}
	else {
		Prefs[0].numprocs = 1;
		Prefs[0].numfilesatatime = 1;	
		std::cout <<"serial setup on this machine.\nnum processors, numfilesatatime set to 1.\n\n\n\n";
	}

	

	
	
	
	return parallel;
}



void ParseData(int & numfunct, int & numvar, int & numparam, int & numconsts,
	       std::vector<std::string> & FunctVector,
	       std::vector<std::string> & VarGroupVector, 
	       std::vector<std::string> & ParamVector, 
	       std::vector< std::string > & ParamStrings,
	       std::vector<std::string>  & Consts,
	       std::vector<std::string> & ConstantStrings,
	       std::vector< std::vector< std::pair<double,double> > > & Values,  
	       std::vector< std::pair<double,double> > & RandomValues,
	       bool & userdefined,
	       std::ifstream & fin,
		   std::vector< int > & NumMeshPoints,
	       std::string filename){

 

  /** Recall structure of input file : */
  /* Numfunct numvargps numparam
     functions line delimited
     VarGroups (comma separated within groups) and line delimited
     0 or 1 -- 0 mesh, 1 user-defined
     if 0
     paramname lepr repr lepi repi nummesh -- line delimited
     if 
     filename
     paramnames line delimited
     END
   */
//  std::cout << "Good here 1\n";
  ReadSizes(numfunct,numvar,numparam,numconsts,fin);
//  std::cout << "Good here 2\n";
  FunctVector = ReadFunctions(numfunct,fin);
//  std::cout << "Good here 3\n";
  VarGroupVector = ReadVarGroups(numvar,fin);
//  std::cout << "Good here 4\n";
  if (numconsts!=0){
    Consts=ReadConstants(fin);
  }
//  std::cout << "Good here 5\n";
  ConstantStrings=ReadConstantStrings(numconsts,fin);
//  std::cout << "Good here 6\n";
  std::string tmp;
  std::stringstream ss;
  getline(fin,tmp);
  ss << tmp;
  ss >> userdefined;
  if (userdefined){
    std::string paramfilename;
    
    getline(fin, paramfilename);
    ParamVector = ReadParameters(numparam,fin);
    ParamStrings = MakeParameterStrings(ParamVector);
    
    // copy user defined file to bfiles_filename/mc
    std::string fileloc = "bfiles_";
    fileloc.append(filename);
    mkdirunix(fileloc.c_str());
    fileloc.append("/mc");
    std::string thecommand = "cp ";
    std::cout << "Paramfilename (i.e. location of user-defined mcfile = "
	      << paramfilename << "\n";
    thecommand.append(paramfilename);
    thecommand.append(" ");
    thecommand.append(fileloc);
    system(thecommand.c_str());
      
	  

//    std::ifstream fin2(paramfilename.c_str());
//    Values = MakeValues(numparam,fin2);
//    fin2.close();
    
  }
  else{
    
    ParamVector = ReadParameters(numparam,fin);
    //  ParamInfo = MakeParamInfo(ParamVector, numparam);
    ParamStrings = MakeParameterStrings(ParamVector);
    Values = MakeValues(ParamVector,NumMeshPoints);
  }
  
  
  
  // temporary output of the parsed data
  RandomValues = MakeRandomValues(ParamStrings.size());



  std::cout << "The functions in FunctVector (as strings) are : \n\n";
  for (int i = 0; i < int(FunctVector.size());++i){
    std::cout << FunctVector[i] << "\n";
  }
  std::cout << "\nThe variable groups in VarGroupVector (as strings) are : \n\n";
  for (int i = 0; i < int(VarGroupVector.size());++i){
    std::cout << VarGroupVector[i] << "\n";
  }
  std::cout << "\nThe parameters in ParamVector (as strings) are :\n\n";
  for (int i = 0; i < int(ParamVector.size());++i){
    std::cout << ParamVector[i] << "\n";
  }

  fin.close();
}


void mkdirunix(std::string mydir){
  
  std::string tmp = "mkdir -p ";
  tmp.append(mydir);
  system(tmp.c_str());

}


void mkdirstep1(std::string inputfilename){

  std::string tmp = "bfiles_";
  tmp.append(inputfilename);
  mkdirunix(tmp);
  tmp.append("/step1");
  mkdirunix(tmp);
  
}

void ReadSizes(int & numfunct, int & numvar, int & numparam, int & numconsts, 
	       std::ifstream & fin){
  
  std::string tmp;
  getline(fin,tmp);
  std::stringstream ss;
  ss << tmp;
  ss >> numfunct;
  ss >> numvar;
  ss >> numparam;
  ss >> numconsts;
}
std::vector<std::string> ReadFunctions(int numfunct, std::ifstream & fin){
  
  std::vector<std::string> MyFunctions;
  
  for (int i = 0; i < numfunct; ++i){
    std::string tmp;  
    getline(fin,tmp);
    MyFunctions.push_back(tmp);
  }
  return MyFunctions;
}
std::vector<std::string> ReadVarGroups(int numvar, std::ifstream & fin){
  std::vector<std::string> MyVarGroups;
  for (int i = 0; i < numvar; ++i){
    std::string tmp;
    getline(fin,tmp);
    MyVarGroups.push_back(tmp);
  }
  return MyVarGroups;
  
}

std::vector<std::string> ReadConstants(std::ifstream & fin){
  std::vector<std::string> MyConstants;
  std::string tmp;
  getline(fin,tmp);
  MyConstants.push_back(tmp);
  return MyConstants;
}

std::vector<std::string> ReadConstantStrings(int numconsts, 
					     std::ifstream & fin){
  std::vector<std::string> MyConstantStrings(0);
  std::cout << "Number of constants = "  << numconsts << "\n";
  for (int i = 0; i < numconsts;++i){
    std::string tmp;
    getline(fin,tmp);
    MyConstantStrings.push_back(tmp);
  }
  return MyConstantStrings;
}



std::vector<std::string> ReadParameters(int numparam, std::ifstream & fin){

  std::vector<std::string> MyParams;
  for (int i = 0; i < numparam; ++i){
    std::string tmp;
    getline(fin,tmp);
    MyParams.push_back(tmp);
  }
  return MyParams;
}



void WriteStep1(std::string filename, 
		std::string configfilename, 
		std::vector<std::string> & FunctVector, 
		std::vector<std::string> & ParamStrings, 
		std::vector<std::string> & VarGroupVector,
		std::vector<std::string> & Consts,
		std::vector<std::string> & ConstantStrings,
		std::vector<std::pair<double, double> > & RandomValues){

  // Make the appropriate Directory to place the input file in
  mkdirstep1(filename);

  // Create the write filename for the input file used by bertini
  // for step 1
  std::string fname2 = "bfiles_";
  fname2.append(filename);
  fname2.append("/step1/input");
  
  // open a file output stream to the input file used by bertini for step 1
  std::ofstream fout(fname2.c_str());
	fout.precision(20);
  // open a file input stream to where the config file is located

  std::ifstream fin(configfilename.c_str());
  
  // Copy contents of config file to the input file
  
  MakeConfig(fin,fout);

  fout << "\nINPUT\n";
  MakeVariableGroups(fout, VarGroupVector);
//  std::cout << "Good here 1\n";
  MakeDeclareConstants(fout, ParamStrings, Consts);
//  std::cout << "Good here 2\n";
  MakeDeclareFunctions(fout, FunctVector.size());
//  std::cout << "Good here 3\n";
  MakeConstants(fout,ParamStrings, ConstantStrings,RandomValues);
//  std::cout << "Good here 4\n";
  MakeFunctions(fout, FunctVector);
//  std::cout << "Good here 5\n";
  fout << "\nEND;\n";
  fout.close();
  fin.close();
  
}

void MakeFunctions(std::ofstream & fout, 
		   std::vector<std::string> FunctVector){

  fout << "\n";
  for (int i = 0; i < int(FunctVector.size());++i){
    fout << "f" << i+1
	 << " = "
	 << FunctVector[i]
	 << ";\n";
    
  }
}

void MakeVariableGroups(std::ofstream & fout, 
			std::vector<std::string> VarGroupVector){

  for (int i = 0; i < int(VarGroupVector.size());++i){
    fout << "\n";
    fout << "variable_group ";
    fout << VarGroupVector[i];
    fout << ";";    
  }
}

void MakeDeclareConstants(std::ofstream & fout, 
			  std::vector<std::string> ParamStrings,
			  std::vector<std::string> Consts){
  
  fout << "\n";
  fout << "constant ";
  for (int i = 0; i < int(ParamStrings.size());++i){
    fout << ParamStrings[i]
	 << (i != int(ParamStrings.size())-1?",":";\n");
    
  }
  if (Consts.size()!=0){
  fout << Consts[0]
       << "\n";
  }
}

void MakeDeclareFunctions(std::ofstream & fout, int size){
  fout << "function ";
  for (int i = 1; i <=size;++i){
    fout << "f" << i;
    if (i != size){
      fout << ", ";
    }
    else{
      fout << ";\n";
    }    
  }
}





void MakeConstants(std::ofstream & fout, 
		   std::vector<std::string> ParamStrings,
		   std::vector<std::string> ConstantStrings,
		   std::vector<std::pair<double,double> > RandomValues){

  for (int i = 0; i < int(RandomValues.size());++i){
    fout << ParamStrings[i]
	 << " = "
	 << RandomValues[i].first
	 << " + "
	 << RandomValues[i].second
	 << "*I;\n";
  }
	for (int i = 0; i < int(ConstantStrings.size());++i){
		fout << "\n" << ConstantStrings[i];
	}
	fout << "\n";
}




std::vector< std::vector< std::pair<double,double>  > >
     MakeValues(int numparam, std::ifstream & fin){


  std::vector< std::vector< std::pair<double,double> > > Values;

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
  return Values;
}

std::vector< std::vector< std::pair<double,double>  > >MakeValues(
	std::vector< std::string > ToParse, std::vector< int > & NumMeshPoints){
  
  std::vector< std::vector< std::pair<double,double> > > MeshValues;
  
  // parse the parameter string per line
  for (int i = 0; i < int(ToParse.size());++i){
    std::string temp;
    //    bool userdefined;
    
    std::stringstream ss;
    ss << ToParse[i];
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
    MeshValues.push_back(currentmesh);
    
    
    // fin.close();
    
  }
  return MeshValues; 
}

std::vector< std::string > MakeParameterStrings(    
       std::vector<std::string> ToParse){
  
  std::vector<std::string> ParamStrings;
  for (int i = 0; i < int(ToParse.size());++i){
    std::stringstream ss;
    ss << ToParse[i];
    std::string paramname;
    ss >> paramname;
    ParamStrings.push_back(paramname);
  }

  return ParamStrings;
}

// assuming we open the input file stream and the output filestream
void MakeConfig(std::ifstream & fin, std::ofstream & fout){
  std::string token;
  while (getline(fin,token)){
    fout << token;
    fout << "\n";
  }
  
}

void MakeConfig(std::vector< std::string > configvector, std::ofstream & fout){
	for (int i=0; i< int(configvector.size());++i) {
		fout << configvector[i] << "\n";
	}	
}

void WriteMeshToMonteCarlo(int level, 
			   std::vector<std::vector<std::pair<double, double> > > Values, 
			   std::string dirfilename, 
			   std::string cline){
  
  if (level==int(Values.size())-1){
    // at the last end of the parameters ....
    //    std::string cline2 = cline;
    for (int i = 0; i < int(Values[level].size());++i){
      // cline = cline2;
      std::stringstream ss;
      ss << cline;
      ss << " ";
      ss << Values[level][i].first;
      ss << " ";
      ss << Values[level][i].second;
      ss << "\n";
      // cline.append(ss.str());
      // std::cout << "dirfilename = " << dirfilename << "\n";
      std::ofstream fout(dirfilename.c_str(),std::ios::app);
      fout << ss.str();
      fout.close();
    }
  }
  else{
    for (int i= 0; i < int(Values[level].size());++i){
      std::stringstream ss;
      ss << cline;
      ss << " ";
      ss << Values[level][i].first;
      ss << " ";
      ss << Values[level][i].second;
      ss << " ";
      // cline.append(Values[level][i].first);
      // cline.append(" ");
      // cline.append(Values[level][i].second);
      // cline.append(" ");
      // cline=ss.str();
      WriteMeshToMonteCarlo(level+1,Values,dirfilename,ss.str());
    }
  } 
}

