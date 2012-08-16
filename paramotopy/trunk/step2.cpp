#include <mpi.h>

#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h> 
#include <omp.h>

#include "step1_funcs.hpp"
#include "step2_funcs.hpp"
#include "mtrand.hpp"
#include "random.hpp"
#include "step2readandwrite.hpp"
#include "master.hpp"
#include "slave.hpp"
#include "para_aux_funcs.hpp"

#define BOOST_FILESYSTEM_VERSION 3
#define BOOST_FILESYSTEM_NO_DEPRECATED

#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/progress.hpp>
#include <boost/regex.hpp>

/*
 the computeNumdenom function is in the bertini library.  it takes in a character array, and pointers which return the numerator
 and denominator of the number.  I pass the input by casting via (char *) 
 */
extern "C" {
	void computeNumDenom(char **numer, char **denom, char *s);
}


extern "C" {
	int bertini_main(int argC, char *args[]);
}











int main(int argc, char* argv[]){

	
	int numfilesatatime;//added march7,11 db
	int saveprogresseverysomany;
	int devshm, newfilethreshold;
	int myid;
	int numprocs;
	int namelen;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int headnode=0;

	std::ifstream fin;



	std::string proc_name_str;
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD,&myid);
	MPI_Get_processor_name(processor_name,&namelen);
	proc_name_str = processor_name;

	
	std::string called_dir = stackoverflow_getcwd();	




	//  bool firstrun = true;
	ToSave *TheFiles;
	std::string filename, location;
	int numsubfolders;
	int numfiles;
	int numparam;


	std::vector<std::vector<std::pair<double,double> > > AllParams;
	std::vector<std::string> ParamNames;
	std::stringstream commandss;  //we put info on and off this stringstream, to get from strings to numbers.


	
    ///////////////////
    //
    //    should be done getting info from commandstring (argv), including some of:
    //
    //    ParamNames (vector)
    //    CValuesVect
    //    numsubfolders (computed from numfilesatatime)
    //    TheFiles with fields filecount (string), saved (bool), filename (string).
    //    numparam
    //    firstrun
    //    toss
    //    blank
    //    filename
    //    numfiles
    //    numfilesatatime
    //
    //////////////////
	
  for (int i = 0; i < argc; ++i){
    commandss << argv[i] << " ";
  }
  
  if (argc!=1){
        std::string blank;
        commandss >> blank;  // name of program, ./mystep2
		commandss >> filename;  // name of input file to polysaurus
	    commandss >> location;
		commandss >> numfiles; // number of files to save

	  TheFiles = new ToSave[numfiles];
	  for (int i = 0; i < numfiles;++i){
		  commandss >> TheFiles[i].filename;
		  TheFiles[i].saved = true;
		  commandss >> TheFiles[i].filecount;
	  }  
	

    commandss >> numfilesatatime;//number of folders for each processor to own, and work out of.
    commandss >> numparam;//number of parameters in the input file.

    for (int i = 0; i < numparam;++i){
      std::string param;
      commandss >> param;
      ParamNames.push_back(param);
    }
	  
	  commandss >> saveprogresseverysomany;
	  commandss >> newfilethreshold;
	  commandss >> devshm;
	  commandss.clear();
	  commandss.str("");


#ifdef verbosestep2
    if (myid==headnode){
      std::cout << "ParamNames: ";
      for (int i=0; i< int(ParamNames.size());++i){
		std::cout << ParamNames[i] << " "; 
      }

      std::cout << "\n"
		<< "numparam: " << numparam << "\n"
		<< "blank: " << blank << "\n"
		<< "filename: " << filename << "\n"
		<< "numfilestosave: " << numfiles << "\n"
		<< "numfilesatatime: " << numfilesatatime << "\n";
    }
#endif
	  
  }
	else{
	// didn't provide filename or any arguments ...
		std::cerr << "Nothing passed as an argument ... \n"
			<< "this should never pop up as this program is only "
			<< " called from paramotopy...\n";
		  MPI_Finalize();
		return 1;
	}
  
	std::string base_dir=make_base_dir_name(filename);  //called from 
	
	std::string templocation;
	if (devshm==1) {
		templocation = "/dev/shm";
	}
	else {
		templocation = called_dir;
	}

	if (myid==headnode) {
		SetUpFolders(location,
					 numprocs,
					 numfilesatatime,
					 templocation);
	}
	
	
    numsubfolders = (numprocs-1)*numfilesatatime;


	std::vector<std::string> tmpfolderlocs;

	for (int i = 1; i < numprocs ;++i){  //used to have numbersubfolders here for upper limit on loop
		std::stringstream tmpfolder;
		tmpfolder << templocation << "/" << location << "/step2/tmp/" << i ;
		tmpfolderlocs.push_back(tmpfolder.str());
	}
	
	
	
     //the main body of the program is here:
  if (myid==headnode){
	  
	  master(tmpfolderlocs,
			 filename, 
			 numfilesatatime, 
			 saveprogresseverysomany,
			 called_dir,
			 templocation,
			 location);
  }
  else{

    slave(tmpfolderlocs,
		  TheFiles, 
		  numfiles, 
		  ParamNames,
		  base_dir,
		  numfilesatatime,
		  called_dir,
		  templocation,
		  newfilethreshold,
		  location);
  }

	
	boost::filesystem::path temppath(templocation);
	temppath /= location;
	boost::filesystem::remove_all(temppath);


  delete[] TheFiles;
  MPI_Finalize();//wrap it up
	return 0;

}//re: main














