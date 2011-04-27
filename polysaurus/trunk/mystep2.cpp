#include <mpi.h>
#include "step1.h"
#include "step2.h"
#include "mtrand.h"
#include "random.h"
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
#define verbosestep2
#define timingstep2


void master(std::vector<std::string> dir,
			std::string filename, 
			int numfilesatatime,
			int saveprogresseverysomany);


void slave(std::vector<std::string> dir, 
		   ToSave *TheFiles,
		   int numfiles,
		   std::vector<std::string> ParamNames, 
		   std::string filename,
		   int numfilesatatime);




int main(int argc, char* argv[]){

  int numfilesatatime;//added march7,11 db
	int saveprogresseverysomany;
	
  int myid;
  int numprocs;
  int namelen;
  char   processor_name[MPI_MAX_PROCESSOR_NAME];
  int headnode=0;
	
  std::vector< int > lastnumsent;

  std::ifstream fin;
	

	
  std::string proc_name_str;
  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD,&myid);
  MPI_Get_processor_name(processor_name,&namelen);
  proc_name_str = processor_name;
	

	
  
  bool firstrun = true;
  ToSave *TheFiles;
  std::string filename;
  int numsubfolders;//changed 3/7/11 db
  int numfiles;
  int numparam;


  std::vector<std::vector<std::pair<float,float> > > AllParams;
  std::vector<std::string> ParamNames;
  std::stringstream commandss;

  for (int i = 0; i < argc; ++i){
    commandss << argv[i] << " ";
  }
  
  if (argc!=1){
        std::string blank;
        commandss >> blank;  // name of program, ./mystep2
    commandss >> filename;  // name of input file to polysaurus
    commandss >> numfiles; // number of files to save
	  

    TheFiles = new ToSave[numfiles];
    for (int i = 0; i < numfiles;++i){
      commandss >> TheFiles[i].filename;
      TheFiles[i].saved = true;
      commandss >> TheFiles[i].filecount;
    }  

    commandss >> numfilesatatime;//used to be numsubfolders.  3/7/11 db
    commandss >> numparam;

    for (int i = 0; i < numparam;++i){
      std::string param;
      commandss >> param;
      ParamNames.push_back(param);
    }
	  
	  commandss >> saveprogresseverysomany;
	
	  
	  
	  commandss.clear();
	  commandss.str("");



    ///////////////////
    //
    //    should be done getting info from commandstring (argv), including some of:
    //
    //    ParamNames (vector)
    //    CValuesVect
    //    numsubfolders (computed from numfilesatatime)
    //    TheFiles with fields filecount (string), saved (bool), filename (string).
    //    numparam
    //    firstrun
    //    toss
    //    blank
    //    filename
    //    numfiles
    //    numfilesatatime
    //
    //////////////////

#ifdef verbosestep2
    if (myid==headnode){
      std::cout << "ParamNames: ";
      for (int i=0;i<ParamNames.size();++i){
		std::cout << ParamNames[i] << " "; 
      }

      std::cout << "\n"
		<< "numparam: " << numparam << "\n"
		<< "blank: " << blank << "\n"
		<< "filename: " << filename << "\n"
		<< "numfiles: " << numfiles << "\n"
		<< "numfilesatatime: " << numfilesatatime << "\n";
    }
#endif
	  
  }
  else{
    // didn't provide filename or any arguments ...
    std::cerr << "Nothing passed as an argument ... this"
	      << " should never pop up as this program is only "
	      << "ever called from polysaurus...\n";
	  MPI_Finalize();
    return 1;
  }
  

	
    numsubfolders = (numprocs-1)*numfilesatatime;
	std::string base_dir="bfiles_";
	base_dir.append(filename);
	base_dir.append("/");
	std::string DataCollectedbase_dir = base_dir;
	DataCollectedbase_dir.append("step2/DataCollected/");
	std::vector<std::string> tmpfolderlocs;
	int foldersdone=0;
	for (int i = 0; i < numsubfolders;++i){
		std::stringstream tmpfolder;
		tmpfolder << "bfiles_" << filename << "/step2/"
		<< "tmp/"
		<< i
		<< "/";
		tmpfolderlocs.push_back(tmpfolder.str());
	}

	
	
	
	
  if(myid==headnode){ // head node stuff
	  
 #ifdef verbosemystep2
     std::cout << "I'm the head node, " << myid << " of "
 	      << numprocs << " on "
 	      << processor_name << "\n";
 #endif
	  
	  
	  
	  std::string base_dir = "bfiles_";
	  base_dir.append(filename);
	  
	  
	  
	  


	  std::string filenamestep2=base_dir;
	  filenamestep2.append("/step2/tmp/");
	  
#ifdef timingstep2
	  std::string timingdir = base_dir;
	  timingdir.append("/timing");
	  mkdirunix(timingdir.c_str());
	  
#endif
	  
	  
	  //make the tmp folders.  
	  for (int i=0; i<numprocs*numfilesatatime; ++i) {
		  std::stringstream ss;
		  ss <<filenamestep2;      
		  ss << i;
		  mkdirunix(ss.str().c_str());//make the folder for the run.
		  ss.clear();
		  ss.str("");
	  }
	  
	  
	//make text file with names of folders with data in them
    std::string mydirfname = base_dir;
    mydirfname.append("/folders");
    std::ofstream fout(mydirfname.c_str());
    for (int i = 1; i < numprocs;++i){
      std::stringstream tmpfolder;
      tmpfolder << DataCollectedbase_dir;
      tmpfolder << "c";
      tmpfolder << i;
      fout << tmpfolder.str() << (i!=numprocs-1 ? "\n" :  "");      
    }
    fout.close();


	  
  }//end head node init stuff	
  else{ // worker node stuff   

	  
  }
  
	
	
	
	


	
	
	
	//the main body of the program is here:
  if (myid==headnode){
    master(tmpfolderlocs,filename, numfilesatatime, saveprogresseverysomany);
  }
  else{
    slave(tmpfolderlocs,
		  TheFiles, 
		  numfiles, 
		  ParamNames,
		  base_dir,
		  numfilesatatime);
  }
	
	
	
	

	
	
	



  delete[] TheFiles;
  MPI_Finalize();//wrap it up
	

}//re: main


























