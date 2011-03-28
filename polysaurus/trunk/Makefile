# Define the file suffixes for C++ source files
.SUFFIXES:
.SUFFIXES: .cpp $(SUFFIXES)

# Define the command to be used for compilation
MCC = mpic++ $(INCLUDE)
CC = g++ $(INCLUDE)

# Define the object files to be used
OBJS = # none
# Define Libary Locations

LIBDIR = #-L/home/matt/HD2/mpich2-1.3.1/lib/
LIBS = #lmpich
INCLUDE = # -/home/matt/HD2/mpich2-1.3.1/src/include/ 


# Describe how to build the executable file
#mpipolysaurus: polysaurus.o mtrand.o random.o step2.o step1.o 
#	$(MCC) -o mpipolysaurus polysaurus.o mtrand.o random.o step2.o step1.o
polysaurus: polysaurus.o mtrand.o random.o step2.o step1.o 
	$(CC) -o polysaurus polysaurus.o mtrand.o random.o step2.o step1.o
mystep2: mystep2.o mtrand.o random.o step2.o step1.o
	$(MCC) -o mystep2 mystep2.o mtrand.o random.o step2.o step1.o

# Describe how to create the object file
.cpp.o: 
	$(CC) -c $< $(INCLUDE) $(LIBDIR) $(LIBS) 
.c.o:
	$(CC) -c $<

.cpp:   
	$(CC) -c $< $(INCLUDE) $(LIBDIR) $(LIBS)
.c:
	$(CC) -c $<


# Describe actions necessary to clean the current directory
clean:
	rm -f *.o polysaurus mystep2 *~
