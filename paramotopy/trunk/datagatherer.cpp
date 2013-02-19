//contains the datagatherer class.

#include "datagatherer.hpp"



bool datagatherer::GatherDataForFails(std::vector< point > terminal_fails, std::vector< point > & successful_resolves){
	
	
	
	//a sanity check
	if (this->run_to_analyze=="") {
		std::cerr << "run_to_analyze not set yet :(" << std::endl;
		return false;
	}
	
	
	
	GetFilesToParse(this->run_to_analyze, this->gather_savefiles, this->gather_parser_indices);  //in para_aux_funcs
	//sets the gather_savefiles and gather_parser_indices, based on the c1 folder
	
	std::vector< std::string > folders_with_data = GetFoldersForData(run_to_analyze.string());
	
	for (int ii=0;ii<int(gather_savefiles.size());++ii){
		datagatherer::CollectSpecificFiles(gather_savefiles[ii], folders_with_data, run_to_analyze.string(), gather_parser_indices[ii], false);
	}
	
	//at this point, we should have the run_to_analyze/gathered_data/finalized folder, which will contain exactly one file for each of the user-specified saved files, in order, with all the data for the run.  just need to parse it.
	
	boost::filesystem::path folder_with_data = this->run_to_analyze;
	folder_with_data /= "gathered_data/finalized";
	
	std::vector < point > current_data = datagatherer::GatherFinalizedDataToMemory(folder_with_data);
	
	std::cout << "comparing initial to gathered" << std::endl;
	
	successful_resolves = CompareInitial_Gathered(terminal_fails, current_data);
	std::cout << "done comparing initial to gathered" << std::endl;
	return true;
	
}


std::vector< point > datagatherer::CompareInitial_Gathered(std::vector< point > terminal_fails, std::vector< point > current_data){
	std::vector< point > successful_resolves;
	
	
	
	if (terminal_fails.size() == current_data.size()) {
		std::cout << "no points successfully resolved" << std::endl;
	}
	else{
		std::vector< int > flag_if_success;
		flag_if_success.resize(current_data.size());
		
		for (int ii=0; ii<current_data.size(); ++ii) {
			flag_if_success[ii]=1;
		}
		
		
		successful_resolves.resize(current_data.size()-terminal_fails.size());
		
		for (int ii=0; ii<int(terminal_fails.size()); ++ii) {
			flag_if_success[terminal_fails[ii].index] = 0;
		}
		
		int counter=0;
		for (int ii=0; ii<int(current_data.size()); ++ii) {
			if (flag_if_success[ii]==1){
				successful_resolves[counter].index = current_data[ii].index;
				successful_resolves[counter].collected_data = current_data[ii].collected_data;
				
				counter++;
			}
		}
		
		
	}
	
	
	return successful_resolves;
}


