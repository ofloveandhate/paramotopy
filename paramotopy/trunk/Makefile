#this makefile prepared for the cray at csu



OPT  := #-O3  #-Wall  -g -funroll-loops -fexpensive-optimizations # -g  
DEFS := -Dtimingstep2 -Dverbosestep2



#Define the file suffixes for C++ source files
.SUFFIXES: .cpp $(SUFFIXES)



# Define the object files to be used

OBJScommon :=  failed_paths.o menu_cases.o master.o slave.o step2readandwrite.o runinfo.o mtrand.o random.o step2_funcs.o step1_funcs.o para_aux_funcs.o tinyxmlerror.o tinyxml.o tinystr.o tinyxmlparser.o  xml_preferences.o

           
# Define Libary Locations

BOOSTLOC := /home/GRAD511/dbrake/lustrefs/realwork/boost_1_50_0/stage/lib/lib
#BOOSTLOC := /apps/boost_1_50_0/lib/lib
E := .a
#lmpich
#-L/usr/lib64/ #-L/home/matt/HD2/mpich2-1.3.1/lib/


LIBDIR :=       
LIBS := ./libbertini.a  -lm -L/opt/gcc/mpfr/2.4.2/lib  -lmpfr  -lgmp   
LIBSboost :=   $(BOOSTLOC)boost_system$E  $(BOOSTLOC)boost_filesystem$E $(BOOSTLOC)boost_regex$E $(BOOSTLOC)boost_thread$E $(BOOSTLOC)boost_chrono$E
INCLUDE := -I/home/GRAD511/dbrake/lustrefs/realwork/boost_1_50_0     #I/apps/boost_1_50_0/include #/home/GRAD511/dbrake/lustrefs/libraries/boost/include 

# Define the command to be used for compilation
MCC :=	CC
#mpic++
CC  :=	CC
#g++	







MANDATORYOPT := #-fopenmp
OPT := $(OPT) $(MANDATORYOPT)
DEFS := $(DEFS) -DTIXML_USE_STL




# Describe how to build the executable file
all:	paramotopy step2

paramotopy:	 paramotopy.o $(OBJScommon) 
	$(MCC) $(INCLUDE) $(DEFS) $(OPT) -o paramotopy paramotopy.cpp $(OBJScommon) $(LIBSboost) $(LIBS) -dynamic 
	
step2:	step2.o $(OBJScommon) 
	$(MCC) $(INCLUDE) $(DEFS) $(OPT) -o step2 step2.cpp $(OBJScommon) $(LIBSboost)  $(LIBS)  
	
	

	


	
	
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
	
	
	
xml_preferences.o: tinyxml.o tinystr.o
tinyxml.o: tinyxml.h tinystr.h 
tinyxmlparser.o: tinyxml.h tinystr.h
tinyxmlerror.o: tinyxml.h tinystr.h
