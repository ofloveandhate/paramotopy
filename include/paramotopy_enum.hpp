#include <string>

#ifndef __PARA_ENUM__
#define __PARA_ENUM__

enum OPTIONS {Start, Input, SetRandom, SaveRandom, LoadRandom, WriteStepOne, RunStepOne, Step2, FailedPaths, DetPrefs, Quit};



enum MPI_FLAGS {
	NUM_PACKETS,
	NUM_CHARACTERS,
	TERMINATE,
	TEXT_FILE,
	DATA_DOUBLE,
	worker_ITERATION_FINISHED};


/**
 * Struct containing names of files to be collected.
 * \brief Struct for indicating the names of the files to be collected.
 */
struct ToSave{
	
	std::string filename;
	bool saved;
	int filecount;
	int parser_index;
};
#endif
