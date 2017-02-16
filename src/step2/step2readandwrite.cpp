#include "step2readandwrite.hpp"













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
    for (int ii=1; ii<numprocs; ++ii) {
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
    for (int ii=1; ii<numprocs; ++ii) {
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
    for (int ii=1; ii<numprocs; ++ii) {
      if (lastnumsent0[ii]<lastnumsent1[ii]) {
				vectortosave = 1;
				break;
      }
      else if (lastnumsent1[ii]<lastnumsent0[ii]){
				vectortosave = 0;
				break;
      }
    }
  }
  
  //not possible for them to be the same.  vectortosave==(-1) if neither size matched.  fresh run if -1
  lastnumsent.push_back(0);
  if (vectortosave==0) {
    smallest = lastnumsent0[0];
    for (int ii=1; ii<numprocs; ++ii) {
      lastnumsent.push_back(lastnumsent0[ii]);
      if (lastnumsent0[ii] < smallest) {
				smallest = lastnumsent0[ii];
      }
    }
    
  }
  else if (vectortosave==1){
    smallest = lastnumsent1[0];
    for (int ii=1; ii<numprocs; ++ii) {
      lastnumsent.push_back(lastnumsent1[ii]);
      if (lastnumsent1[ii] < smallest) {
				smallest = lastnumsent1[ii];
      }
    }
    
  }
  return smallest;
}












