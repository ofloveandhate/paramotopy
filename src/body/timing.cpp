#include "timing.hpp"



void timer::press_start(const std::string timer_name){
	
	timer::create_timer(timer_name);
	this->active_timers[timer_name].t1 = std::chrono::high_resolution_clock::now();
	return;
}

void timer::add_time(const std::string timer_name){
	
	if (timer::create_timer(timer_name)){
		std::cerr << "tried to add time to a counter which didn't exist!" << std::endl;
		exit(909);
	}
	
	this->active_timers[timer_name].elapsed_time += std::chrono::duration_cast<std::chrono::milliseconds>(
																										  std::chrono::high_resolution_clock::now() - this->active_timers[timer_name].t1);
	this->active_timers[timer_name].num_calls_timed++;
	
	return;
}
//aa
void timer::add_time(const std::string timer_name, const int num_incrementations){
	
	if (timer::create_timer(timer_name)){
		std::cerr << "tried to add time to a counter which didn't exist!" << std::endl;
		exit(910);
	}
	
	this->active_timers[timer_name].elapsed_time += std::chrono::duration_cast<std::chrono::milliseconds>(
																										  std::chrono::high_resolution_clock::now() - this->active_timers[timer_name].t1);
	this->active_timers[timer_name].num_calls_timed += num_incrementations;
	
	return;
}


bool timer::create_timer(const std::string timer_name){
	
	if (this->active_timers.find( timer_name ) == this->active_timers.end()) {
		this->active_timers[timer_name] = timer_data();  //instantiate.
		return true;
	}
	else{
		return false;
	}
	
}





bool timer::write_timing_data(const boost::filesystem::path folder_to_write_to, const int myid){
	
	boost::filesystem::path file_to_write_to = folder_to_write_to;
	
	
	
	std::stringstream converter;
	converter << myid;
	
	if (myid==0) {
		file_to_write_to /= "controller";
	}
	else{
		file_to_write_to /= "worker";
	}
	file_to_write_to += "timing";
	
	
	if (myid==0) {
	}
	else{
		file_to_write_to += converter.str();
	}
	
	std::ofstream fout(file_to_write_to.c_str());
	if (!fout.is_open()) {
		std::cerr << "failed to open " << file_to_write_to.string() << " to write timing data" << std::endl;
		return false;
	}
	
	
	
	std::map< std::string, timer_data >::iterator iter;
	fout << myid << std::endl;
	
	timer::add_time("total");
	
	for (iter = this->active_timers.begin(); iter != this->active_timers.end(); iter++) {
		fout << iter->first << ": " << iter->second.elapsed_time.count() << " " << iter->second.num_calls_timed << std::endl;
	}
	
	fout.close();
	return true;
}



