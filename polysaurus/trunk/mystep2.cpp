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
#include "step2readandwrite.h"
#define timingstep2


/*
 the computeNumdenom function is in the bertini library.  it takes in a character array, and pointers which return the numerator
 and denominator of the number.  I pass the input by casting via (char *) blabla
 */
extern "C" {
	void computeNumDenom(char **numer, char **denom, char *s);
}


extern "C" {
	int bertini_main(int argC, char *args[]);
}



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

  std::ifstream fin;
	

	
  std::string proc_name_str;
  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD,&myid);
  MPI_Get_processor_name(processor_name,&namelen);
  proc_name_str = processor_name;
	

	
  
//  bool firstrun = true;
  ToSave *TheFiles;
  std::string filename;
  int numsubfolders;
  int numfiles;
  int numparam;


  std::vector<std::vector<std::pair<double,double> > > AllParams;
  std::vector<std::string> ParamNames;
  std::stringstream commandss;  //we put info on and off this stringstream, to get from strings to numbers.

	
	
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
	
  for (int i = 0; i < argc; ++i){
    commandss << argv[i] << " ";
  }
  
  if (argc!=1){
        std::string blank;
        commandss >> blank;  // name of program, ./mystep2
		commandss >> filename;  // name of input file to polysaurus
		commandss >> numfiles; // number of files to save
	  std::string base_dir = "bfiles_";
	  base_dir.append(filename);

	  TheFiles = new ToSave[numfiles];
	  for (int i = 0; i < numfiles;++i){
		  commandss >> TheFiles[i].filename;
		  TheFiles[i].saved = true;
		  commandss >> TheFiles[i].filecount;
	  }  

    commandss >> numfilesatatime;//number of folders for each processor to own, and work out of.
    commandss >> numparam;//number of parameters in the input file.

    for (int i = 0; i < numparam;++i){
      std::string param;
      commandss >> param;
      ParamNames.push_back(param);
    }
	  
	  commandss >> saveprogresseverysomany;
	  commandss.clear();
	  commandss.str("");


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
		<< "numfilestosave: " << numfiles << "\n"
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
	std::string DataCollectedbase_dir = base_dir;
	DataCollectedbase_dir.append("/step2/DataCollected/");
	std::vector<std::string> tmpfolderlocs;
	int foldersdone=0;
	for (int i = 0; i < numsubfolders;++i){
		std::stringstream tmpfolder;
		tmpfolder << base_dir << "/step2/tmp/" << i ;
		tmpfolderlocs.push_back(tmpfolder.str());
	}
	
	
	
	
	if (myid==headnode) {
		SetUpFolders(base_dir,
					 numprocs,
					 numfilesatatime);
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
	
	
	

	
  //some data members used in this function
	int numsubfolders = dir.size();
	int numprocs, rank;
	int workerlastrecieved = 0;
	MPI_Status status;
	std::ifstream fin;
	std::ofstream fout;
	int numtodo;
	std::string base_dir = "bfiles_";
	base_dir.append(filename);	
	std::string finishedfile = base_dir;
	finishedfile.append("/finished");
	std::string lastoutfilename0 = base_dir;
	lastoutfilename0.append("/step2/lastnumsent0");	
	std::string lastoutfilename1 = base_dir;
	lastoutfilename1.append("/step2/lastnumsent1");	
	

	
#ifdef timingstep2
	double t_start, t_initial, t_receive=0, t_send=0, t_write=0, t1;//, t2, t3, t4, t_end; //for timing the whatnot
	int writecounter = 0, sendcounter = 0, receivecounter = 0;
	t_start = omp_get_wtime();
	std::ofstream timingout;
	std::string timingname = base_dir;
	timingname.append("/timing/mastertiming");
#endif 
	
	/* Find out how many processes there are in the default
     communicator */
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	

	

	
	
	
	

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
	std::vector< std::vector< std::pair<double,double> > > Values;
	std::vector< std::pair<double,double> > RandomValues; 
	bool userdefined;
	std::vector< int > NumMeshPoints;
	fin.open(filename.c_str());
	//reparse.  waaah.
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
	
#ifdef verbosestep2
	std::cout << "parsed\n";	
#endif
	
	std::vector< int > lastnumsent;
	GetLastNumSent(base_dir,
				   lastnumsent,
				   numprocs);
	
	//get random values.  probably destroyed during parsing process.  gotta do this after parsing.
	GetRandomValues(base_dir,
					RandomValues);
	
	
	
	
	//open mc file for writing out to
	std::string mcfname = base_dir;
	mcfname.append("/mc");
	std::ofstream mcfout;
	if (lastnumsent.size()>1) {
		mcfout.open(mcfname.c_str(), std::ios::out | std::ios::app);	
	}
	else {
		mcfout.open(mcfname.c_str());	
	}
	//end open mc file

	std::vector< std::string > startvector;
	std::vector< std::string > configvector;
	GetStartConfig(base_dir,
				   startvector,
				   configvector);
	

	
	
	
	
	
	std::vector< std::pair<double,double> > TmpValues;
	TmpValues.resize(numparam);
	std::vector< int > indexvector;//for forming a subscript from an index, for !userdefined case
	std::vector< int > KVector;//same
	indexvector.resize(numparam);
	double ParamSends[numsubfolders*(2*numparam+1)];//for sending to workers, to print into datacollected files
	int I, J, index;
	double a, b;//real and imaginary parts of the parameter values.
	std::stringstream ssdeleteme;
	int localcounter;//how many times through the loop
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
	
	
	


	

	for (int ii=1; ii<numprocs; ++ii) {
		std::stringstream myss;
		myss << base_dir << "/step2/tmp/init" << ii;
		std::string initrunfolder = base_dir;
		myss >> initrunfolder;
		myss.clear();
		myss.str("");
		mkdirunix(initrunfolder.c_str());
		std::string initruninput = initrunfolder;
		initruninput.append("/input");
		
		std::vector<std::pair<double, double> > tmprandomvalues = MakeRandomValues(numparam);
		

		//		std::string curbase_dir=dir[(proc_count-1)*numfilesatatime + localcounter];
		//		curbase_dir.append("/input");
		fout.open(initruninput.c_str());	  
		WriteStep2(configvector,
				   fout,
				   tmprandomvalues, //  <------- this, in this call, has been replaced from TmpValues to tmprandomvalues.
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
		std::string initrunstart = initrunfolder;
		initrunstart.append("/start");
		fout.open(initrunstart.c_str());
		for (int j=0; j<startvector.size(); ++j) {
			fout << startvector[j] << "\n";
		}
		fout.close();		
	}
	
	
	
	
	
	
#ifdef timingstep2
	t_initial = omp_get_wtime() - t_start;	
#endif	
	
	//to let workers know its ok to do the 2.1 solve
	int arbitraryint =0 ;
	 MPI_Bcast(&arbitraryint, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	
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
			
			
#ifdef timingstep2
			t1 = omp_get_wtime();
#endif			
			//write the mc line
			for (int j=0; j<numparam; ++j) {
				mcfout << TmpValues[j].first << " " << TmpValues[j].second << " ";

			}
			mcfout << "\n";
			
			//write the start files.  only need to do this once.
			std::string curbase_dir=dir[(proc_count-1)*numfilesatatime + localcounter];
			curbase_dir.append("/start");
			fout.open(curbase_dir.c_str());
			for (int j=0; j<startvector.size(); ++j) {
				fout << startvector[j] << "\n";
			}
			fout.close();
			
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
	
	//to let the workers know its ok to move on, and to let head node know workers are done with initial solve
	MPI_Bcast(&arbitraryint, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	
	
	
	double tempsends[numfilesatatime*(2*numparam+1)];
	
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
		
		
		
#ifdef verbosestep2
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
#ifdef timingstep2
		t_receive+= omp_get_wtime() - t1;
#endif

		
#ifdef verbosestep2
		std::cout << "received from : " << status.MPI_SOURCE << "\n";
#endif

  
		
	  
	    
		localcounter=0;
	    for (int i = (status.MPI_SOURCE-1)*numfilesatatime; i < (status.MPI_SOURCE-1)*numfilesatatime+numtodo; ++i){
			
			if (userdefined){
//				TmpValues = Values[countingup];
				
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
//					TmpValues[j].first = a;
//					TmpValues[j].second = b;
				
				}
				tempsends[localcounter*(2*numparam+1)+2*numparam] = countingup;//the line number in mc file
			}

			
#ifdef timingstep2
			t1 = omp_get_wtime();
#endif			//write the mc line
			for (int j=0; j<numparam; ++j) {
				mcfout << TmpValues[j].first << " " << TmpValues[j].second << " ";
			}
			mcfout << "\n";
#ifdef timingstep2
			t_write+= omp_get_wtime()-t1;
#endif
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
	double arbitraryfloat = -1;//this way the number of things sent matches the number expected to recieve.

	
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
		
#ifdef verbosestep2
		std::cout << "finally received from " << status.MPI_SOURCE << "\n";
		std::cout << "Sending kill tag to rank " << rank << "\n";
#endif
#ifdef timingstep2
		t1 = omp_get_wtime();
#endif
		MPI_Send(&arbitraryfloat, 1, MPI_FLOAT, rank, 0, MPI_COMM_WORLD);//added arbitraryfloat to make number send match number received.  db.
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
		   std::string base_dir,
		   int numfilesatatime){

	
	std::string up = "../../../..";
	std::string currentSeedstring;
	int blaint, currentSeed = 0;
	int linenumber;
	int numparam = ParamNames.size();
	int myid;
	MPI_Comm_rank(MPI_COMM_WORLD,&myid);
	MPI_Status status;
	
	double datareceived[numfilesatatime*(2*numparam+1)]; //for catching from the mpi
	std::stringstream myss;
	char *args_parse[3];
	args_parse[0] = "bertini";
	args_parse[1] = "input";
	args_parse[2] = "0";
	
	char *args_noparse[4];
	args_noparse[0] = "bertini";
	args_noparse[1] = "input";
	args_noparse[2] = "1";
	
	
	
	int firstrun=1; //starts at 1, gets turned to 0.
	std::vector<std::string> Numoutvector; 
	std::vector<std::string> arroutvector;
	std::vector<std::string> degoutvector;
	std::vector<std::string> namesoutvector;
	std::vector<std::string> configvector;
	std::vector<std::string> funcinputvector;


	
	
	
	myss << base_dir << "/step2/DataCollected/c"
		<< myid << "/";
	std::string DataCollectedbase_dir = myss.str();//specific to this worker, as based on myid
	std::vector<std::pair<double,double> >  AllParams;
	AllParams.resize(numparam);//preallocate
	myss.clear();
	myss.str("");

#ifdef timingstep2
	double t_start, t_receive=0, t_send=0, t_write=0, t1, t_bertini=0; //for timing the whatnot , t2, t3, t4, t_end, t_initial=0
	int writecounter = 0, sendcounter = 0, receivecounter = 0, bertinicounter = 0;
	t_start = omp_get_wtime();
	std::ofstream timingout;
	std::string timingname = base_dir;
	myss << myid;
	timingname.append("/timing/slavetiming");
	timingname.append(myss.str());
	myss.clear();
	myss.str("");
#endif 
	std::cout << base_dir;
	
	std::string initrunfolder;
	myss << base_dir <<  "/step2/tmp/init" << myid;
	myss >> initrunfolder;
	myss.clear();
	myss.str("");
	
	
	int arbitraryint =0 ;
	MPI_Bcast(&arbitraryint, 1, MPI_INT, 0, MPI_COMM_WORLD);// recieve a bcast from head telling its ok to proceed
	
	if (firstrun==1) {
		blaint = chdir(initrunfolder.c_str());
		
#ifdef timingstep2
		t1 = omp_get_wtime();
#endif
		currentSeed = bertini_main(3,args_parse);
#ifdef timingstep2
		t_bertini += omp_get_wtime() - t1;
		bertinicounter++;
#endif
		std::cout << "done with the first pass, worker " << myid << "\n";
		ReadDotOut(Numoutvector, 
				   arroutvector,
				   degoutvector,
				   namesoutvector,
				   configvector,
				   funcinputvector);
		blaint = chdir(up.c_str());

		std::cout << "done reading \n";
		myss << currentSeed;
		myss >> currentSeedstring;
		args_noparse[3] = const_cast<char *>(currentSeedstring.c_str());
		myss.clear();
		myss.str("");
		firstrun=0;

	}
	
	arbitraryint =0 ;
	MPI_Bcast(&arbitraryint, 1, MPI_INT, 0, MPI_COMM_WORLD); //agree w headnode that done w initial solve.
	
	
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
#ifdef verbosestep2
		std::cout << "received kill tag to rank " << myid << "\nkilling now\n";
#endif
		break; //exits the while loop
    }


    int localcounter = 0;
    for (int k = (myid-1)*numfilesatatime; k < (myid-1)*numfilesatatime+int(status.MPI_TAG); ++k){


		// unpack some data for passing around; perhaps should rewrite stuff to make this unnecessary
		for (int mm=0; mm<numparam; ++mm) {
			AllParams[mm].first = datareceived[localcounter*(2*numparam+1)+2*mm];
			AllParams[mm].second = datareceived[localcounter*(2*numparam+1)+2*mm+1];
		}
		linenumber = int(datareceived[localcounter*(2*numparam+1)+2*numparam]);
		
		
		// Call Bertini
		blaint = chdir(dir[k].c_str());
		if (loopcounter<numfilesatatime) {
			WriteDotOut(arroutvector,
						degoutvector,
						namesoutvector,
						configvector,
						funcinputvector);
		}
		WriteNumDotOut(Numoutvector,
					   AllParams,
					   numparam);
#ifdef timingstep2
		t1 = omp_get_wtime();
#endif
		blaint = bertini_main(4,args_noparse);
#ifdef timingstep2
		t_bertini += omp_get_wtime() - t1;
		bertinicounter++;
#endif
		blaint = chdir(up.c_str());

		

		
		// Collect the Data
		for (int j = 0; j < numfiles;++j){
			  
			  std::string target_file =
				MakeTargetFilename(DataCollectedbase_dir,
								   TheFiles,
								   j);
			  std::string orig_file = dir[k];
			
			
			  orig_file.append(TheFiles[j].filename);
			
#ifdef verbosestep2
			std::cout << target_file << " " << orig_file << "\n";
			std::cout << "allparams: " << AllParams[numparam-1].first << "\n";
			std::cout << "trying to write data to " << target_file << " from worker " << myid << " with folder" << k <<"\n";
#endif
			
			
			
			//write data to file
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
			
		}
    		
			
#ifdef verbosestep2
		std::cout << "Finished doing work on folder : " << k << " on rank " << myid << "\n";
#endif
		
		
		++localcounter;
		++loopcounter;
		if (loopcounter==512){//every 512 files, check file sizes
			UpdateFileCount(TheFiles,numfiles,DataCollectedbase_dir);
			loopcounter=0;
		}
		
		
    }//re: for (int k = 0; k < int(status.MPI_TAG);++k)

#ifdef verbosestep2
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






