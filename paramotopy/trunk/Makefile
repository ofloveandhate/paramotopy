
OPT=-O3 -funroll-loops -fexpensive-optimizations -Wall -fopenmp -Dtimingstep2 -Dverbosestep2##Define the file suffixes for C++ source files
.SUFFIXES:
.SUFFIXES: .cpp $(SUFFIXES)

# Define the command to be used for compilation
MCC = mpic++ $(INCLUDE)
CC = g++ $(INCLUDE)

# Define the object files to be used
OBJS = # none
# Define Libary Locations

LIBDIR =#-L/usr/lib64/ #-L/home/matt/HD2/mpich2-1.3.1/lib/
LIBS = #lmpich
INCLUDE = # -/home/matt/HD2/mpich2-1.3.1/src/include/   


# Describe how to build the executable file
all: paramotopy mystep2

paramotopy: paramotopy.o mtrand.o random.o step2.o step1.o para_aux_funcs.o
	$(MCC) $(OPT) -o paramotopy paramotopy.o mtrand.o random.o step2.o step1.o para_aux_funcs.o
mystep2: mystep2.o mtrand.o random.o step2.o step1.o  step2readandwrite.o master.o slave.o
	$(MCC) -L./ $(OPT) -o mystep2 mystep2.cpp mtrand.o random.o step2.o step1.o  step2readandwrite.o  master.o slave.o -lbertini -lm -lmpfr -lgmp -static 
#mystep2shared:
#	$(MCC) -L$(HOME)/lib $(OPT) -o mystep2 mystep2.cpp mtrand.o random.o step2.o step1.o -Bdynamic -lbertini -Bstatic -lm  -Bstatic -lmpfr  -Bstatic -lgmp # -static 

# Describe how to create the object file
.cpp.o: 
	$(MCC) $(OPT) -c $< $(INCLUDE) $(LIBDIR) $(LIBS) 
.c.o:
	$(MCC) $(OPT) -c $<

.cpp:   
	$(MCC) $(OPT) -c $< $(INCLUDE) $(LIBDIR) $(LIBS)
.c:
	$(MCC) $(OPT) -c $<


# Describe actions necessary to clean the current directory
clean:
	rm -f *.o paramotopy mystep2 *~
