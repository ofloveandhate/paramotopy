#include "mtrand.h"
#include <iostream>
#include <vector>


void PrintRandom(std::vector<std::pair<float,float> > RandomValues,
		 std::vector<std::string> ParamStrings);

/** Make the random values for the start positions in the parameter
    homotopy.  Based off of the number of parameters.
    @param size - the number of parameters.

*/

std::vector<std::pair<float,float> > MakeRandomValues(int size);


std::vector<std::pair<float, float> > 
  MakeRandomValues(std::vector< std::pair< std::pair< float, float >, 
		   std::pair< float, float > > > RandomRanges);

