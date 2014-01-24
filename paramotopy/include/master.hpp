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
#include <iomanip>


#ifndef __MASTER_H__
#define __MASTER_H__

#include "step1_funcs.hpp"
#include "step2_funcs.hpp"
#include "mtrand.hpp"
#include "random.hpp"

#include "para_aux_funcs.hpp"
#include "step2readandwrite.hpp"
#include "runinfo.hpp"
#include "timing.hpp"





/**
 *  @class "master_process"
 *
 * process for distribution of parameter points and termination of workers. 
 *
 * \brief master process for basic searches and brute-force runs.
 **/

class master_process{
	
public:
	
//	master_process(communicator){
//		
//		/* Find out how many processes there are in the default
//		 communicator */
//		MPI_Comm_size(MPI_COMM_WORLD, &this->numprocs);
//		
//		MPI_Comm_rank(MPI_COMM_WORLD,&this->myid);
//		
//		
//		this->called_dir = stackoverflow_getcwd();
//		this->homedir = getenv("HOME");
//		
//		this->num_active_workers = 0;
//	}
	
	master_process(){
		
		/* Find out how many processes there are in the default
		 communicator */
		MPI_Comm_size(MPI_COMM_WORLD, &this->numprocs);
		
		MPI_Comm_rank(MPI_COMM_WORLD,&this->myid);
		
		
		this->called_dir = stackoverflow_getcwd();
		this->homedir = getenv("HOME");
		
		this->num_active_workers = 0;
	}
	
	
	/**
	 * the main master process.  to be used by which ever process gets myid==0.  handles the distribution of the parameter points to the workers via MPI.

	 this is the only public method excepting the constructors.
	 
	 \param input_settings     current input settings.
	 \param input_p_info       current parser run info.
	 \param process_timer      MUTABLE current timer, and returns to user for further use.
	 */
	void master_main(ProgSettings input_settings,
									 runinfo input_p_info,
									 timer & process_timer);
	
	
	
	
	
	
private:
	
	runinfo paramotopy_info; ///<  the parsed paramotopy input file.
	ProgSettings paramotopy_settings; ///< the ProgSettings for this process
	
	std::vector< int > lastnumsent; ///< progress tracker
	
	int numprocs;    ///< number of processors in the communicator
	
	int myid;        ///<  my MPI id relative to communicator
	
	int numparam;		 ///< number of parameters in problem
	
	boost::filesystem::path filename;  ///< filename for problem
	
	int standardstep2;  ///< switch for total degree vs. coeff. homotopy
	
	int numfilesatatime;    ///< current number of files passed out at each distribution
	int numfilesatatime_orig; ///< given number of files passed out at each distribution
	
	int terminationint;   ///< maximum absolute_current_index value before loop breaks.
	int current_absolute_index;  ///< the current absolute index.  
	
	
	boost::filesystem::path lastoutfilename;  ///< name of the progress file
	boost::filesystem::path tmpfolder; ///< where the temp files at?
	boost::filesystem::path called_dir;  ///< where was I when I was instantiated?
	boost::filesystem::path homedir; ///< where is $(HOME)?  
	
	std::ofstream mc_out_stream;  ///< parameter point out file
	std::ifstream mc_in_stream; ///< parameter point in file
	
	std::vector< int > index_conversion_vector;// for the index making function

	
	std::map< int, bool> active_workers; ///< map for tracking the active workers.
	int num_active_workers;  ///< parity checker
	
	
	
	/**
	 the master setup function  
	 
	 uses the runinfo and ProgSettings in memory to setup the rest of the necessaries.
	 

	 */
	void MasterSetup();
	
	
	
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
	 sends input file to all workers in the communicator via a for loop MPI_Send
	 
	 \param process_timer      current timer
	 */
	void SendInput(timer & process_timer);
	
	/**
	 sends start file to all workers in communicator via a for loop MPI_Send
	 
	 \param process_timer      current timer
	 */
	void SendStart(timer & process_timer);
	
	
	
	
	
	
	
	
	////////////////////
	//////////////
	//////////
	////////
	//////           Auxiliary functions
	////
	///
	//
	
	
	
	/**
	 * generates a random point in an interval, contained in the runinfo object.
	 * \param paramotopy_info holds configuration info
	 * \param numparam the number of parameters in the problem being solved.
	 * \param pointcounter counts the number of loops.  determines which index in tempsends gets set.
	 * \param tempsends the variable in which the parameter points get set, for distribution to the workers via MPI.
	 */
	void NextRandomValue(int pointcounter, // localcounter tells which indices in tempsends
											 double tempsends[]);
	
	
	
	
	/**
	 * the function for determining the parameter values to send next.
	 
	 * \param numparam the number of parameters in the problem being solved.
	 * \param pointcounter counts the number of loops.  determines which index in tempsends gets set.
	 * \param Values the parameter discretization values.
	 * \param countingup counter.
	 * \param tempsends the variable in which the parameter points get set, for distribution to the workers via MPI.
	 */
	void FormNextValues(int pointcounter,
											std::vector< std::vector< std::pair<double,double> > > Values,
											double tempsends[]);
	
	/**
	 * the user-defined parameter set function for setting the parameter values to send next
	 * \param pointcounter counts the number of loops.  determines which index in tempsends gets set.
	 * \param tempsends the variable in which the parameter points get set, for distribution to the workers via MPI.
	 */
	void FormNextValues_mc(int pointcounter,
												 double tempsends[]);

	
	
	/** 
	 sets the value of this->terminationint
	 
	 */
	void GetTerminationInt();
	
	/**
	 opens the mc_in_stream and mc_out_stream files as necessary 
	 */
	void OpenMC();
	
	
	/**
	 sets the value of this->tmpfolder
	 
	 */
	void SetTmpFolder();
	
	/**
	 creates the folders for the workers, and writes the names of the folders to a file in bfiles_filename/run#/folders
	 
	 */
	void SetUpFolders();
	
	/**
	 sends the TERMINATE tag to an INACTIVE worker, but does not remove them from the list of active workers (cuz this worker shouldn't be on it).
	 */
	void TerminateInactiveWorker(int worker_id,
															 timer & process_timer);
	
	
	/**
	 sends the TERMINATE tag to an ACTIVE worker, and removes it from the list, decrementing the num_active_workers counter.
	 */
	void TerminateActiveWorker(int worker_id,
														 timer & process_timer);
	
	/**
	 just makin' sure we are ready to go before we do go.
	 */
	void ReadyCheck();
};









#endif
