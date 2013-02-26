#this file prepared for use with multiple computers.


#i understand that including personal machine specifics is a bit selfish, but it's only until we start using a tool like configure
#if you add a machine to this file, please add an option to this list.
#machine can be:     sam - MACBRAKE - cray - macniemerg
#MACHINENAME is an environment var set by the user.


#User-set options.  cannot use some options on the cray.  
OPT  := -Wall  -g  #-O3 -funroll-loops -fexpensive-optimizations # -g 

#use timing info for step 2?  YES, or NO (or anything other than YES really)
TIMING := YES

#print extra stuff to screen in step 2?  YES or NO (or anything other than YES really)
VERBOSE := YES

#turn off bertini and data collection calls?
NOSOLVE := NO

##################################
# Define machine specific stuff
##################################
STATICFLAG:= 
INSTALLDIR=UNSETLOCATION

ifeq (sam,$(MACHINENAME))
	#sam (hopefully user independent)
	MCC := mpic++ 
	CC  := g++
	MANDATORYOPT := -fopenmp
	BOOSTLOC := /usr/local/boost/lib/
	INCLUDE := -I../include -I/usr/local/boost/include 
	BOOSTSUFFIX :=#no spaces
	BERTINILIB := ./libbertinisam.a
	MPFRLOC :=#already in path
	STATICFLAG := -static
	
else ifeq (MACBRAKE, $(MACHINENAME))
	#dan brake's macintosh 
	MCC := mpic++ -cxx=g++
	CC  := g++
	MANDATORYOPT := -fopenmp
	BOOSTLOC := /usr/local/Cellar/boost/1.52.0/lib/
	INCLUDE := -I../include -I/usr/local/Cellar/boost/1.52.0/include  -I/usr/local/gfortran/lib/gcc/x86_64-apple-darwin10/4.6.2/include/
	BOOSTSUFFIX :=-mt#no spaces
	BERTINILIB := $(HOME)/lib/libbertini.a
	MPFRLOC :=#in path
	INSTALLDIR:=~/bin
	
else ifeq (cray,$(MACHINENAME))
	#cray, as dan brake
	MCC := CC
	CC := CC
	MANDATORYOPT := #do not use the -fopenmp flag on the cray with pgi compilers
	BOOSTLOC := /home/GRAD511/dbrake/lustrefs/realwork/boost_1_50_0/stage/lib/
	INCLUDE := -I../include -I/home/GRAD511/dbrake/lustrefs/realwork/boost_1_50_0
	BOOSTSUFFIX :=#no spaces
	BERTINILIB := ./libbertinicray.a
	MPFRLOC := -L/opt/gcc/mpfr/2.4.2/lib
	STATICFLAG := -static
	
else ifeq (macniemerg,$(MACHINENAME))
	#matt n's macintosh
	MCC:= mpic++ -mpe=mpicheck
	CC:= g++
	BOOSTLOC := /usr/local/lib/
	INCLUDE := -I../include -I/usr/local/include
	MANDATORYOPT := -fopenmp
	BOOSTSUFFIX :=#nothing
	BERTINILIB := ./libbertinimacmn.a
	MPFRLOC:=#already in path
	STATICFLAG :=#nothing
	INSTALLDIR=UNSETLOCATION
	
else
  #i expect errors here, or down below, because some variables are not set.  this is deliberate.  
  
  #todo:  use gnu autoconf to prepare the system for building

endif





#################################
#
#     end user-set stuff (hopefully)
#
#################################


#Define the file suffixes for C++ source files
.SUFFIXES: .cpp $(SUFFIXES)



# Define the object files to be used
OBJScommon :=  point.o datagatherer.o failed_paths.o menu_cases.o master.o slave.o step2readandwrite.o runinfo.o mtrand.o random.o step2_funcs.o step1_funcs.o para_aux_funcs.o timing.o tinyxmlerror.o tinyxml.o tinystr.o tinyxmlparser.o  xml_preferences.o 

           
# Define Libary Locations
LIBS := $(BERTINILIB) -lm $(MPFRLOC) -lmpfr -lgmp   


DEFS := #initialize DEFS



ifeq (YES,${TIMING})
	DEFS := $(DEFS) -Dtimingstep2
endif

ifeq (YES,${VERBOSE})
	DEFS := $(DEFS) -Dverbosestep2
endif
 
ifeq (YES,${NOSOLVE})
	DEFS := $(DEFS) -Dnosolve
endif







OPT := $(OPT) $(MANDATORYOPT)
DEFS := $(DEFS) -DTIXML_USE_STL
LIBSboost := $(BOOSTLOC)libboost_system$(BOOSTSUFFIX).a $(BOOSTLOC)libboost_filesystem$(BOOSTSUFFIX).a   $(BOOSTLOC)libboost_regex$(BOOSTSUFFIX).a  


# Describe how to build the executable file
all:	paramotopy step2

paramotopy:	 paramotopy.o $(OBJScommon) 
	$(MCC) $(INCLUDE) $(DEFS) $(OPT) -o paramotopy paramotopy.cpp $(OBJScommon) $(LIBS) $(LIBSboost) $(STATICFLAG)

step2:	step2.o $(OBJScommon) 
	$(MCC) $(INCLUDE) $(DEFS) $(OPT) -o step2 step2.cpp $(OBJScommon)  $(LIBS) $(LIBSboost) $(STATICFLAG)

install:	step2 paramotopy
	cp step2 paramotopy $(INSTALLDIR)



xml_preferences.o: tinyxml.o tinystr.o



# Describe how to create the object file
%.o:  %.cpp
	$(MCC) $(INCLUDE) $(DEFS) $(OPT) -c $< -o $@


#.cpp:   
#	$(MCC) $(INCLUDE) $(DEFS) $(OPT) -c $*.cpp 
#.c:
#	$(MCC) $(INCLUDE) $(DEFS) $(OPT) -c $*.c


# Describe actions necessary to clean the current directory
clean:
	rm -f *.o paramotopy step2 *~
	rm -rf paramotopy.dSYM step2.dSYM



