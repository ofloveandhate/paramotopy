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



class point {
	
public:
	
	//variables
	long long index;
	std::vector< std::pair<double,double> > parameter_values;
	std::map< std::string, std::string > collected_data;
	
	
	//functions
	point (){};// constructor
	
	bool operator<(const point & p2 ) const { return this->index < p2.index; }  //define the comparison for sorting.
	
	
private:
	
};//re: class point


#endif




