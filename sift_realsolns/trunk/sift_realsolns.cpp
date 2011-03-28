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

int main(){
//	Data members used throughout the program  

	std::string linenumber, location, inputfilename, outputfilename, wasteme, tmp;
	std::ifstream fin;
	std::ofstream fout;
	bool realbool;
	int numsolns;
	std::stringstream ss, intstream;
	int numvars;
	
	
	
	std::cout << "\nEnter the original polysaurus input file's name : ";
	std::cin >> inputfilename;
	
	std::cout << "\nEnter your output file name, incl path from here : ";
	std::cin >> outputfilename;
	
	while (inputfilename==outputfilename) {
		std::cout << "cannot have same input and output filenames.\nchoose another output filename : ";
		std::cin >> outputfilename;
	}
	
	//not determined by polysaurus, just have user enter number.  don't really want to reparse input file.
	std::cout << "\nEnter the number of variables : ";
	std::cin >> numvars;
	
	

	
	
	
	
	
	//read in folder file.
	std::string foldername = "bfiles_";
	foldername.append(inputfilename);
	foldername.append("/folders");
	std::ifstream folderstream;
	folderstream.open(foldername.c_str());
	std::string tmpstr;
	std::vector< std::string > foldervector;
	while (getline(folderstream,tmpstr)) {
		foldervector.push_back(tmpstr);
		std::cout << tmpstr << "\n";
	}
	folderstream.close();
	
	
	
	
	
	
	
	
	
	/* begin the main meat of the progam */
	
	
	
	
	fout.open(outputfilename.c_str());
	for (int bla=0; bla<foldervector.size(); ++bla){
		
		inputfilename = foldervector[bla];
		inputfilename.append("/real_solutions");
		inputfilename.append("0");
		std::cout << inputfilename << "\n";
		fin.open(inputfilename.c_str());
		int mm=1;//reset counter
		while (fin.is_open()) {
			getline(fin,tmp);//the parameter names come first in the file
			while (getline(fin,linenumber)) {

				/*
				  first should be the point, already have as tmp.
				  second should be information about the number of solutions.
				*/
				getline(fin,location);
				getline(fin,tmp);
				ss << tmp;
				ss >> numsolns;
				ss.str("");
				ss.clear();
				getline(fin,tmp);
				if (numsolns>0) {
					for (int ii=0; ii<numsolns; ++ii) {
						for (int jj=0; jj<numvars+1; ++jj) {
							getline(fin,wasteme); //dont care about the solution at all.  just if they are real, and we already have that info.
							}
						}
					fout << location << "\n";
					}		
			}
			fin.close();
			inputfilename = foldervector[bla];//reinitialize
			inputfilename.append("/real_solutions");//append
			intstream << mm;//push the integer onto the stringstream, so it can go on the end of inputfilename
			inputfilename.append(intstream.str());//put the integer mm on the end, to increase the real_solutions file count
			intstream.str("");//clear
			intstream.clear();//clear  --  do i need both?  db
			std::cout << inputfilename << "\n";//for verification
			fin.open(inputfilename.c_str());//will test for openness, to continue the while...
			mm++;
		}//re:while fin.is_open
		fin.close();//close just in case is open.
		

	}	//re:for (int bla=0; bla<foldervector.size(); ++bla)
	fout.close();//close the output stream
	
}//end main