////gather the data from a completed, gathered, finalized step2, to memory.
std::vector < point > datagatherer::GatherFinalizedDataToMemory(boost::filesystem::path folder_with_data){
	
	
	std::map <std::string, std::vector< point > > temp_point_storage;
	std::vector< point > master_point_storage;
	
	
	for (int ii = 0; ii< int(this->gather_savefiles.size()); ++ii) {
		
		boost::filesystem::path currentfile = folder_with_data;
		currentfile /= this->gather_savefiles[ii] + "0";
		std::vector < point > gathered_data;
		
		//std::cout << "opening file " << currentfile.string() << " to read data" << std::endl;//remove me?
		std::ifstream datafile(currentfile.string().c_str());
		
		if (!datafile.is_open()) {
			std::cerr << "failed to open " << currentfile.string() << std::endl;
			return master_point_storage;
		}
		
		std::string tmpstr;
		
		getline(datafile,tmpstr);  //waste the parameter names
		int index;
		std::string data;
		while (!datagatherer::ReadPoint(datafile, index, data, gather_parser_indices[ii])) {
			point tmppoint;
			tmppoint.index = index;
			tmppoint.collected_data[gather_savefiles[ii]] = data;
			gathered_data.push_back(tmppoint);
		}
		datafile.close();
		
		std::sort(gathered_data.begin(), gathered_data.end()); //pick up here tomorrow morning.
		temp_point_storage[gather_savefiles[ii]] = gathered_data;
	}
	
	master_point_storage.resize(temp_point_storage[gather_savefiles[0]].size());
	
	
	//now put them together, return.
	
	for (int ii=0; ii< int (temp_point_storage[gather_savefiles[0]].size()); ++ii) {
		point tmppoint;
		tmppoint.index = temp_point_storage[gather_savefiles[0]][ii].index;
		
		for (int jj=0; jj< int(gather_savefiles.size()); ++jj){
			tmppoint.collected_data[gather_savefiles[jj]] = temp_point_storage[gather_savefiles[jj]][ii].collected_data[gather_savefiles[jj]];
		}
		master_point_storage[ii] = tmppoint;
	}
	
	std::sort(master_point_storage.begin(), master_point_storage.end());
	
	
	return master_point_storage;
}









void datagatherer::GatherDataFromMenu(){
	
	boost::filesystem::path run_to_analyze = datagatherer::GetAvailableRuns();
	if (run_to_analyze=="") {
		return;
	}
	
	///////////////////////////////
	//get the file type to analyze
	//////////////////////////////
	
	
	
	
	
	GetFilesToParse(run_to_analyze, this->gather_savefiles, this->gather_parser_indices);  //in para_aux_funcs
	//sets the gather_savefiles and gather_parser_indices, based on the c1 folder
	
	std::vector< std::string > folders_with_data = GetFoldersForData(run_to_analyze.string());
	
	for (int ii=0;ii<int(gather_savefiles.size());++ii){
		std::cout << "collecting " << gather_savefiles[ii] << std::endl;
		double t1 = omp_get_wtime();
		datagatherer::CollectSpecificFiles(gather_savefiles[ii], folders_with_data, run_to_analyze.string(), gather_parser_indices[ii], true);
		std::cout << "sorting " << gather_savefiles[ii] << " took " << omp_get_wtime()-t1 << " seconds." << std::endl;
	}
	
	
	return;
}