/////////////////////////////////////
//
//    master
//
////////////////////////////








void master(std::vector<std::string> dir,
			std::string filename,
			int numfilesatatime,
			int saveprogresseverysomany){//added mcfin,filename, etc 3/7/11 db
	
	
	

	
  //data members used in this function
	int numsubfolders = dir.size();
	int numprocs, rank;
	int workerlastrecieved = 0;
//	int modme = 10;//mod lastoutcounter this, and if 0 write to lastout file. every other do 0 or 1.
	MPI_Status status;
	std::ifstream fin;
	std::ofstream fout;
	int numtodo;
	std::string base_dir = "bfiles_";
	base_dir.append(filename);	
	std::string finishedfile = base_dir;
	finishedfile.append("/finished");
	
	

	
#ifdef timingstep2
	double t_start, t_end, t_initial, t_receive=0, t_send=0, t_write=0, t1, t2, t3, t4; //for timing the whatnot
	int writecounter = 0, sendcounter = 0, receivecounter = 0;
	t_start = omp_get_wtime();
	std::ofstream timingout;
	std::string timingname = base_dir;
	timingname.append("/timing/mastertiming");
#endif 
	
	/* Find out how many processes there are in the default
     communicator */
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	
	
	
	
	
	
	
	
	
	std::string blank;
	std::stringstream commandss;
	
	
	
	std::string lastoutfilename0 = base_dir;
	lastoutfilename0.append("/step2/lastnumsent0");	  
	// query whether had run or not yet. 
	fin.open(lastoutfilename0.c_str());
	std::vector< int > lastnumsent0;
	lastnumsent0.push_back(0);
	int tmpint;
	if (fin.is_open()){
		int lastoutcounter=1;
		for (int i=1; i<numprocs; ++i) {
			getline(fin,blank);
			if (blank=="") {
				break;
			}
			commandss << blank;
			commandss >> tmpint;
			lastnumsent0.push_back(tmpint);
			lastoutcounter++;
			commandss.clear();
			commandss.str("");
		}
		
	}
	fin.close();
	
	
	
	
	
	std::string lastoutfilename1 = base_dir;
	lastoutfilename1.append("/step2/lastnumsent1");	  
	// query whether had run or not yet. 
	fin.open(lastoutfilename1.c_str());
	std::vector< int > lastnumsent1;
	lastnumsent1.push_back(0);
	if (fin.is_open()){
		int lastoutcounter=1;
		for (int i=1; i<numprocs; ++i) {
			getline(fin,blank);
			if (blank=="") {
				break;
			}
			commandss << blank;
			commandss >> tmpint;
			lastnumsent1.push_back(tmpint);
			lastoutcounter++;
			commandss.clear();
			commandss.str("");
		}

	}
	fin.close();	
	

	int vectortosave = -1;
	if ( (lastnumsent1.size()<numprocs) && lastnumsent0.size()<numprocs ) {//failed overall...suck.
		//both vectors fail.  initialize fresh run.
	}
	else if (lastnumsent0.size()<numprocs){//failed during writing of 0?
		vectortosave = 1;
	}
	else if (lastnumsent1.size()<numprocs){//failed during writing of 1?
		vectortosave = 0;
	}
	else {//not possible for exactly one of the two following things to happen, given the previous 3 failed
		for (int i=1; i<numprocs; ++i) {
			if (lastnumsent0[i]<lastnumsent1[i]) {
				vectortosave = 1;
				break;
			}
			else if (lastnumsent1[i]<lastnumsent0[i]){
				vectortosave = 0;
				break;
			}
		}


	}

	
	//not possible for them to be the same.  vectortosave==(-1) if neither size matched.  fresh run if -1
	
	std::vector< int > lastnumsent;
	lastnumsent.push_back(0);
	if (vectortosave==0) {
		for (int i=1; i<numprocs; ++i) {
			lastnumsent.push_back(lastnumsent0[i]);
		}		
	}
	else if (vectortosave==1){
		for (int i=1; i<numprocs; ++i) {
			lastnumsent.push_back(lastnumsent1[i]);
		}
	}
	
	
#ifdef verbosestep2
	std::cout << vectortosave << " is the lastnumsent vector to save.\n";
#endif 
	
	
	

	//begin parse
	int numfunct;
	int numvar;
	int numparam;
	int numconsts;
	std::vector<std::string> FunctVector; 
	std::vector<std::string> VarGroupVector; 
	std::vector<std::string> ParamVector; 
	std::vector<std::string> ParamStrings;
	std::vector<std::string> Consts;
	std::vector<std::string> ConstantStrings;
	std::vector< std::vector< std::pair<float,float> > > Values;
	std::vector< std::pair<float,float> > RandomValues; 
	bool userdefined;
	std::vector< int > NumMeshPoints;
	fin.open(filename.c_str());
	//reparse.
	ParseData(numfunct,
			  numvar,
			  numparam,
			  numconsts,
			  FunctVector,
			  VarGroupVector,
			  ParamVector,
			  ParamStrings,
			  Consts,
			  ConstantStrings,
			  Values,//captures both user-defined and program-generated
			  RandomValues,//we will overwrite, because will be wrong.  load from a file in the bfiles folder
			  userdefined,
			  fin,//yep
			  NumMeshPoints,//a vector of integers, indicating the number of points for each parameter
			  filename);
	fin.close();
	//end parse
	
	std::cout << "parsed\n";	

	
	//get random values.  probably destroyed during parsing process
	std::string randfilename = base_dir;
	randfilename.append("/randstart");
	fin.open(randfilename.c_str());
	int ccount=0;
	std::string mytemp;
	std::cout << "\n\n";
	while(getline(fin,mytemp)){
		std::stringstream myss;
		myss << mytemp;
		float crandreal;
		float crandimaginary;
		myss >> crandreal;
		myss >> crandimaginary;
		RandomValues[ccount].first = crandreal;
		RandomValues[ccount].second = crandimaginary;
		std::cout << ParamStrings[ccount]
		<< " = "
		<< RandomValues[ccount].first 
		<< " + "
		<< RandomValues[ccount].second
		<< "*I\n";
		++ccount;
	}
	std::cout << "\n";
	fin.close();
	//end get random values
	
	
	//open mc file
	
	std::cout << base_dir << " " << lastnumsent.size() << "\n"; 
	std::string mcfname = base_dir;
	mcfname.append("/mc");
	std::ofstream mcfout;
		if (lastnumsent.size()>1) {
		mcfout.open(mcfname.c_str(),std::ios::out | std::ios::app);	
	}
	else {
		mcfout.open(mcfname.c_str());	
	}
	//end open mc file

#ifdef verbosestep2
	std::cout << "opened mc file\n";
#endif 
	
	//get start file in memory
	std::string copyme;
	std::vector< std::string > startvector;
	std::string startstring=base_dir;
	startstring.append("/step1/nonsingular_solutions");
	fin.open(startstring.c_str());
	while (!fin.eof()) {
		getline(fin,copyme);
		startvector.push_back(copyme);
	}
	fin.close();
	//end get start in memory
	
	
	
	//read in config2 file
	std::vector< std::string > configvector;
	fin.open("config2");
	while (!fin.eof()) {
		getline(fin,copyme);
		configvector.push_back(copyme);
	}
	fin.close();
	//end read config2
	
	
	

	
	
	/////////////////////////
	//
	//  end initial setup for master
	//
	//////////////////////
	
	
	
	
	

	
	
	
	
	
	
	
	
	
	
	
	
	
	
#ifdef verbosestep2
	std::cout << "starting initial stuff\n";
#endif
	
//////////////////////////////
	//
	//        initial writes and sends
	//
	//////////////////////////
	
	
	
	
	
	
	if (lastnumsent.size()==1) {
		for (int i=1; i<numprocs; ++i) {
			lastnumsent.push_back((i-1)*numfilesatatime);
		}
	}
	
	
	

	std::vector< int > indexvector;//for forming a subscript from an index, for !userdefined case
	std::vector< int > KVector;//same
	indexvector.resize(numparam);
	
	int terminationint;//how many solves to do total.
	if (!userdefined) {
		KVector.push_back(1);
		for (int i=1; i<numparam; ++i) {
			KVector.push_back(KVector[i-1]*NumMeshPoints[i-1]);
		}
		terminationint = KVector[numparam-1]*NumMeshPoints[numparam-1];
	}
	else {
		terminationint = Values.size();
	}

	
	
	float ParamSends[numsubfolders*(2*numparam+1)];//for sending to workers, to print into datacollected files

	int I, J, index;
	float a, b;//real and imaginary parts of the parameter values.
	std::stringstream ssdeleteme;
	std::vector< std::pair<float,float> > TmpValues;
	TmpValues.resize(numparam);
	int localcounter;//how many times through the loop
	
	
	
#ifdef timingstep2
	t_initial = omp_get_wtime() - t_start;	
#endif	
	
	
	///////////////
	//
	//    create the input files for the initial seed of workers
	//
	///////////////
	int biggestnumsent = 0;
	int loopcounter = 0;
	for (int proc_count=1;proc_count<numprocs;++proc_count){
		localcounter=0;
		for (int i = lastnumsent[proc_count]; (i < lastnumsent[proc_count]+numfilesatatime) && (i<terminationint  ); ++i){
			

			//determine the parameter values for this file
			if (userdefined){
				TmpValues = Values[i];
				for (int j=0; j<numparam; ++j) {
					ParamSends[loopcounter*(2*numparam+1)+2*j] = Values[i][j].first;//
					ParamSends[loopcounter*(2*numparam+1)+2*j+1] = Values[i][j].second;//
				}
				ParamSends[loopcounter*(2*numparam+1)+2*numparam] = i;//the line number in mc file
			}
			
			else {//not user defined
				//form index
				index = i;
				for (int j=numparam-1; j>-1; j--) {
					I = (index)%KVector[j];
					J = (index - I)/KVector[j];
					indexvector[j] = J;
					index = I;
				}
				//get param values
				for (int j=0; j<numparam; ++j) {
					a = Values[j][ indexvector[j] ].first;
					b = Values[j][ indexvector[j] ].second;
					TmpValues[j].first = a;
					TmpValues[j].second = b;
					ParamSends[loopcounter*(2*numparam+1) + 2*j] = a;//for sending to worker, to write to data file
					ParamSends[loopcounter*(2*numparam+1) + 2*j+1] = b;
				}
				ParamSends[loopcounter*(2*numparam+1) + 2*numparam] = i;//the line number in the mc file
			}
			
			
			
			//write the input file
#ifdef timingstep2
			t1 = omp_get_wtime();
#endif
			std::string curbase_dir=dir[(proc_count-1)*numfilesatatime + localcounter];
			curbase_dir.append("/input");
			fout.open(curbase_dir.c_str());	  
			WriteStep2(configvector,
			   fout,
			   TmpValues,
			   FunctVector, 
			   VarGroupVector,
			   ParamVector,
			   ParamStrings,
			   Consts,
			   ConstantStrings,
			   RandomValues,
			   numfunct,
			   numvar,
			   numparam,
			   numconsts);
			fout.close();
			
			
			//write the start files.  only need to do this once.
			curbase_dir=dir[(proc_count-1)*numfilesatatime + localcounter];
			curbase_dir.append("/start");
			fout.open(curbase_dir.c_str());
			for (int j=0; j<startvector.size(); ++j) {
				fout << startvector[j] << "\n";
			}
			fout.close();
			

			
			//write the mc line
			for (int j=0; j<numparam; ++j) {
				mcfout << TmpValues[j].first << " " << TmpValues[j].second << " ";

			}
			mcfout << "\n";
			
			
#ifdef timingstep2
			t_write+= omp_get_wtime() - t1;
#endif
			
			//increment
			localcounter++;
			loopcounter++;//for keeping track of folder to write to
			if (i>biggestnumsent){
				biggestnumsent=i;
			}
		}		//re: for int i=lastnumsent[proc_count]	  
		
	}//re: for int i=1:numprocs
	
		
	float tempsends[numfilesatatime*(2*numparam+1)];
	
  /* Seed the slaves; send one unit of work to each slave. */
	for (rank = 1; rank < numprocs; ++rank) {  //no rank 0, because 0 is head node
	  
		localcounter=0;
		for (int i=(rank-1)*numfilesatatime; i<rank*numfilesatatime; ++i) {
			for (int j=0; j<2*numparam+1; ++j) {
				
				tempsends[localcounter*(2*numparam+1)+j] = ParamSends[i*(2*numparam+1)+j];
			}	  
			localcounter++;
		}
		lastnumsent[rank] = int(tempsends[2*numparam]);
		
		
		
#ifdef verbosemystep2
		std::cout << "to process : " << rank << "with numfilesatatime " << numfilesatatime 
			  << "\n";
#endif
	  
	
#ifdef timingstep2
		t1 = omp_get_wtime();
#endif
		
		
		MPI_Send(&tempsends,      /* message buffer */
				 numfilesatatime*(2*numparam+1),                 /* one data item */
				 MPI_FLOAT,           /* data item is an integer */
				 rank,              /* destination process rank */
				 numfilesatatime,           /* user chosen message tag */
				 MPI_COMM_WORLD);   /* default communicator */
		
		
#ifdef timingstep2
		t_send += omp_get_wtime()-t1;
#endif
	}//re: initial sends
	
	


	
	
	
	
	
	
	
	
	
	
	//////////////////////////////
	//
	//        loop over rest of points.
	//
	//////////////////////////
	
	


  /* Loop over getting new work requests 
     until there is no more work
     to be done */
	int filereceived = 0;
	int lastoutcounter=1;//for writing the lastnumsent file.  every so many, we write the file.
	
	int countingup = biggestnumsent+1;
	
	
	
	while (countingup < terminationint) {

		numtodo = numfilesatatime;
		
		if (numtodo+countingup>=terminationint) {
		  numtodo = terminationint - countingup;
		}
		
		
		
		
#ifdef timingstep2
		t1 = omp_get_wtime();
#endif
		
		/* Receive results from a slave */
		MPI_Recv(&filereceived,       /* message buffer */
				 1,                 /* one data item */
				 MPI_INT,           /* of type int */
				 MPI_ANY_SOURCE,    /* receive from any sender */
				 MPI_ANY_TAG,       /* any type of message */
				 MPI_COMM_WORLD,    /* default communicator */
				 &status);          /* info about the received message */

#ifdef verbosemystep2
		std::cout << "received from : " << status.MPI_SOURCE << "\n";
#endif
#ifdef timingstep2
		t_receive+= omp_get_wtime() - t1;
#endif
  
		
	  
	    //write new files to run.
		localcounter=0;
	    for (int i = (status.MPI_SOURCE-1)*numfilesatatime; i < (status.MPI_SOURCE-1)*numfilesatatime+numtodo; ++i){
#ifdef verbosemystep2
			std::cout << "head node writing to folder: " << i << "\n";
#endif	
			
			if (userdefined){
				TmpValues = Values[countingup];
				
				for (int j=0; j<numparam; ++j) {
					tempsends[localcounter*(2*numparam+1)+2*j] = Values[countingup][j].first;//
					tempsends[localcounter*(2*numparam+1)+2*j+1] = Values[countingup][j].second;//

				}
				tempsends[localcounter*(2*numparam+1)+2*numparam] = countingup;//the line number in mc file

			}
			else {

				//get subscripts (zero-based) for the index, countingup
				index = countingup;
				for (int j=numparam-1; j>-1; --j) {
					I = (index)%KVector[j];
					J = (index - I)/KVector[j];
					indexvector[j] = J;
					index = I;
				}
				
				//assign the data
				for (int j=0; j<numparam; ++j) {
					

					a = Values[j][ indexvector[j] ].first;
					b = Values[j][ indexvector[j] ].second;
					tempsends[localcounter*(2*numparam+1)+2*j] = a;//for sending to workers, to write into data files.
					tempsends[localcounter*(2*numparam+1)+2*j+1] = b;
					TmpValues[j].first = a;
					TmpValues[j].second = b;
				
				}
				tempsends[localcounter*(2*numparam+1)+2*numparam] = countingup;//the line number in mc file
			}
			
			
#ifdef timingstep2
			t1 = omp_get_wtime();
#endif
			std::string curbase_dir=dir[i];
			curbase_dir.append("/input");
			fout.open(curbase_dir.c_str());	  
			WriteStep2(configvector,
					   fout,
					   TmpValues,
					   FunctVector, 
					   VarGroupVector,
					   ParamVector,
					   ParamStrings,
					   Consts,
					   ConstantStrings,
					   RandomValues,
					   numfunct,
					   numvar,
					   numparam,
					   numconsts);
			fout.close();
#ifdef timingstep2
			t_write+= omp_get_wtime()-t1;
#endif
			
			//write the mc line
			for (int j=0; j<numparam; ++j) {
				mcfout << TmpValues[j].first << " " << TmpValues[j].second << " ";
			}
			mcfout << "\n";
			
			//increment
			countingup++;
			localcounter++;
	    } //re: write files in the step2 loop
  

	  

	  
	  

#ifdef timingstep2
		t1 = omp_get_wtime();
#endif
		/* Send the slave a new work unit */
		MPI_Send(&tempsends,             /* message buffer */
			 numfilesatatime*(2*numparam+1),                 /* how many data items */
			 MPI_FLOAT,           /* data item is an integer */
			 status.MPI_SOURCE, /* to who we just received from */
			 numtodo,           /* user chosen message tag */
			 MPI_COMM_WORLD);   /* default communicator */
#ifdef timingstep2
		t_send+= omp_get_wtime()-t1;
#endif
		
		
		lastnumsent[status.MPI_SOURCE] = int(tempsends[(2*numparam)]);
		
		if ((lastoutcounter%saveprogresseverysomany)==0) {
			std::ofstream lastout;
			if (lastoutcounter%(2*saveprogresseverysomany)==0) {
				lastout.open(lastoutfilename1.c_str());
			}
			else {
				lastout.open(lastoutfilename0.c_str());
			}
#ifdef timingstep2
			t1 = omp_get_wtime();
#endif
		
			for (int i=1; i<numprocs; ++i) {
				lastout << lastnumsent[i] << "\n";
			}		
			lastout.close();
			
#ifdef timingstep2
			t_write += omp_get_wtime() - t1;
			
#endif
		}
		
		
		lastoutcounter++;//essentially counts this loop.  will take mod 10, for the time being, and if 0, will write lastout
	} //re: while loop
	
	////////////
	//
	//     end while
	//
	/////////////
	
	
	

	
	
	
	
  
  /* There's no more work to be done, so receive all the outstanding
     results from the slaves. */
	float arbitraryfloat = -1;//this way the number of things sent matches the number expected to recieve.

	
	for (rank = 1; rank < numprocs; ++rank) {
		
#ifdef timingstep2
		t1 = omp_get_wtime();
#endif
		
		MPI_Recv(&filereceived, 
				 1, 
				 MPI_INT, 
				 MPI_ANY_SOURCE,
				 MPI_ANY_TAG, 
				 MPI_COMM_WORLD, 
				 &status);
#ifdef timingstep2
		t_receive += omp_get_wtime() - t1;
#endif
		
#ifdef verbosemystep2
		std::cout << "finally received from " << status.MPI_SOURCE << "\n";
		std::cout << "Sending kill tag to rank " << rank << "\n";
#endif
#ifdef timingstep2
		t1 = omp_get_wtime();
#endif
		MPI_Send(&arbitraryfloat, 1, MPI_FLOAT, rank, 0, MPI_COMM_WORLD);//added arbitraryint to make number send match number received.  db.
#ifdef timingstep2
		t_send += omp_get_wtime()-t1;
#endif
		
	}

	
#ifdef timingstep2
	t1 = omp_get_wtime();
#endif
	
	mcfout.close();
	fout.open(finishedfile.c_str());
	fout << 1;
	fout.close();

#ifdef timingstep2
	t_write += omp_get_wtime() - t1;
#endif
	
	
#ifdef timingstep2
	timingout.open(timingname.c_str(),std::ios::out);
	timingout <<  "initialize: " << t_initial << "\n"
			<< "write time: " << t_write << "\n"
			<< "send time: " << t_send << "\n"
			<< "receive time: " << t_receive << "\n"
			<< "total time: " << omp_get_wtime() - t_start << "\n";
	timingout.close();
#endif
	
	
}//re:master





























