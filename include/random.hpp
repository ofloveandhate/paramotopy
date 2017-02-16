#include "mtrand.hpp"
#include <iostream>
#include <vector>

/** Print the random values. 
 \param RandomValues - The random values.
 \param ParamStrings - The parameter names as strings. */

void PrintRandom(std::vector<std::pair<double,double> > RandomValues,
		 std::vector<std::string> ParamStrings);

/** Make the random values for the start positions in the parameter
    homotopy.  Based off of the number of parameters.
    \param size - the number of parameters.

*/

//std::vector<std::pair<double,double> > MakeRandomValues(int size);
//
//
//std::vector<std::pair<double, double> > 
//  MakeRandomValues(std::vector< std::pair< std::pair< double, double >, 
//		   std::pair< double, double > > > RandomRanges);
//