void datagatherer::CollectSpecificFiles(std::string file_to_gather, std::vector < std::string > folders_with_data, std::string run_to_analyze, int parser_index, bool mergefailed){
	
	
	//now need to merge-sort method of gathering data, since the data is distributed in a collection of folders and files, in monotone order.  we will do this in files, since the data may be larger than available ram.
	std::string output_folder_name = "unset_folder_name";
	int output_folder_index = -1;
	
	std::string base_output_folder_name = run_to_analyze;
	base_output_folder_name.append("/gathered_data");
	boost::filesystem::create_directory(base_output_folder_name);
	
	
	
	std::vector< bool> current_folders_are_basic, next_folders_are_basic;
	
	std::vector< std::string > next_folders = folders_with_data; // seed
	std::vector< std::string > current_folders;
	
	for (int ii=0; ii<folders_with_data.size(); ++ii) {
		next_folders_are_basic.push_back(true);
	}
	
	int current_num_folders;
  int level = 0;
	while (next_folders.size()>1){
		
		current_folders = next_folders;
		current_num_folders = current_folders.size();
		current_folders_are_basic = next_folders_are_basic;
		
		next_folders.clear();
		next_folders_are_basic.clear();
		
		
		for (int jj=0; jj< ( current_num_folders - (current_num_folders%2) ) ; jj=jj+2) {
			datagatherer::IncrementOutputFolder(output_folder_name, base_output_folder_name, output_folder_index);
			boost::filesystem::create_directory(output_folder_name);
			std::cout << "merging " << current_folders[jj] << " " << current_folders[jj+1] << " lvl " << level << " iteration " << jj/2+1 << std::endl;
			datagatherer::MergeFolders( file_to_gather, current_folders[jj], current_folders[jj+1], output_folder_name,parser_index);
			
			
			next_folders_are_basic.push_back(false);
			next_folders.push_back(output_folder_name); //put on the list for next level of sorting
			
			if ( current_folders_are_basic[jj]==false ) { //if not on the first level (that is, to leave the raw unsorted data intact), remove the data.
				boost::filesystem::remove_all(current_folders[jj]);  //remove the previous step's files
				// i reiterate how awesome the boost library is.
			}
			
			if ( current_folders_are_basic[jj+1]==false ) { //if not on the first level (that is, to leave the raw unsorted data intact), remove the data.
				boost::filesystem::remove_all(current_folders[jj+1]);  //remove the previous step's files
				// i reiterate how awesome the boost library is.
			}
			
			
		}
		
		if (  ( (current_num_folders%2) == 1)   ) { //if there is an odd man out.
			next_folders.push_back(current_folders[current_num_folders-1]); // put the last folder on (-1) for zero indexing
			if (current_folders_are_basic[current_num_folders-1]==true) {
				next_folders_are_basic.push_back(true);
			}
		}
		level++;
	}
	
	std::string source_folder;
	source_folder = next_folders[0]; // set this for finalizing.
	bool source_folder_is_basic = next_folders_are_basic[0];
	
	
	datagatherer::finalize_run_to_file(file_to_gather, source_folder, base_output_folder_name, parser_index, mergefailed);
	
	std::cout << "done finalizing" << std::endl;  // removeme
	
	if (source_folder_is_basic==false) {
		boost::filesystem::remove_all(source_folder);  //remove the previous step's files
	}
	std::cout << "done removing" << std::endl;  // removeme
	return;
}




std::map< int, point > datagatherer::ReadSuccessfulResolves(){
	
	std::map< int, point > successful_resolves;
	
	
	boost::filesystem::path filetoopen = this->base_dir;
	filetoopen /= "failure_analysis/resolved_file";
	
	if (!boost::filesystem::exists(filetoopen)) {
		std::cout << filetoopen.string() << " does not exist, for reading in successful re-solves." << std::endl;
		return successful_resolves;
	}
	
	std::string resolved_file_name;
	
	std::ifstream sourcefile(filetoopen.string().c_str());
	getline(sourcefile,resolved_file_name);
	sourcefile.close();
	
	
	filetoopen = resolved_file_name;
	
	std::cout << filetoopen.string() << " opening to read in successful resolves." << std::endl;
	
	if (!boost::filesystem::exists(filetoopen)) {
		std::cerr << "file " << filetoopen.string() << " doesn't exist, even though it should." << std::endl;
		return successful_resolves;
	}
	
	
	sourcefile.open(resolved_file_name.c_str());
	
	std::string tmpstr;
	std::stringstream converter;
	int num_attempted_resolves;
	
	
	getline(sourcefile,tmpstr);
	converter << tmpstr;
	converter >> num_attempted_resolves;
	
	std::map < std::string, int > parsermap;
	parsermap["real_solutions"] = 1;
	parsermap["nonsingular_solutions"] = 1;
	parsermap["singular_solutions"] = 1;
	parsermap["raw_data"] = -1;
	parsermap["raw_solutions"] = -1;
	parsermap["main_data"] = -1;
	parsermap["midpath_data"] = -1;
	parsermap["failed_paths"] = 2;
	
	
	for (int ii=0; ii<num_attempted_resolves; ++ii) {
		point tmppoint;
		int next_index, num_files_to_collect;
		std::string next_data;
		std::stringstream converter;
		getline(sourcefile,tmpstr);
		converter << tmpstr;
		converter >> next_index;
		converter >> num_files_to_collect;
		
		converter.clear();
		converter.str("");
		
		tmppoint.index = next_index;
		
		if (num_files_to_collect>0) {
			std::vector< std::string > filename;
			filename.resize(num_files_to_collect);
			getline(sourcefile, tmpstr);
			converter << tmpstr;
			for (int jj = 0; jj<num_files_to_collect; ++jj) {
				converter >> filename[jj];
			}
			
			
			for (int kk=0; kk<num_files_to_collect; ++kk) {
				ReadPoint(sourcefile, tmppoint.collected_data[filename[kk]], parsermap[filename[kk]]);
			}
		}
		
		
		successful_resolves[next_index] = tmppoint;
		
		
	}
	
	sourcefile.close();
	
	return successful_resolves;
	
	
	
}


