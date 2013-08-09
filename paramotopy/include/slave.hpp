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
#include "bertini.h"
}



/**
 *  @class "slave_process"
 *
 * process for running Bertini to solve a problem.
 *
 * \brief slave process for basic searches and brute-force runs.
 **/


class slave_process{
	
	
public:
	
	
	/** 
	 default constructor.
	 
	 for now, gets its membership in the default communicator MPI_COMM_WORLD.  
	 
	 sets stuff to empty or bad values for checking readiness later.
	 */
	
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
	
	
	/**
	 * the main slave process.  to be used by myid!=0.
	 
	 this is the only public method excepting the constructors.
	 
	 \param input_settings     current input settings.
	 \param input_p_info       current parser run info.
	 \param process_timer      MUTABLE current timer, and returns to user for further use.
	 */	void slave_main(ProgSettings input_settings,
									runinfo input_p_info,
									timer & process_timer);
	
	
	
private:
	
	runinfo paramotopy_info; ///<  the parsed paramotopy input file.
	ProgSettings paramotopy_settings; ///< the ProgSettings for this process
	
	
	boost::filesystem::path filename;			///< the filename for the problem
	int numfilesatatime;			///< the expected number of files per send
	int numfilesatatime_orig; ///< the expected number of files per send
	boost::filesystem::path called_dir;   ///< where was I instantiated?
	boost::filesystem::path homedir;      ///< where is $(HOME)?
	
	boost::filesystem::path workingfolder;  ///< where should I go to perform work?
	
	boost::filesystem::path tmpfolder;			///< the basis for workingfolder
	
	
	
	datagatherer slavegatherer;  ///< data member from datagatherer, which will gather bertini output files.
	
	unsigned int currentSeed;    ///< the current seed for random number generator.
	int MPType;   ///< MPType for current run.
	
	
	int linenumber;  ///<  what line is currently being run

	int numparam;  ///<  the number of parameters in the problem.
	
	
	int numprocs;  ///< the number of processors in my MPI communicator.

	
	int myid; ///< my id withing my MPI communicator
	
	int standardstep2;   ///< flag for total degree or coefficient parameter homotopy.
	
	
	std::vector<std::string> numdotout;			///<  num.out file.
	std::vector<std::string> arrdotout;			///<  arr.out file.
	std::vector<std::string> degdotout;			///<  degout file.
	std::vector<std::string> namesdotout;		///<  names.out file.
	std::vector<std::string> config;				///<  config file.
	std::vector<std::string> funcinput;			///<  func_input file.
	std::vector<std::string> preproc_data;	///<  preproc_data file.
	

	bool have_dotout;  			///<  flag indicating whether we have read the .out files into memory.
	bool have_input;  			///<  flag indicating whether we have received the INPUT file from master.
	char* input_file;       ///< the input file in a c-friendly format
	
	bool have_start;  			///<  flag indicating whether we have received the START file from master.
	char* start_file;       ///< the start file in a c-friendly format
	
	
	bool in_working_folder; ///< boolean indicating whether i think i am in the correct location to perform work.
	
	
	
	
	
	/**
	 the slave setup function
	 
	 uses the runinfo and ProgSettings in memory to setup the rest of the necessaries.
	 
	 
	 */
	void SlaveSetup();
	
	
	
	
	
	////////////////////
	//////////////
	//////////
	////////
	//////           Seeding methods
	////
	///
	//
	
	
	/**
	 switch for choosing seed process
	 
	 \param process_timer      current timer
	 */
	void SeedSwitch(timer & process_timer);
	
	
	/**
	 basic seeding function
	 
	 \param process_timer      current timer
	 */
	void SeedBasic(timer & process_timer);
	
	
	/**
	 search mode seeding  function
	 
	 
	 \param process_timer      current timer
	 */
	void SeedSearch(timer & process_timer);
	
	
	
	
	
	
	
	
	////////////////////
	//////////////
	//////////
	////////
	//////           Loops
	////
	///
	//
	
	
	
	
	/**
	 main loop switch
	 
	 \param process_timer      current timer
	 */
	void LoopSwitch(timer & process_timer);
	
	
	/**
	 basic mode run loop
	 
	 \param process_timer      current timer
	 */
	void LoopBasic(timer & process_timer);
	
	
	/**
	 search mode loop
	 
	 \param process_timer      current timer
	 */
	void LoopSearch(timer & process_timer);
	
	
	
	
	
	////////////////////
	//////////////
	//////////
	////////
	//////           CLEANUP FUNCTIONS
	////
	///
	//
	
	
	
	/**
	 cleanup switch
	 
	 \param process_timer      current timer
	 */
	void CleanupSwitch(timer & process_timer);
	
	
	/**
	 basic mode cleanup function.
	 
	 \param process_timer      current timer
	 */
	void CleanupBasic(timer & process_timer);
	
	
	/**
	 search mode cleanup function.
	 
	 \param process_timer      current timer
	 */
	
	void CleanupSearch(timer & process_timer);
	
	
	
	
	
	
	////////////////////
	//////////////
	//////////
	////////
	//////           Important/Common Send Functions
	////
	///
	//
	/**
	 receives the input file to all workers in the communicator
	 
	 \param process_timer      current timer
	 */
	void ReceiveInput(timer & process_timer);
	
	/**
	 receives the start file to all workers in the communicator
	 
	 \param process_timer      current timer
	 */
	void ReceiveStart(timer & process_timer);
	
	
	/**
	 writes the start file to the current location with the name "start"
	 */
	void WriteStart();
	
	/**
	 writes the input file to the current location with the name "input"
	 */
	void WriteInput();
	
	/**
	 reads the preparsed bertini input file into memory.
	 */
	void ReadDotOut();
	
	/**
	 writes the previously stored preparsed bertini input file to disk.
	 */
	void WriteDotOut();
	
	
	/**
	 writes the previously stored num.out file to disk, replacing the old parameter values with our custom ones.
	 */
	void WriteNumDotOut(double current_params[]);
	
	
	/**
	 sets the working_folder value.
	 */
	void SetWorkingFolder();
	
	
	/**
	 clears the working_folder.
	 */
	void PurgeWorkingFolder();
	
	/**
	 moves into the working folder, and sets in_working_folder = true.
	 */
	void MoveToWorkingFolder();
	
	/**
	 moves to called_dir and sets in_working_folder = false.
	 */
	void GoCalledDir();
	
	
	/**
	 makes sure ready to roll.  helps ensure reliability.
	 */
	void ReadyCheck();
	
	
};








#endif
