#this file prepared for use with multiple computers.


#i understand that including personal machine specifics is a bit selfish, but it's only until we start using a tool like configure
#if you add a machine to this file, please add an option to this list.
#machine can be:     sam - macbrake - cray - macniemerg
MACHINE := sam


#User-set options.  cannot use some options on the cray.  
OPT  := -Wall  -g  -O3 -funroll-loops -fexpensive-optimizations # -g 

#use timing info for step 2?  YES, or NO (or anything other than YES really)
TIMING := YES

#print extra stuff to screen in step 2?  YES or NO (or anything other than YES really)
VERBOSE := YES


##################################
# Define machine specific stuff
##################################
STATICFLAG:= 
ifeq (sam,${MACHINE})
	#sam (hopefully user independent)
	MCC := mpic++ 
	CC  := g++
	MANDATORYOPT := -fopenmp
	BOOSTLOC := /usr/local/boost/lib/
	INCLUDE := -I/usr/local/boost/include 
	BOOSTSUFFIX :=#no spaces
	BERTINILIB := ./libbertinisam.a
	MPFRLOC :=#already in path
	STATICFLAG := -static
endif

ifeq (macbrake, ${MACHINE})
	#dan brake's macintosh 
	MCC := mpic++ -cxx=g++
	CC  := g++
	MANDATORYOPT := -fopenmp
	BOOSTLOC := /usr/local/Cellar/boost/1.52.0/lib/
	INCLUDE := -I/usr/local/Cellar/boost/1.52.0/include  -I/usr/local/gfortran/lib/gcc/x86_64-apple-darwin10/4.6.2/include/
	BOOSTSUFFIX :=-mt#no spaces
	BERTINILIB := ./libbertinimacdb.a
	MPFRLOC :=#in path
endif

ifeq (cray,${MACHINE})
	#cray, dan brake
	MCC := CC
	CC := CC
	MANDATORYOPT := #do not use the -fopenmp flag on the cray with pgi compilers
	BOOSTLOC := /home/GRAD511/dbrake/lustrefs/realwork/boost_1_50_0/stage/lib/
	INCLUDE := -I/home/GRAD511/dbrake/lustrefs/realwork/boost_1_50_0
	BOOSTSUFFIX :=#no spaces
	BERTINILIB := ./libbertinicray.a
	MPFRLOC := -L/opt/gcc/mpfr/2.4.2/lib
	STATICFLAG := -static
endif

ifeq (macniemerg,${MACHINE})
	#matt n's macintosh
MCC:= mpic++ -mpe=mpicheck
CC:= g++
BOOSTLOC := /usr/local/lib/
INCLUDE := -I/usr/local/include
MANDATORYOPT := -fopenmp
BOOSTSUFFIX :=#nothing
BERTINILIB := ./libbertinimacmn.a
MPFRLOC:=#already in path
STATICFLAG :=#nothing

endif





#################################
#
#     end user-set stuff (hopefully)
#
#################################


#Define the file suffixes for C++ source files
.SUFFIXES: .cpp $(SUFFIXES)



# Define the object files to be used
OBJScommon :=  point.o datagatherer.o failed_paths.o menu_cases.o master.o slave.o step2readandwrite.o runinfo.o mtrand.o random.o step2_funcs.o step1_funcs.o para_aux_funcs.o tinyxmlerror.o tinyxml.o tinystr.o tinyxmlparser.o  xml_preferences.o 

           
# Define Libary Locations
LIBS := $(BERTINILIB) -lm $(MPFRLOC) -lmpfr -lgmp   


DEFS := #initialize DEFS



ifeq (YES,${TIMING})
	DEFS := $(DEFS) -Dtimingstep2
endif

ifeq (YES,${VERBOSE})
	DEFS := $(DEFS) -Dverbosestep2
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





xml_preferences.o: tinyxml.o tinystr.o
tinyxml.o: tinyxml.h tinystr.h 
tinyxmlparser.o: tinyxml.h tinystr.h
tinyxmlerror.o: tinyxml.h tinystr.h


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