void datagatherer::WriteUnsuccessfulResolves(std::map< int, point> successful_resolves){
	
	std::map<int, point>::iterator iter;
	
	boost::filesystem::path persistentfails_name = this->base_dir;
	persistentfails_name /= "gathered_data/finalized/persistent_fails";
	
	std::ofstream persistentfails(persistentfails_name.string().c_str());
	if (!persistentfails.is_open()) {
		std::cerr << "failed to open " << persistentfails_name.string() << std::endl;
		return;
	}
	
	for (iter=successful_resolves.begin(); iter!=successful_resolves.end(); iter++) {
		if (iter->second.collected_data.size()==0) {
			persistentfails << iter->first << "\n";
		}
	}
	
	persistentfails.close();
	return;
}


void datagatherer::finalize_run_to_file(std::string file_to_gather, std::string source_folder,std::string base_output_folder_name, int parser_index, bool mergefailed){
	
	
	boost::filesystem::path output_file_name = base_output_folder_name;
	output_file_name /= "finalized";
	
	
	boost::filesystem::create_directory(output_file_name);
	
	output_file_name /= file_to_gather;
	output_file_name += "0";
	
	boost::filesystem::remove(output_file_name);//remove the old file
	
	
	
	std::string expression = "^";  //specify beginning of string
	expression.append(file_to_gather);
	std::vector < std::string > filelist = FindFiles(source_folder, expression);  //this function is in para_aux_funcs
	
	
	std::ofstream output_file(output_file_name.string().c_str());
	std::string tmpstr;
	
	
	//initialize
	int previous_index = -1;
	int next_index = 0;
	std::string next_data = "";
	
	
	std::map< int,  point > successful_resolves;
	if (mergefailed) {
		successful_resolves = datagatherer::ReadSuccessfulResolves();
		datagatherer::WriteUnsuccessfulResolves(successful_resolves);
	}
	
	bool passed_check = true;
	
	for (int ii=0; ii< int(filelist.size()); ++ii) {
		std::ifstream sourcefile(filelist[ii].c_str());
		
		if (!sourcefile.is_open()) {
			std::cerr << "error: " << filelist[ii] << " failed to open" << std::endl;
		}
		
		getline(sourcefile,tmpstr);  //waste the parameter line
		if (ii==0) {
			output_file << tmpstr << "\n";
		}
		
		
		while (! (datagatherer::ReadPoint(sourcefile, next_index, next_data, parser_index))  ) {
			
			
			//check the integrity of the data
			if (next_index!=(previous_index+1)){
				std::cerr << "index mismatch, filename " << file_to_gather	<< ", with indices " << previous_index << " & " << next_index << std::endl;
				passed_check = false;
			}
			
			if (mergefailed) {
				datagatherer::CheckForResolvedFailedPoint(file_to_gather,next_index,next_data,successful_resolves);
			}
			
			output_file << next_index << "\n" << next_data;
			previous_index = next_index;
			
			
		}
		sourcefile.close();
	}
	
	
	output_file.close();
	
	std::cout << "done finalizing file " << file_to_gather << "." << std::endl;
	
	if (passed_check) {
		std::cout << "passed integrity check for in-orderness of points." << std::endl;
	}
	else{
		std::cout << "failed integrity check.  one or more points were out of order or missing." << std::endl;
		std::cout << "todo:  keep record of missing points" << std::endl; //remove me when completed
	}
	
	
	return;
}



