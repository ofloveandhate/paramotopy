#include <string>

#ifndef __PARA_ENUM__
#define __PARA_ENUM__

enum OPTIONS {Start, Input, SetRandom, SaveRandom, LoadRandom, WriteStepOne, RunStepOne, Step2, FailedPaths ,DetPrefs, Quit};



//struct preferences{
//	
//	std::string machinefile;
//	int architecture;
//	int usemachine;
//	int numprocs;
//	int numfilesatatime;
//	int saveprogresseverysomany;
//	int newfilethreshold;
//	int devshm;
//	int stifle;
//	std::string step2location;
//};

/**
 * Struct containing names of files to be collected.
 * \brief Struct for indicating the names of the files to be collected.
 */
struct ToSave{
	
	std::string filename;
	bool saved;
	int filecount;
	
};
#endif
