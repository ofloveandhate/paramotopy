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
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>

#define BOOST_FILESYSTEM_VERSION 3
#define BOOST_FILESYSTEM_NO_DEPRECATED

#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/progress.hpp>
#include <boost/regex.hpp>

#ifndef __TIMING_H__
#define __TIMING_H__




/** the timer_data class is for use in conjunction with the timer class below, via a map.  it holds elapsed time, a start time, and a number of times it has been incremented.  the methods for adjusting these are all set in the timer class.  this merely holds the data 
 \class timer_data 
 \brief Small class, with no methods, for holding timing data.
 */
class timer_data{
	

public:
	
	
	
	/** start time.  set by pushing start. */
	std::chrono::high_resolution_clock::time_point t1;
	
	/** total elapsed time so far.  incremented by adding time. */
	std::chrono::milliseconds elapsed_time;
	
	/** how many times this category has been timed. */
	int num_calls_timed;
	

	
	/** default constructor */
	timer_data(){
		num_calls_timed = 0;
		elapsed_time = std::chrono::milliseconds(0);
	}
	
	
};




/**
* \class timer
 this class defines that which keeps track of timing data.  it uses a map to hold actual time elapsed, and the number of times the timer has been incremented;
 \brief Capable of timing arbitrary categories of processes, using a map of timer_data's.
 */
class timer{
	
public:
	
	
	//data members
	
	
	/** default constructor.  it always pushes start on the total category at instantiation */
	timer(){
	
		timer::press_start("total");
		
	};
	
	
	
 
	/** starts the timer for timer_name
	 * \param timer_name 
	 * internally, sets the value of t1 in the respective timer_data.
	 */
	void press_start(const std::string timer_name);
	
	/**
	 * this is how to add time to a particular timer.
	* \param timer_name string of the name of the timer to be incremented
	* increments both the elapsed time based on t1, and the counter by 1.
	*/
	void add_time(const std::string timer_name);
	
	/**
	 * this is how to add time to a particular timer.
	 * \param timer_name string of the name of the timer to be incremented
	 * \param num_incrementations the number to add to the respective timer_data's num_calls_timed field
	 * increments both the elapsed time based on t1, and the counter by num_incrementations.
	 */
	void add_time(const std::string timer_name, const int num_incrementations);
	
	/**
	 * write all the timing data to disk.
	 * \param folder_to_write_to the folder in which to write the timing data
	 * \param myid a number unique to the process to identify the worker.  designed for use in a multi-process environment.  as long as each process gets a unique number, things will be ok.
	 */
	bool write_timing_data(const boost::filesystem::path folder_to_write_to, const int myid);
	
	
private:
	
	
	//data members
	
	/** map of active timer_data objects. */
	std::map< std::string, timer_data > active_timers;
	
	
	//functions
	
	/** 
	 * add a timer to the map.
	 * \param timer_name name to assign to the timer_data in the map of active_timers.
	 */
	bool create_timer(const std::string timer_name);
	
	
};



//	double start_time;
//	double t1;





#endif