void datagatherer::CheckForResolvedFailedPoint(std::string file_to_gather,int next_index,std::string & next_data,std::map< int, point> successful_resolves){
	
	if (successful_resolves.find(next_index) ==successful_resolves.end()) {
		
		
		return;
	}
	else{
		if (successful_resolves[next_index].collected_data.find(file_to_gather)!=successful_resolves[next_index].collected_data.end() ){
			next_data = successful_resolves[next_index].collected_data[file_to_gather];
			std::cout <<  next_index << " replaced with resolved data." << std::endl;
		}
		else{
			return;
		}
	}
	
	return;
}

boost::filesystem::path datagatherer::GetAvailableRuns(){
	///////////////////////////////
	//get the runs available
	//////////////////////////////
	
	boost::filesystem::path run_to_analyze = "";
	
	std::vector< boost::filesystem::path > found_runs = FindDirectories(this->fundamental_dir, "^run");
	std::vector< boost::filesystem::path > completed_runs;
	
	
	int completed_counter = 0;
	for	(int ii = 0; ii<int(found_runs.size()); ii++){
		if ( TestIfFinished(found_runs[ii])==true){ // in para_aux_funcs
			
			completed_runs.push_back(found_runs[ii]);
			std::cout << completed_counter << ": " << found_runs[ii].string() << "\n";
			completed_counter++;
		}
		else{
			
		}
	}
	
	if (completed_counter==0){
		std::cout << "found no completed runs.  sorry.\n";
		return run_to_analyze;
	}
	
	
	if (completed_counter==1){
		run_to_analyze = completed_runs[0];
	}
	else{
		int choice = get_int_choice("which run?\n: ",0,completed_counter-1);
		run_to_analyze = completed_runs[choice];
	}
	
	return run_to_analyze;
}




