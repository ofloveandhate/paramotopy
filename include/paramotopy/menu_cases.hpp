#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include "paramotopy/mtrand.hpp"
#include "paramotopy/paramotopy_enum.hpp"
#include "paramotopy/step1_funcs.hpp"
#include "paramotopy/step2_funcs.hpp"
#include "paramotopy/step2readandwrite.hpp"
#include "paramotopy/xml_preferences.hpp"
#include "paramotopy/controller.hpp"


#ifndef PARAMOTOPY_MENU_CASES_HPP
#define PARAMOTOPY_MENU_CASES_HPP



// for running the step2 program from paramotopy
void steptwo_case(ProgSettings paramotopy_settings,
		  runinfo paramotopy_info);


void parallel_case( ProgSettings paramotopy_settings, runinfo paramotopy_info);

void serial_case(ProgSettings paramotopy_settings, runinfo paramotopy_info);
#endif


