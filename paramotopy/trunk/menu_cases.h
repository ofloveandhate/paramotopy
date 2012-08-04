#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include "mtrand.h"
#include "paramotopy_enum.h"
#include "step1_funcs.h"
#include "step2_funcs.h"
#include "step2readandwrite.h"
#include "xml_preferences.h"
#include "master.h"
#include <omp.h>


#ifndef __MENU_CASES__
#define __MENU_CASES__

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






// for running the step2 program from paramotopy
void steptwo_case(ProgSettings paramotopy_settings,
				   runinfo paramotopy_info);


void parallel_case( ProgSettings paramotopy_settings, runinfo paramotopy_info);

void serial_case(ProgSettings paramotopy_settings, runinfo paramotopy_info);
#endif


