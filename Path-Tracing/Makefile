CC = g++
LD = g++

OBJ = PathTracing.o Structs.o OBJReader.o
TARGET = PathTracing

CFLAGS = -O3 -Wall -Wextra -std=c++1y -fopenmp
LDLIBS =
INCLUDES = 

SRC_DIR = 
BUILD_DIR = 
VPATH = 

# Rules
all: $(TARGET)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $^ -o $@

clean:
	rm -f *.o $(TARGET)
	
run: clean all
		./PathTracing

.PHONY: clean run

# Dependencies
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@
