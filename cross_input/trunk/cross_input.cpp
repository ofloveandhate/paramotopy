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



void WriteMeshToMonteCarlo(int level, std::vector<std::vector<std::pair<float,float> > > Values, std::string dirfilename,std::string cline);

std::vector<std::string> ReadParameters(int numparam, std::ifstream & fin);

std::vector< std::vector< std::pair<float,float>  > >MakeValues(std::vector< std::string > ToParse);

void AddVars(int numparam, 
			 std::string mcfname,
			 std::string inputfilename,
			 std::ofstream & fout);



int main(){
//	
//	// Data members used throughout the program  
	std::string inputfilename;
	std::string outputfilename;
	std::ifstream fin;
	std::ofstream fout;
	std::string tmp;
	std::stringstream ss;
	int numparam;
	std::string crossing;
	std::ifstream crossingin;
	
	
	
	std::cout << "\nEnter the input file's name, incl path form here : ";
	std::cin >> inputfilename;
	

	
	
	std::cout << "\nEnter the output file's name, incl path from here : ";
	std::cin >> outputfilename;
	
	fout.open(outputfilename.c_str());

	
	std::cout << "\nname of crossing file : ";
	std::cin >> crossing;
	crossingin.open(crossing.c_str());
	
	
	getline(crossingin,tmp);
	ss << tmp;
	ss >> numparam;
	
	
	std::vector<std::string> ParamVector;//initialize
	ParamVector = ReadParameters(numparam,crossingin);//get parameter lines
	crossingin.close();//close the input stream
	std::vector< std::vector< std::pair<float,float>  > > Values;//initialize
	Values = MakeValues(ParamVector);//make the values
	std::string cline = "";
	std::string mcfname = "tempmc";
	WriteMeshToMonteCarlo(0, Values, mcfname,cline); //write a temp file
	
	AddVars(numparam,mcfname,inputfilename,fout);//cross the two inputs
	
	
	remove( "tempmc" );//remove temp file
	fout.close();//close stream
	
	return 0;
	
}//end main




////////////
//
//   subfunctions
//
////////





void AddVars(int numparam,
			 std::string mcfname,
			 std::string inputfilename,
			 std::ofstream & fout){
	
	std::ifstream mcfin;
	mcfin.open(mcfname.c_str());
	std::ifstream fin;

	std::string mctmp, tmp;

	while (getline(mcfin,mctmp)) {
		fin.open(inputfilename.c_str());
		while (getline(fin,tmp)) {
			fout << mctmp << " " << tmp << "\n" ;
		}
		fin.close();
	}

	
	
	
	mcfin.close();
	return;
}







void WriteMeshToMonteCarlo(int level, 
						   std::vector<std::vector<std::pair<float, float> > > Values, 
						   std::string dirfilename, 
						   std::string cline){
	
	if (level==Values.size()-1){
		// at the last end of the parameters ....
		//    std::string cline2 = cline;
		for (int i = 0; i < Values[level].size();++i){
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
		for (int i= 0; i < Values[level].size();++i){
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





std::vector<std::string> ReadParameters(int numparam, std::ifstream & fin){
	
	std::vector<std::string> MyParams;
	for (int i = 0; i < numparam; ++i){
		std::string tmp;
		getline(fin,tmp);
		MyParams.push_back(tmp);
	}
	return MyParams;
}


std::vector< std::vector< std::pair<float,float>  > >MakeValues(
																std::vector< std::string > ToParse){
	
	std::vector< std::vector< std::pair<float,float> > > MeshValues;
	
	// parse the parameter string per line
	for (int i = 0; i < ToParse.size();++i){
		std::cout << ToParse[i] << "\n";
		std::string temp;		
		std::stringstream ss;
		ss << ToParse[i];
		ss >> temp;
		// values given by lep, rep, and # of mesh points
		float lepr; // left end point real
		float lepi; // right end point real
		float repr; // 
		float repi;
		int meshpoints;
		std::vector< std::pair<float,float> > currentmesh;
		ss >> lepr;
		ss >> lepi;
		ss >> repr;
		ss >> repi;
		ss >> meshpoints;
		float stepr = (repr - lepr)/(meshpoints-1);
		float stepi = (repi - lepi)/(meshpoints+1);
		float currentr = lepr;
		float currenti = lepi;
		for (int j = 0; j < meshpoints;++j){
			currentmesh.push_back(std::pair<float,float>(currentr,currenti));
			currentr+=stepr;
			currenti+=stepi;
		}
		MeshValues.push_back(currentmesh);
		
		
		
	}
	return MeshValues; 
}





