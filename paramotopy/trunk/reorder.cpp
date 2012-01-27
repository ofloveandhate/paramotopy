
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

int MyFind(int value, std::vector<int> C);

int main(){


  std::cout << "A quick reordering of collected data.\n\n";

  std::string filename;
  int numvar;
  
  std::cout << "Enter the filename : ";
  std::cin >> filename;
  std::cout << "Enter the number of variables : ";
  std::cin >> numvar;

  std::string folderloc = "bfiles_";
  folderloc.append(filename);
  folderloc.append("/folders");
  
  std::vector<std::string> foldernames;

  std::ifstream folderfin;
  folderfin.open(folderloc.c_str());
  std::string blank;
  while (getline(folderfin,blank)){


    foldernames.push_back(blank);
  }
  
  folderfin.close();
  std::ifstream* real_solutions = new std::ifstream[foldernames.size()*2];

  
  
  for (int i = 0; i < foldernames.size();++i){
    
    std::stringstream ss;
    ss << foldernames[i]
       << "/real_solutions0";
		      
    real_solutions[i].open(ss.str().c_str());
  }


  for (int i = foldernames.size(); i < foldernames.size()*2;++i){

    std::stringstream ss;
    ss << foldernames[i-foldernames.size()]
       << "/real_solutions1";
    real_solutions[i].open(ss.str().c_str());
  }

  // everything is open to read in .../

  // read in the first line (i.e. the parameter line) for each file


  for (int i = 0; i < foldernames.size()*2;++i){
    std::string blank2;
    getline(real_solutions[i],blank2);

  }


  // read in ID number (line number in mc file) accordingly



  std::vector<int> CurrentID;

  std::vector<int> CurrentNumSolutions;
  
  for (int i = 0; i < foldernames.size();++i){
    
    //    MyMapping.push_back(std::pair<int,int>
    
    // get id;
    int cid;
    std::string blank2;
    getline(real_solutions[i],blank2);
    
    std::stringstream ss;
    ss << blank2;
    ss >> cid;

    CurrentID.push_back(cid);

    // read the next line that holds param mesh value and ignore


    getline(real_solutions[i],blank2);
  
    // read in the number of solutions provided

    getline(real_solutions[i],blank2);
    
    ss.str("");
    ss.clear();
    
    ss << blank2;
    
    int cnumsolutions;
    ss >> cnumsolutions;
    
    CurrentNumSolutions.push_back(cnumsolutions);
  }

  

  // output run id's with appropriate core # in order

  std::cout << "RunID \t\t\t\t Core\n";

  int lastID;

  for (int i = 0; i < foldernames.size();++i){
    
    std::cout << CurrentID[i] << "\t\t\t\t" << i << "\n";

    lastID = CurrentID[i];
  }
  

  bool done = false;

  while (!done){

    // for each file, read through to next CID

    for (int i = 0; i < foldernames.size();++i){

      // read blank line
      std::string blank3;
      for (int j = 0; j < CurrentNumSolutions[i];++j){
	getline(real_solutions[i],blank3);
	for (int k = 0; k < numvar;++k){
	  getline(real_solutions[i],blank3);   
	}	
      }
      // one more blank
      getline(real_solutions[i],blank3);
      // next is run id, followed by mesh value and then numsolutions again
      if (!real_solutions[i].eof()){
	getline(real_solutions[i],blank3);
	std::stringstream myss;
	myss << blank3;
	int cid2;
	myss >> cid2;
	getline(real_solutions[i],blank3);
	getline(real_solutions[i],blank3);
	myss.str("");
	myss.clear();
	myss << blank3;
	int cnum;
	myss >> cnum;
	CurrentID[i]=cid2;
	CurrentNumSolutions[i]=cnum;
	/*	if (lastID+1 == cid2){
	 
	  lastID=cid2;
	}
	*/
      }
      else{
	real_solutions[i].close();
	
      }      
    }

    // print the new CIDs on which core
    

    for (int i = 0; i < foldernames.size();++i){
      int location = MyFind(lastID,CurrentID);
      if (real_solutions[i].is_open()){

	if (location !=-1){
	  std::cout << CurrentID[location] << "\t\t\t\t" << location << "\n";
	}
	else{
	  std::cout << "couldn't find " << lastID << " in any core.\n";
	}
      }

      ++lastID;
    }
    

    // update done
    
    
    done = true;
    for (int i = 0; i < foldernames.size();++i){
      if (!real_solutions[i].eof()){
	done = false;	
      }
    }

  }


  delete [] real_solutions;
  return 0;

}

int MyFind(int value, std::vector<int> C){


  bool found = false;
  
  for (int i = 0; i < C.size()&&!found;++i){

    if (C[i]==value){

      found = true;
      return i;
    }

  }

  return -1;

}
