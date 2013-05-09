#include "menu_cases.hpp"



///////////////////////////////////
//
//
//      SERIAL CASE
//
//
/////////////////////////////////

void serial_case(ProgSettings paramotopy_settings, runinfo paramotopy_info_original){
  
	std::cout << "the serial mode has been removed. \nif serial mode is something you really really need,\nplease contact the authors for support.\nwww.paramotopy.com\n";
	
  return;
}//re: serial_case()









/////////////////////////////////
//
//   PARALLEL STEP 2 CASE
//
//   STANDARD
//
/////////////////////////////////

void parallel_case(ProgSettings paramotopy_settings, runinfo paramotopy_info){
  

	
  std::stringstream mpicommand;
	
  mpicommand << paramotopy_settings.settings["parallelism"]["architecture"].value() << " -n ";
  mpicommand << paramotopy_settings.settings["parallelism"]["numprocs"].value() << " ";
  mpicommand << paramotopy_settings.settings["system"]["step2location"].value() << "/step2 ";
  mpicommand << paramotopy_info.inputfilename << " ";
  mpicommand << paramotopy_info.location << " ";
	mpicommand << paramotopy_info.steptwomode << " ";

  if (paramotopy_settings.settings["system"]["stifle"].intvalue==1){
    mpicommand << " > /dev/null ";
  }

  
  std::cout << "\n\n\n\n\n\n\n\n" << mpicommand.str() << "\n\n\n\n\n\n\n\n\n";
  
  system(mpicommand.str().c_str());  //make the system call to bertini
  
}//re: parallel_case()






////////////////////////////////////////
//
//
//          MAIN STEP 2, CALLED FROM PARAMOTOPY
//
//
////////////////////////////////////////


void steptwo_case(ProgSettings paramotopy_settings,
									runinfo paramotopy_info){
	

	
	std::string settings_filename = paramotopy_info.location;
	settings_filename.append("/prefs.xml");
	paramotopy_settings.save(settings_filename);
  
  //check if folder exists already.  if it does, prompt user
  boost::filesystem::path step2path(paramotopy_info.location);
  step2path /= ("step2");  //concatenation in boost
  
  if (boost::filesystem::exists(step2path)){  // if it can see the current run's step2 folder
    if (get_int_choice("found previous step2 folder.  remove, or bail out?\n0) bail out\n1) remove and continue\n: ",0,1)==1){
      boost::filesystem::remove_all( step2path );
      
      step2path += "finished";   // remove the step2finished file if it exists.
      if (boost::filesystem::exists(step2path)) {
				boost::filesystem::remove( step2path );
      }
    }
    else{
      std::cout << "returning to paramotopy main menu without running step2" << std::endl;
      return;
    }
  }
  
  

  
  
	
  //write a file containing the random values.
  std::string randpointfilename;
  randpointfilename = paramotopy_info.location;
  randpointfilename.append("/randstart");
  std::ofstream fout;
  fout.open(randpointfilename.c_str());
  fout.precision(16);
  for (int ii = 0; ii < paramotopy_info.numparam; ++ii){
    fout << paramotopy_info.RandomValues[ii].first << " "
		<< paramotopy_info.RandomValues[ii].second << "\n";
    
  }
  fout.close();
  
  
  
	
  //actually run the case
	
	if (paramotopy_settings.settings["parallelism"]["parallel"].intvalue == 0) {		
		serial_case(paramotopy_settings, paramotopy_info);
	}// end not parallel
	
	else{//parallel case...
		parallel_case(paramotopy_settings, paramotopy_info);
	} // end parallel case
	
	
  
}//re: step2case()





