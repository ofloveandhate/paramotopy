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
#include <omp.h>


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

	 */
	void master_main(ProgSettings & paramotopy_settings,
									 runinfo & paramotopy_info,
									 timer & process_timer);
	
	
	
	
	
	
private:
	std::vector< int > lastnumsent; //progress tracker
	
	int numprocs;
	
	int myid;
	
	int numparam;
	
	std::string filename;
	
	int standardstep2;
	
	int numfilesatatime;
	int numfilesatatime_orig;
	
	int terminationint;
	int current_absolute_index;
	
	
	std::string lastoutfilename;
	std::string tmpfolder;
	std::string called_dir;
	std::string homedir;
	
	std::ofstream mc_out_stream;
	std::ifstream mc_in_stream;
	
	std::vector< int > index_conversion_vector;// for the index making function

	
	std::map< int, bool> active_workers;
	int num_active_workers;
	
	
	
	
	void MasterSetup(ProgSettings & paramotopy_settings,
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
	
	void SendInput(ProgSettings & paramotopy_settings,
																 runinfo & paramotopy_info,
								 timer & process_timer);
	
	void SendStart(ProgSettings & paramotopy_settings,
								 runinfo & paramotopy_info,
								 timer & process_timer);
	
	
	
	
	
	
	
	
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
	void NextRandomValue(runinfo paramotopy_info,
											 int pointcounter, // localcounter tells which indices in tempsends
											 int current_absolute_index,
											 double tempsends[]);
	
	
	
	
	/**
	 * the function for determining the parameter values to send next.
	 * \param numfilesatatime how many parameter points get passed to a worker at the appropriate time.
	 * \param numparam the number of parameters in the problem being solved.
	 * \param pointcounter counts the number of loops.  determines which index in tempsends gets set.
	 * \param Values the parameter discretization values.
	 * \param countingup counter.
	 * \param index_conversion_vector for converting the number corresponding to the parameter point to an index for lookup in Values
	 * \param tempsends the variable in which the parameter points get set, for distribution to the workers via MPI.
	 */
	void FormNextValues(int numparam,
											int pointcounter,
											std::vector< std::vector< std::pair<double,double> > > Values,
											int countingup,
											double tempsends[]);
	
	/**
	 * the user-defined parameter set function for setting the parameter values to send next
	 * \param numfilesatatime how many parameter points get passed to a worker at the appropriate time.
	 * \param numparam the number of parameters in the problem being solved.
	 * \param pointcounter counts the number of loops.  determines which index in tempsends gets set.
	 * \param mc_line_number the line number of the current parameter point, in the mc file.
	 * \param mc_in_stream the file from which we are reading parameter points.
	 * \param tempsends the variable in which the parameter points get set, for distribution to the workers via MPI.
	 */
	void FormNextValues_mc(int numparam,
												 int pointcounter,
												 int mc_line_number,
												 std::ifstream & mc_in_stream,
												 double tempsends[]);

	
	
	
	void GetTerminationInt(runinfo & paramotopy_info,
														 ProgSettings & paramotopy_settings);
	
	
	void OpenMC(runinfo & paramotopy_info,
							ProgSettings & paramotopy_settings);
	
	
	void SetTmpFolder(runinfo & paramotopy_info,
										ProgSettings & paramotopy_settings);
	
	
	void SetUpFolders(runinfo & paramotopy_info);
	
	void TerminateInactiveWorker(int worker_id,
											 timer & process_timer);
	
	void TerminateActiveWorker(int worker_id,
															 timer & process_timer);
	
	void ReadyCheck();
};









#endif
