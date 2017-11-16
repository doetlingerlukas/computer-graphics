CC = g++
LD = g++

OBJ = Radiosity.o 
TARGET = Radiosity

CFLAGS = -O3 -Wall -std=c++1y
LDLIBS =
INCLUDES = 

SRC_DIR = 
BUILD_DIR = 
VPATH = 

# Rules
all: $(TARGET)

$(TARGET).o: $(TARGET).cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $^ -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $^ -o $@

clean:
	rm -f *.o $(TARGET)
	
run: clean all
		./Radiosity

.PHONY: clean run

# Dependencies
$(TARGET): $(OBJ)

