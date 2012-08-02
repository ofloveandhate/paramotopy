



OPT  := -Wall  -g # -O3 -funroll-loops -fexpensive-optimizations # -g  
DEFS := -Dtimingstep2 -Dverbosestep2



#Define the file suffixes for C++ source files
.SUFFIXES: .cpp $(SUFFIXES)



# Define the object files to be used

OBJScommon :=  bertini_funcs.h failed_paths.o menu_cases.o master.o slave.o step2readandwrite.o runinfo.o mtrand.o random.o step2_funcs.o step1_funcs.o para_aux_funcs.o paramotopy_enum.h tinyxmlerror.o tinyxml.o tinystr.o tinyxmlparser.o  xml_preferences.o

           
# Define Libary Locations

BOOSTLOC := /usr/local/boost/lib/

#lmpich
#-L/usr/lib64/ #-L/home/matt/HD2/mpich2-1.3.1/lib/


LIBDIR :=       
LIBS := ./libbertini.a -lm -lmpfr -lgmp   
LIBSboost := $(BOOSTLOC)libboost_system.a $(BOOSTLOC)libboost_filesystem.a   $(BOOSTLOC)libboost_regex.a  $(BOOSTLOC)libboost_filesystem.a
INCLUDE := -I/usr/local/boost/include 

# Define the command to be used for compilation
MCC :=	mpic++	 
CC  :=	g++	







MANDATORYOPT := -fopenmp
OPT := $(OPT) $(MANDATORYOPT)
DEFS := $(DEFS) -DTIXML_USE_STL




# Describe how to build the executable file
all:	paramotopy step2

paramotopy:	 paramotopy.o $(OBJScommon) 
	$(MCC) $(INCLUDE) $(DEFS) $(OPT) -o paramotopy paramotopy.cpp $(OBJScommon) $(LIBS) $(LIBSboost) -static
	
step2:	step2.o $(OBJScommon) 
	$(MCC) $(INCLUDE) $(DEFS) $(OPT) -o step2 step2.cpp $(OBJScommon)  $(LIBS) $(LIBSboost) -static
	
	

	


	
	
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
