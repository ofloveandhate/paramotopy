//contains the point class
#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>


#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/progress.hpp>
#include <boost/regex.hpp>

#ifndef __POINT_H__
#define __POINT_H__


/**
 * Class for holding a parameter point.
 *
 */
class point {
	
	/** default constructor */
	point (){};
	
public:
	
	//data members
	
	/**  line-number of the point */
	long long index;
	/** actual parameter coordinates */
	std::vector< std::pair<double,double> > parameter_values;
	/** map, indexed by filename (e.g. nonsingular_solutions), containing the collected data in string form. */
	std::map< std::string, std::string > collected_data;
	
	
	//functions

	/** define the comparison for sorting order. */
	bool operator<(const point & p2 ) const { return this->index < p2.index; }  
	
	
private:
	
};//re: class point


#endif