void datagatherer::MergeFolders(std::string file_to_gather, std::string left_folder, std::string right_folder, std::string output_folder_name, int parser_index){
	
	std::stringstream converter;
	
	std::string output_file_name = output_folder_name;
	output_file_name.append("/");
	output_file_name.append(file_to_gather);
	output_file_name.append("0");
	
	//std::cout << "opening file " << output_file_name << " to write data" << std::endl; //remove me
	std::ofstream outputfile(output_file_name.c_str());
	
	if (!outputfile.is_open()) {
		std::cerr << "outputfile " << output_file_name << " failed to open for writing results of merge." << std::endl;
	}
	
	
	
	std::string expression = "^"; //specify the beginning of the string (regex)
	expression.append(file_to_gather);
	
	int file_index_left = 0, file_index_right = 0;//, file_index_output = -1;
	
	int next_index_left, next_index_right;//, parameter_index_left= -2, parameter_index_right = -2,  current_index=-1;
	
	std::string output_buffer;
	
	std::vector < std::string > filelist_left  = FindFiles(left_folder,  expression);  //this function is in para_aux_funcs
	std::vector < std::string > filelist_right = FindFiles(right_folder, expression);  //this function is in para_aux_funcs
	
	
	if (filelist_left.size()==0) {
		std::cerr << "folder to merge '" << left_folder << "' had no data files!" << std::endl;
		return;
	}
	if (filelist_right.size()==0) {
		std::cerr << "folder to merge '" << right_folder << "' had no data files!" << std::endl;
		return;
	}
	
	
	std::string tmp_left, tmp_right;  //temporary strings for holding data.
	std::string next_data_left, next_data_right;
	
	std::ifstream datafile_left( filelist_left[0].c_str());
	std::ifstream datafile_right(filelist_right[0].c_str());
	
	if (!datafile_left.is_open()) {
		std::cerr << filelist_left[0] << " failed to open for merging" << std::endl;
	}
	if (!datafile_right.is_open()) {
		std::cerr << filelist_right[0] << " failed to open for merging" << std::endl;
	}
	
	getline(datafile_left,tmp_left);//read the parameters
	getline(datafile_right,tmp_right);//read the parameters.  even "empty" data files contain this line.
	
	
	bool keep_going_left, keep_going_right, next_read_left, next_read_right;
	
	
	
	if (datafile_left.eof()) {
		keep_going_left = false;
	}
	else{
		keep_going_left = true;
		next_read_left = true; // to seed the merge
	}
	
	if (datafile_right.eof()) {
		keep_going_right = false;
	}
	else{
		keep_going_right = true; // to seed the merge
		next_read_right = true;
	}
	
	
	output_buffer.append(tmp_left); //make the top line be the parameter names, to conform to the standard
	output_buffer.append("\n");
	
	
	while ( 1 ) {
		
		
		if (next_read_left){ // read the left file
			datagatherer::ReadPoint(datafile_left, next_index_left, next_data_left, parser_index);
		}
		if (next_read_right) { //read the right file
			datagatherer::ReadPoint(datafile_right, next_index_right, next_data_right, parser_index);
		}
		
		
		while ( datafile_left.eof() && keep_going_left  ) {
			keep_going_left  = datagatherer::endoffile_stuff(datafile_left,file_index_left,filelist_left);
			datagatherer::ReadPoint(datafile_left, next_index_left, next_data_left, parser_index);
		}
		while (datafile_right.eof() && keep_going_right ) {
			keep_going_right = datagatherer::endoffile_stuff(datafile_right,file_index_right,filelist_right);
			datagatherer::ReadPoint(datafile_right, next_index_right, next_data_right, parser_index);
		}
		
		
		
		
		if ( (!keep_going_left) || (!keep_going_right) ){
			
			
			//append the last point
			if (!keep_going_left) {
				
				converter << next_index_right;
				output_buffer.append(converter.str());
				converter.clear();
				converter.str("");
				output_buffer.append("\n");
				output_buffer.append(next_data_right);
			}
			
			
			if (!keep_going_right){
				
				converter << next_index_left;
				output_buffer.append(converter.str());
				converter.clear();
				converter.str("");
				output_buffer.append("\n");
				output_buffer.append(next_data_left);
			}
			
			
			break; //break the while loop
		}
		
		
		
		
		
		if (next_index_left< next_index_right) {
			converter << next_index_left;
			output_buffer.append(converter.str());
			converter.clear();
			converter.str("");
			output_buffer.append("\n");
			output_buffer.append(next_data_left);
			next_read_left = true;
			next_read_right = false;
		}
		else{ // next_index_right < next_index_left
			converter << next_index_right;
			output_buffer.append(converter.str());
			converter.clear();
			converter.str("");
			output_buffer.append("\n");
			output_buffer.append(next_data_right);
			next_read_left = false;
			next_read_right = true;
		}
		
		
		
		if (output_buffer.size()>67108864) {
			
			outputfile << output_buffer;
			output_buffer = "";
		}
		
		
	}//re: while
	
	
	
	
	
	//now may have one file to keep reading
	if (keep_going_left) {
		datagatherer::rest_of_files(datafile_left, output_buffer, outputfile, filelist_left, file_index_left, parser_index);
	}
	
	if (keep_going_right) {
		datagatherer::rest_of_files(datafile_right, output_buffer, outputfile, filelist_right, file_index_right, parser_index);
	}
	
	
	
	//dump the rest of the buffer into the file
	outputfile << output_buffer;
	outputfile.close();
	
	return;
}







