#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include "mtrand.hpp"
#include "paramotopy_enum.hpp"
#include "step1_funcs.hpp"
#include "step2_funcs.hpp"
#include "step2readandwrite.hpp"
#include "xml_preferences.hpp"
#include "master.hpp"


#ifndef __MENU_CASES__
#define __MENU_CASES__

/*
 the computeNumdenom function is in the bertini library.  it takes in a character array, and pointers which return the numerator
 and denominator of the number.  I pass the input by casting via (char *)
 */
//extern "C" {
//	void computeNumDenom(char **numer, char **denom, char *s);
//}

//#include <bertini.h>
//





// for running the step2 program from paramotopy
void steptwo_case(ProgSettings paramotopy_settings,
		  runinfo paramotopy_info);


void parallel_case( ProgSettings paramotopy_settings, runinfo paramotopy_info);

void serial_case(ProgSettings paramotopy_settings, runinfo paramotopy_info);
#endif


