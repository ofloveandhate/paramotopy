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
#include "controller.hpp"


#ifndef PARAMOTOPY_MENU_CASES_HPP
#define PARAMOTOPY_MENU_CASES_HPP



// for running the step2 program from paramotopy
void steptwo_case(ProgSettings paramotopy_settings,
		  runinfo paramotopy_info);


void parallel_case( ProgSettings paramotopy_settings, runinfo paramotopy_info);

void serial_case(ProgSettings paramotopy_settings, runinfo paramotopy_info);
#endif


