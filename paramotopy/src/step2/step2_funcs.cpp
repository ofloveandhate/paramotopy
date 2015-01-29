#include "step2_funcs.hpp"



void parse_input_file_bertini(unsigned int & currentSeed, int & MPType){
	int trackType, genType, userHom, sharpenOnly, needToDiff, remove_temp, useParallelDiff;
	parse_input(const_cast<char *>("input"), &trackType, &MPType, &genType, &userHom, &currentSeed, &sharpenOnly, &needToDiff, &remove_temp, useParallelDiff, 0, 1, 0);
}



void run_zero_dim_main(int MPType, unsigned int currentSeed){	
	zero_dim_main(MPType, (double) 0, currentSeed, const_cast<char *>("start"), 0,1,0);
}







//creates a string containing the file for step2 input file. note that the specific points at which we solve are written to the num.out file by each worker, so this is only written once.  then the associated .out files are created in a step2.1 solve, and left intact for the step2.2 solves, of which there are many.
std::string WriteStep2(std::vector<std::pair<double, double> > CurrentValues,
		       ProgSettings paramotopy_settings,
		       runinfo paramotopy_info)
{
  

	std::stringstream inputstringstream;
	inputstringstream << paramotopy_settings.WriteConfigStepTwo();
	
	inputstringstream << paramotopy_info.WriteInputStepTwo(CurrentValues,
																												 paramotopy_settings.settings["mode"]["standardstep2"].intvalue == 1);
	
	return inputstringstream.str();
}











