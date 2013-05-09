#include "step2.hpp"




int main(int argc, char* argv[]){

	timer process_timer; // initializes the t_start value in the timer, and a map of zeros for the other timers.
		//do this straight away for total time.
	
	srand(time(NULL));
	

	int steptwomode;
	int myid;
	int numprocs;
	int namelen;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int headnode=0;
	
	std::ifstream fin;
	
	std::string sharedmemorylocation;
	
	
	std::string proc_name_str;
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD,&myid);
	MPI_Get_processor_name(processor_name,&namelen);
	proc_name_str = processor_name;
	
	
	
	std::string called_dir = stackoverflow_getcwd();
	std::string homedir = getenv("HOME");
	

	std::string filename, location;
	

	
	std::vector<std::vector<std::pair<double,double> > > AllParams;
	std::vector<std::string> ParamNames;
	std::stringstream commandss;  //we put info on and off this stringstream, to get from strings to numbers.
	
	
	
    ///////////////////
    //
    //    should be done getting info from commandstring (argv):
    //
    //    filename
		//		location
    //    steptwomode
    //
    //////////////////
	
	for (int ii = 0; ii < argc; ++ii){
		commandss << argv[ii] << " ";  //put the argv on the commandss, so that can convert appropriate types (integers, etc)
	}
	
	if ( (argc!=1) && (numprocs>1)){
	  std::string blank;
	  commandss >> blank;  // name of program, */step2
	  commandss >> filename;  // name of input file to paramotopy
	  commandss >> location;
		commandss >> steptwomode;
		commandss.clear(); commandss.str("");

	}
	else{
	  // didn't provide filename or any arguments ...
	  std::cerr << "Nothing passed as an argument ... \n"
		    << "this should never pop up as this program is only "
		    << " called from paramotopy...\n";
	  MPI_Finalize();
	  return 1;
	}
	
	
	
	

	

	
	runinfo paramotopy_info;  //holds all the info for the run
	
#ifdef timingstep2
	process_timer.press_start("read");
#endif
	paramotopy_info.GetInputFileName(filename); //dear god please don't fail to find the file.
	paramotopy_info.ParseData(location);
#ifdef timingstep2
	process_timer.add_time("read");
#endif
	paramotopy_info.location = location; // essentially from the command line
	paramotopy_info.steptwomode = steptwomode;

	
	
	
	
	//instantiate the settings
	std::string settingsfilename = location;
	settingsfilename.append("/prefs.xml");
	if (!boost::filesystem::exists(settingsfilename)) {
	  std::cerr << "for some reason the prefs file " << settingsfilename << " does not exist! id:" << myid << std::endl;
	  exit(-219);
	}
	ProgSettings paramotopy_settings(settingsfilename);
#ifdef timingstep2
	process_timer.press_start("read");
#endif
	paramotopy_settings.load();  // parse the settings xml file
#ifdef timingstep2
	process_timer.add_time("read");
#endif
	
	
	
	//get random values from file in base_dir.  probably destroyed during parsing process.  gotta do this after parsing.
#ifdef timingstep2
	process_timer.press_start("read");
#endif
	paramotopy_info.GetRandomValues();
#ifdef timingstep2
	process_timer.add_time("read");
#endif
	
	
	
	
	
	
	
	
	
	
	
	
	
	std::string base_dir=make_base_dir_name(filename);  //
	
	


	
	std::string timingfolder = location;
	timingfolder.append("/timing");
#ifdef timingstep2
	if (myid==0) {
		if (boost::filesystem::exists(timingfolder)){
			boost::filesystem::remove_all(timingfolder);
		}
		mkdirunix(timingfolder.c_str());
	}
#endif
	
	
	
	
	
	
	
	
	
	//the main body of the program is here:
	if (myid==headnode){
		master_process master;
		master.master_main(paramotopy_settings, paramotopy_info, process_timer);
	}
	else{
	  slave_process slave;
		slave.slave_main(paramotopy_settings, paramotopy_info, process_timer);
	}
	

	
	
	
#ifdef timingstep2
	process_timer.write_timing_data(timingfolder,myid);
#endif
	

	

	MPI_Finalize();//wrap it up
	
	
	
	
	return 0;
	
}//re: main














