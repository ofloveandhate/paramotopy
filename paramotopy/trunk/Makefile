

TINYXML_USE_STL := YES



OPT= -Wall -fopenmp -Dtimingstep2=1 -Dverbosestep2=1 #-funroll-loops -fexpensive-optimizations -O2 -g  

#Define the file suffixes for C++ source files
.SUFFIXES:
.SUFFIXES: .cpp $(SUFFIXES)



# Define the object files to be used

OBJScommon = mtrand.o random.o step2_funcs.o step1_funcs.o para_aux_funcs.o paramotopy_enum.h tinyxml.o tinystr.o tinyxmlparser.o tinyxmlerror.o xml_preferences.o
OBJSparamain = failed_paths.o menu_cases.o
OBJSsteptwo = step2readandwrite.o master.o slave.o
           
# Define Libary Locations

LIBDIR =#-L/usr/lib64/ #-L/home/matt/HD2/mpich2-1.3.1/lib/
LIBS = #lmpich
INCLUDE = # -/home/matt/HD2/mpich2-1.3.1/src/include/   

# Define the command to be used for compilation
MCC =	mpic++	 #mpic++
CC =	g++	


ifeq (YES, ${TINYXML_USE_STL})
  DEFS := -DTIXML_USE_STL
else
  DEFS :=
endif



# Describe how to build the executable file
all:	paramotopy step2

paramotopy:	 paramotopy.o $(OBJScommon) $(OBJSparamain)
	$(MCC) $(INCLUDE) $(DEFS) $(OPT) -o paramotopy paramotopy.cpp $(OBJScommon) $(OBJSparamain)
step2:	step2.o $(OBJScommon) $(OBJSsteptwo)
	$(MCC) $(INCLUDE) $(DEFS) $(OPT) -o step2 step2.cpp $(OBJScommon) $(OBJSsteptwo) -L./ -lbertini -lm -lmpfr -lgmp -static #-static
	
	
xml_preferences.o: tinyxml.o
tinyxml.o: tinyxml.h tinystr.h
tinyxmlparser.o: tinyxml.h tinystr.h

tinyxmlerror.o: tinyxml.h tinystr.h

# Describe how to create the object file
.cpp.o: 
	$(MCC) $(INCLUDE) $(DEFS) $(OPT) -c $*.cpp
.c.o:
	$(MCC) $(INCLUDE) $(DEFS) $(OPT) -c $*.c

.cpp:   
	$(MCC) $(INCLUDE) $(DEFS) $(OPT) -c $*.cpp $(LIBDIR) $(LIBS)
.c:
	$(MCC) $(INCLUDE) $(DEFS) $(OPT) -c $*.c


# Describe actions necessary to clean the current directory
clean:
	rm -f *.o paramotopy mystep2 *~