void datagatherer::rest_of_files(std::ifstream & datafile, std::string & output_buffer, std::ofstream & outputfile, std::vector < std::string > filelist, int file_index, int parser_index){
	std::string tmpstr;
	std::stringstream converter;
	std::string next_data = "";
	int next_index;
	
	//still have one file to read which is open, plus others that may be not yet read.
	while (! (datagatherer::ReadPoint(datafile, next_index, next_data, parser_index))  ) {
		converter << next_index << "\n";
		output_buffer.append(converter.str());
		converter.clear();
		converter.str("");
		output_buffer.append(next_data);
		
		
		if (output_buffer.size()>67108864){
			outputfile << output_buffer;
			output_buffer = "";
		}
		
	}
	
	datafile.close();
	
	
	for (int ii=file_index+1; ii < filelist.size(); ++ii) {
		
		datafile.open(filelist[ii].c_str());
		getline(datafile,tmpstr); // waste the parameter name line
		
		while (! (datagatherer::ReadPoint(datafile, next_index, next_data, parser_index))  ) {
			converter << next_index << "\n";
			output_buffer.append(converter.str());
			converter.clear();
			converter.str("");
			output_buffer.append(next_data);
			
			
			if (output_buffer.size()>67108864){
				outputfile << output_buffer;
				output_buffer = "";
			}
		}
		datafile.close();
	}
	
	
	return;
}


bool datagatherer::endoffile_stuff(std::ifstream & datafile, int & file_index, std::vector < std::string > filelist){
	std::string tmpstr;
	datafile.close();
	file_index++;
	if (file_index+1>filelist.size()) {  // the +1 is to compensate for zero-centered indexing...
		return false;
	}
	else{
		std::cout << "opening " << filelist[file_index] << std::endl;
		datafile.open(filelist[file_index].c_str());
		
		if (!datafile.is_open()) {
			std::cerr << "failed to open " << filelist[file_index] << " to read for data" << std::endl;
		}
		
		getline(datafile,tmpstr); //waste the top line (parameter names)
		return true;
	}
	
}

bool datagatherer::ReadPoint(std::ifstream & fin, std::string & data, int parser_index){
	
	
	std::string tmpstr;
	std::stringstream converter;
	
	
	getline(fin, data);  //gets the parameter line
	data.append("\n");
	switch (parser_index) {
		case 1:
			data.append(datagatherer::ParseSolutionsFile(fin));
			break;
			
		case 2:
			data.append(datagatherer::ParseFailedPaths(fin));
			break;
			
		default:
			std::cerr << "invalid parser index " << parser_index << std::endl;  // this should never happen
			break;
			
	}
	
	if (fin.eof()){
		return true;
	}
	
	
	return false;
	
}


bool datagatherer::ReadPoint(std::ifstream & fin, int & next_index, std::string & data, int parser_index){
	std::string tmpstr;
	std::stringstream converter;
	
	getline(fin, tmpstr);  //get the line number
	
	
	
	converter << tmpstr;
	converter >> next_index;
	converter.clear();
	converter.str("");
	
	getline(fin, data);  //gets the parameter line
	data.append("\n");
	switch (parser_index) {
		case 1:
			data.append(datagatherer::ParseSolutionsFile(fin));
			break;
			
		case 2:
			data.append(datagatherer::ParseFailedPaths(fin));
			break;
			
		default:
			std::cerr << "invalid parser index " << parser_index << std::endl;  // this should never happen
			break;
			
	}
	
	if (fin.eof()){
		return true;
	}
	
	
	return false;
	
}

