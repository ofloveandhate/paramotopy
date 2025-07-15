#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <sstream>
#include <fstream>
#include <string>


#ifndef __STEPONE_H__
#define __STEPONE_H__

#include "paramotopy/random.hpp"
#include "paramotopy/para_aux_funcs.hpp"
#include "paramotopy/xml_preferences.hpp"
#include "paramotopy/runinfo.hpp"




/**
 * calls bertini to run the step1 file.
 * \param paramotopy_settings the bertini and paramotopy settings object.  for writing the input file.
 * \param paramotopy_info the parsed input file.
 */
void CallBertiniStep1(ProgSettings paramotopy_settings, runinfo paramotopy_info);




/**
 * writes the step1 input file, both to the step1 folder (contained in paramotopy_info.location) and to the screen via std::cout .
 * \param paramotopy_settings the bertini and paramotopy settings object.  for writing the input file.
 * \param paramotopy_info the parsed input file.
 */
void WriteStep1(ProgSettings paramotopy_settings,
								runinfo paramotopy_info);





/**
 * a recursive function for writing the entire computer generated parameter point mesh to disk.
 * \param level set internally, determines escape from this recursive call
 * \param Values vector holding the computer-generated parameter values.
 * \param dirfilename the name of the file we are writing to.
 * \param runningline string containing the string so far.  is written to dirfilename when level==Values.size() 
 */
void WriteMeshToMonteCarlo(int level, 
	       std::vector<std::vector<std::pair<double, double> > > Values, 
			   std::string dirfilename, 
			   std::string runningline);



//void WriteMeshToMonteCarlo(int level,
//													 std::vector<std::vector<std::pair<double,double> > > Values,
//													 std::ofstream fout,
//													 std::string cline);



#endif

