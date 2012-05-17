#include <string>

#ifndef __PARA_ENUM__
#define __PARA_ENUM__

enum OPTIONS {Start, Input, SetRandom, SaveRandom, LoadRandom, WriteStepOne, RunStepOne, Step2, FailedPaths ,DetPrefs, Quit};


struct preferences{
	
	std::string machinefile;
	int architecture;
	int usemachine;
	int numprocs;
	int numfilesatatime;
	int saveprogresseverysomany;
	int newfilethreshold;
	int devshm;
	int stifle;
	std::string step2location;
};

struct ToSave{
	
	std::string filename;
	bool saved;
	int filecount;
	
};
#endif
