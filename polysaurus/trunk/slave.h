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

#include "step1.h"
#include "step2.h"
#include "mtrand.h"
#include "random.h"
#include "step2readandwrite.h"

#ifndef __SLAVE_H_INCLUDED__
#define __SLAVE_H_INCLUDED__



extern "C" {
	void computeNumDenom(char **numer, char **denom, char *s);
}


extern "C" {
	int bertini_main(int argC, char *args[]);
}






void slave(std::vector<std::string> dir, 
		   ToSave *TheFiles,
		   int numfiles,
		   std::vector<std::string> ParamNames, 
		   std::string filename,
		   int numfilesatatime);


#endif