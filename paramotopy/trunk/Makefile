
OPT= -Wall -fopenmp -Dtimingstep2=1 -Dverbosestep2=1 #-funroll-loops -fexpensive-optimizations -O2 -g  

#Define the file suffixes for C++ source files
.SUFFIXES:
.SUFFIXES: .cpp $(SUFFIXES)


# Define the object files to be used

OBJScommon = mtrand.o random.o step2.o step1.o para_aux_funcs.o paramotopy_enum.h
OBJSparamain = failed_paths.o preferences.o menu_cases.o
OBJSsteptwo = step2readandwrite.o master.o slave.o
           
# Define Libary Locations

LIBDIR =#-L/usr/lib64/ #-L/home/matt/HD2/mpich2-1.3.1/lib/
LIBS = #lmpich
INCLUDE = # -/home/matt/HD2/mpich2-1.3.1/src/include/   

# Define the command to be used for compilation
MCC =	mpic++	 #mpic++
CC =	g++	

# Describe how to build the executable file
all:	paramotopy mystep2

paramotopy:	 paramotopy.o $(OBJScommon) $(OBJSparamain)
	$(MCC) $(INCLUDE) $(OPT) -o paramotopy paramotopy.cpp $(OBJScommon) $(OBJSparamain)
mystep2:	mystep2.o $(OBJScommon) $(OBJSsteptwo)
	$(MCC) $(INCLUDE) $(OPT) -o mystep2 mystep2.cpp $(OBJScommon) $(OBJSsteptwo) -L./ -lbertini -lm -lmpfr -lgmp -static #-static
	
	
#mystep2shared:
#	$(MCC) -L$(HOME)/lib $(OPT) -o mystep2 mystep2.cpp mtrand.o random.o step2.o step1.o -Bdynamic -lbertini -Bstatic -lm  -Bstatic -lmpfr  -Bstatic -lgmp #-static 

# Describe how to create the object file
.cpp.o: 
	$(MCC) $(INCLUDE) $(OPT) -c $*.cpp
.c.o:
	$(MCC) $(INCLUDE) $(OPT) -c $*.c

.cpp:   
	$(MCC) $(INCLUDE) $(OPT) -c $*.cpp $(LIBDIR) $(LIBS)
.c:
	$(MCC) $(INCLUDE) $(OPT) -c $*.c


# Describe actions necessary to clean the current directory
clean:
	rm -f *.o paramotopy mystep2 *~
