# Define the file suffixes for C++ source files
.SUFFIXES:
.SUFFIXES: .cpp $(SUFFIXES)

# Define the command to be used for compilation
CC = g++

# Define the object files to be used
OBJS = # none

# Describe how to build the executable file
sift_realsolns: sift_realsolns.o $(OBJS)
	$(CC) -o sift_realsolns sift_realsolns.o $(OBJS)

# Describe how to create the object file
.cpp.o:
	$(CC) -c $<
.c.o:
	$(CC) -c $<

.cpp:
	$(CC) -c $<
.c:
	$(CC) -c $<


# Describe actions necessary to clean the current directory
clean:
	rm -f *.o sift_realsolns *~
