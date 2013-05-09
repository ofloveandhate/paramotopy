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



#ifndef __SLAVE_H_INCLUDED__
#define __SLAVE_H_INCLUDED__


#include "step1_funcs.hpp"
#include "step2_funcs.hpp"
#include "mtrand.hpp"
#include "random.hpp"
#include "step2readandwrite.hpp"
#include "timing.hpp"
#include "datagatherer.hpp"


// you must include gmp pour include polysolve.
#include <gmp.h>

extern "C" {
#include "polysolve.h"
}





class slave_process{
	
	
public:
	
	
	
	slave_process(){
		this->have_dotout = false;
		this->have_input = false;
		this->have_start = false;
		
		this->in_working_folder = false;
		/* Find out how many processes there are in the default
		 communicator */
		MPI_Comm_size(MPI_COMM_WORLD, &this->numprocs);
		
		MPI_Comm_rank(MPI_COMM_WORLD,&this->myid);
	
		
		this->called_dir = stackoverflow_getcwd();
		this->homedir = getenv("HOME");
		
		this->filename = "";
		this->numfilesatatime = -1;

		this->tmpfolder = "";
		
		this->numparam = -1;
		this->standardstep2 = -1;

	};
	
	
	
	void slave_main(ProgSettings & paramotopy_settings,
									runinfo & paramotopy_info,
									timer & process_timer);
	
	
	
private:
	
	
	std::string filename;
	int numfilesatatime;
	int numfilesatatime_orig;
	std::string called_dir;
	std::string homedir;
	
	std::string workingfolder;
	
	std::string tmpfolder;
	
	
	
	datagatherer slavegatherer;
	
	unsigned int currentSeed;
	int MPType;
	
	
	int linenumber;

	int numparam;
	
	
	int numprocs;

	
	int myid;
	
	int standardstep2;
	
	
	std::vector<std::string> numdotout;
	std::vector<std::string> arrdotout;
	std::vector<std::string> degdotout;
	std::vector<std::string> namesdotout;
	std::vector<std::string> config;
	std::vector<std::string> funcinput;
	std::vector<std::string> preproc_data;
	
	
//	std::vector<std::pair<double,double> >  current_parameter_values;
	
	bool have_dotout;
	
	
	bool have_input;
	char* input_file;
	
	bool have_start;
	char* start_file;
	
	
	bool in_working_folder;
	
	
	
	
	
	
	void SlaveSetup(ProgSettings & paramotopy_settings,
																 runinfo & paramotopy_info);
	
	
	
	
	
	////////////////////
	//////////////
	//////////
	////////
	//////           Seeding methods
	////
	///
	//
	
	
	
	void SeedSwitch(ProgSettings & paramotopy_settings,
									runinfo & paramotopy_info,
									timer & process_timer);
	
	void SeedBasic(ProgSettings & paramotopy_settings,
								 runinfo & paramotopy_info,
								 timer & process_timer);
	
	
	void SeedSearch(ProgSettings & paramotopy_settings,
									runinfo & paramotopy_info,
									timer & process_timer);
	
	
	
	
	
	
	
	
	////////////////////
	//////////////
	//////////
	////////
	//////           Loops
	////
	///
	//
	
	
	
	void LoopSwitch(ProgSettings & paramotopy_settings,
									runinfo & paramotopy_info,
									timer & process_timer);
	
	
	
	void LoopBasic(ProgSettings & paramotopy_settings,
								 runinfo & paramotopy_info,
								 timer & process_timer);
	
	void LoopSearch(ProgSettings & paramotopy_settings,
									runinfo & paramotopy_info,
									timer & process_timer);
	
	
	
	
	
	////////////////////
	//////////////
	//////////
	////////
	//////           CLEANUP FUNCTIONS
	////
	///
	//
	
	
	
	
	void CleanupSwitch(ProgSettings & paramotopy_settings,
										 runinfo & paramotopy_info,
										 timer & process_timer);
	
	
	
	void CleanupBasic(ProgSettings & paramotopy_settings,
										runinfo & paramotopy_info,
										timer & process_timer);
	
	void CleanupSearch(ProgSettings & paramotopy_settings,
										 runinfo & paramotopy_info,
										 timer & process_timer);
	
	
	
	
	
	
	////////////////////
	//////////////
	//////////
	////////
	//////           Important/Common Send Functions
	////
	///
	//
	
	void ReceiveInput(ProgSettings & paramotopy_settings,
								 runinfo & paramotopy_info,
								 timer & process_timer);
	
	void ReceiveStart(ProgSettings & paramotopy_settings,
								 runinfo & paramotopy_info,
								 timer & process_timer);
	
	
	
	void WriteStart();
	
	
	void WriteInput();
	
	
	void ReadDotOut();
	
	void WriteDotOut();
	
	void WriteNumDotOut(double current_params[]);
	
	void SetWorkingFolder(ProgSettings & paramotopy_settings,
												runinfo & paramotopy_info);
	
	
	
	void PurgeWorkingFolder();
	
	void MoveToWorkingFolder();
	
	
	void GoCalledDir();
	
	void ReadyCheck();
	
	
};








#endif