std::vector< std::vector < std::pair< std::string, std::string > > > datagatherer::ParseSolutionsFile_ActualSolutions(std::ifstream & fin){
	
	std::string tmpstr;
	int number_of_solutions;
	getline(fin,tmpstr);
	
	std::vector< std::vector < std::pair< std::string, std::string > > > solutions;
	
	//  first we read in the number of solutions.
	
	std::stringstream converter;
	converter << tmpstr;  //get the line number
	converter >> number_of_solutions;
	converter.clear(); //reset
	converter.str("");
	
	getline(fin,tmpstr); //burn one line
	
	for (int solution_counter = 0; solution_counter <  number_of_solutions; ++solution_counter) {
		std::vector < std::pair< std::string, std::string > > temporary_solution;//allocate the solution.
		//read in each solution
		
		for (int variable_counter = 0; variable_counter < this->numvariables; ++variable_counter) {
			std::pair < std::string, std::string > temporary_coordinates;
			
			getline(fin,tmpstr);
			
			converter << tmpstr;
			
			converter >> temporary_coordinates.first;
			converter >> temporary_coordinates.second;
			temporary_solution.push_back(temporary_coordinates);
			converter.clear(); //reset
			converter.str("");
		}
		
		solutions.push_back(temporary_solution);
		
		getline(fin,tmpstr); // read the line separating solutions.
		
		
	}
	
	
	
	return solutions;
}

std::string datagatherer::ParseSolutionsFile(std::ifstream & fin){ //std::vector< std::vector < std::pair< std::string, std::string > > >
	
	std::stringstream converter;
	std::stringstream solutions_file;
	std::string tmpstr;
	int number_of_solutions;
	if (!getline(fin,tmpstr)){
		return "";
	}
	
	
	//  first we read in the number of solutions.
	
	
	converter << tmpstr;  //get the line number
	converter >> number_of_solutions;
	converter.clear(); //reset
	converter.str("");
	solutions_file << tmpstr << "\n";
	getline(fin,tmpstr); //burn one line
	solutions_file << tmpstr << "\n";
	
	
	for (int solution_counter = 0; solution_counter <  number_of_solutions; ++solution_counter) {
		
		for (int variable_counter = 0; variable_counter < this->numvariables; ++variable_counter) {
			std::pair < std::string, std::string > temporary_coordinates;
			
			getline(fin,tmpstr);
			solutions_file << tmpstr << "\n";
			
		}
		
		
		getline(fin,tmpstr); // read the line separating solutions.
		solutions_file << tmpstr << "\n";
		
	}
	
	
	
	return solutions_file.str();
}




///the parser for the failed_paths file type, which is output from bertini.
std::string datagatherer::ParseFailedPaths(std::ifstream & fin){
	int tempfailnum = 0;
	std::string tmpstr;
	
	std::stringstream faily_myfailfail;
	
	while (1) {
		
		getline(fin,tmpstr);
		faily_myfailfail << tmpstr << "\n";
		if (tmpstr.length()==0) { // if do not have a solution
			break;
		}
		else { //have a soln
			for (int i=0; i<3+this->numvariables; ++i) {
				getline(fin,tmpstr);
				faily_myfailfail << tmpstr << "\n";
			}
			tempfailnum++;
		}
	}
	return faily_myfailfail.str();
}




//used to control the output folder name for data gathering
void datagatherer::IncrementOutputFolder(std::string & output_folder_name, std::string base_output_folder_name, int & output_folder_index){
	std::stringstream converter;
	
	output_folder_index = (output_folder_index+1);  //will start with 1 (since index starts at 0, and increment at start...);
	output_folder_name = base_output_folder_name;
	output_folder_name.append("/tmp");
	converter << output_folder_index;
	output_folder_name.append(converter.str());
	
	
	
	return;
}




std::vector< std::string > datagatherer::GetFoldersForData(std::string dir){
	
	std::vector< std::string > foldervector;
	
	
	//read in folder file.
	std::string foldername = dir;
	foldername.append("/folders");
	std::ifstream folderstream;
	folderstream.open(foldername.c_str());
	
	if (!folderstream.is_open()) {
		std::cerr << "failed to open file to read folders " << foldername << std::endl;
	}
	else{
		std::string tmpstr;
		
		
		while (getline(folderstream,tmpstr)) {
			foldervector.push_back(tmpstr);
		}
		folderstream.close();
	}
	
	return foldervector;
}



