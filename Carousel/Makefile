CC = gcc
OBJ = Carousel.o LoadShader.o Matrix.o Setup.o
CFLAGS = -g -Wall -Wextra

LDLIBS=-lm -lglut -lGLEW -lGL

Carousel: $(OBJ)
	 $(CC) -o $@ $^ $(CFLAGS) $(LDLIBS)

clean:
	rm -f *.o Carousel 
run: clean Carousel
	./Carousel
.PHONY: all clean run