//////////////////////////////////
//
//         slave function
//
/////////////////////////////











void slave(std::vector<std::string> dir, 
		   ToSave *TheFiles, 
		   int numfiles,
		   std::vector<std::string> ParamNames, 
		   std::string npath,
		   int numfilesatatime){

	
	
	int linenumber;
	int numparam = ParamNames.size();
	int myid;
	MPI_Comm_rank(MPI_COMM_WORLD,&myid);
	MPI_Status status;
	
	float datareceived[numfilesatatime*(2*numparam+1)];
	std::stringstream myss;
	
	
	
	
	std::ifstream fin;
	std::string path;
	std::string tmpname = "path";
	myss << myid;
	tmpname.append(myss.str());
	tmpname.append(".out");
	std::string syscall = "pwd > ";
	syscall.append(tmpname);
	system(syscall.c_str());
	fin.open(tmpname.c_str());
	fin >> path;
	std::cout << path << "\n";
	fin.close();
	myss.clear();
	myss.str("");
	
	
	myss << path << "/step2/DataCollected/c"
		<< myid << "/";
	std::string DataCollectedbase_dir = myss.str();//specific to this worker, as based on myid
	std::vector<std::pair<float,float> >  AllParams;
	AllParams.resize(numparam);//preallocate
	myss.clear();
	myss.str("");

#ifdef timingstep2
	double t_start, t_end, t_initial=0, t_receive=0, t_send=0, t_write=0, t1, t2, t3, t4, t_bertini=0; //for timing the whatnot
	int writecounter = 0, sendcounter = 0, receivecounter = 0, bertinicounter = 0;
	t_start = omp_get_wtime();
	std::ofstream timingout;
	std::string timingname = path;
	myss << myid;
	timingname.append("/timing/slavetiming");
	timingname.append(myss.str());
#endif 
	

	int loopcounter = 0;  //is for counting how many solves this worker has performed, for data collection file incrementing
	
  while (1) {

    /* Receive a message from the master */

#ifdef timingstep2
	  t1 = omp_get_wtime();
#endif
    MPI_Recv(&datareceived, numfilesatatime*(2*numparam+1), MPI_FLOAT, 0, MPI_ANY_TAG,
             MPI_COMM_WORLD, &status);
#ifdef timingstep2
	  t_receive+= omp_get_wtime() - t1;
	  receivecounter++;
#endif
	  
	  
    /* Check the tag of the received message. */
    if (status.MPI_TAG == 0) {
#ifdef verbosemystep2
		std::cout << "received kill tag to rank " << myid << "\nkilling now\n";
#endif
		break;
    }

	  



    int localcounter = 0;
    for (int k = (myid-1)*numfilesatatime; k < (myid-1)*numfilesatatime+int(status.MPI_TAG); ++k){


		/* Do the work */
		  
		
		// Call Bertini
#ifdef timingstep2
		t1 = omp_get_wtime();
#endif
		CallBertiniStep2(dir[k]);
#ifdef timingstep2
		t_bertini += omp_get_wtime() - t1;
		bertinicounter++;
#endif
		
		for (int mm=0; mm<numparam; ++mm) {
			AllParams[mm].first = datareceived[localcounter*(2*numparam+1)+2*mm];
			AllParams[mm].second = datareceived[localcounter*(2*numparam+1)+2*mm+1];
		}
		linenumber = int(datareceived[localcounter*(2*numparam+1)+2*numparam]);
		
		
		// Collect the Data
//		std::string base_dir = "bfiles_";
//		base_dir.append(filename);
//		base_dir.append("/");
		bool append;
		for (int j = 0; j < numfiles;++j){
			  
			  std::string target_file =
				MakeTargetFilename(DataCollectedbase_dir,
								   TheFiles,
								   j);
			  std::string orig_file = dir[k];
			
			
			  orig_file.append(TheFiles[j].filename);
			
#ifdef verbosemystep2
			std::cout << target_file << " " << orig_file << " " << TheFiles[j].filename << "\n";
			std::cout << "allparams: " << AllParams[numparam-1].first << "\n";
			std::cout << "trying to write data to " << target_file << " from worker " << myid << "with folder" << k <<"\n";

#endif
			
			
			
			
#ifdef timingstep2
			t1= omp_get_wtime();
#endif
			  WriteData(linenumber, 
					orig_file, 
					target_file,
					ParamNames,
					AllParams);	
#ifdef timingstep2
			t_write += omp_get_wtime() - t1;
			writecounter++;
#endif
			
    		
			
#ifdef verbosemystep2
			std::cout << "wrote data to " << target_file << " from worker " << myid << "\n";
#endif
		}
	
#ifdef verbosemystep2
		std::cout << "Finished doing work on folder : " << k << " on rank " << myid 
		  << "\n";
#endif
		
		
		++localcounter;
		++loopcounter;
		if (loopcounter==512){//every 512 files, check file sizes
			
			UpdateFileCount(TheFiles,numfiles,DataCollectedbase_dir);


			loopcounter=0;
			
		}
		
		
		
		
    }//re: for (int k = 0; k < int(status.MPI_TAG);++k)

#ifdef verbosemystep2
	  std::cout << "\n\n" << myid << " " << linenumber << " \n";
#endif
	  
	  
#ifdef timingstep2
	  t1 = omp_get_wtime();
#endif
    MPI_Send(&linenumber, 1, MPI_INT, 0, int(status.MPI_TAG), MPI_COMM_WORLD);
#ifdef timingstep2
	  t_send += omp_get_wtime() - t1;
	  sendcounter++;
#endif
  }//re: while (1)
	
  
	
#ifdef timingstep2
	timingout.open(timingname.c_str(),std::ios::out);
	timingout << myid << "\n"
	<< "bertini: " << t_bertini << " " << bertinicounter << "\n"
	<< "write: " << t_write << " " << writecounter << "\n"
	<< "send: " << t_send << " " << sendcounter << "\n"
	<< "receive: " << t_receive << " " << receivecounter << "\n"
	<< "total: " << omp_get_wtime() - t_start << "\n";
	timingout.close();
#endif	
	return;
}//re:slave




