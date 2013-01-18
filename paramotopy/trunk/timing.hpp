#include <mpi.h>

#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

//#include "step1_funcs.hpp"
//#include "step2_funcs.hpp"
//#include "mtrand.hpp"
//#include "random.hpp"
//#include "step2readandwrite.hpp"
//#include "master.hpp"
//#include "slave.hpp"
//#include "para_aux_funcs.hpp"

#define BOOST_FILESYSTEM_VERSION 3
#define BOOST_FILESYSTEM_NO_DEPRECATED

#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/progress.hpp>
#include <boost/regex.hpp>

#ifndef __TIMING_H__
#define __TIMING_H__


/// this class defines that which keeps track of timing data.  it uses a map to hold actual time elapsed, and the number of times the timer has been incremented;

class timer_data{
	
public:
	double t1;
	double elapsed_time;
	int num_calls_timed;
	
	timer_data(){
		num_calls_timed = 0;
		elapsed_time = 0;
	}
	
};





class timer{
	
public:
	
	
	//data members
	
	
	//default constructor
	timer(){
	
		timer::press_start("total");
		
	};
	
	
	
	//input: none
	//output: none externally.  internally, sets the value of t1.
	void press_start(const std::string timer_name);
	
	//input: string of the name of the timer to be incremented
	//output: none external to the object.  increments both the elapsed time based on t1, and the counter by 1.
	void add_time(const std::string timer_name);
	
	void add_time(const std::string timer_name, const int num_incrementations);
	
	bool write_timing_data(const boost::filesystem::path folder_to_write_to, const int myid);
	
	
private:
	
	
	//data members
	std::map< std::string, timer_data > active_timers;
	double start_time;
	double t1;
	
	//functions
	bool create_timer(const std::string timer_name);
	
	
};







#endif

