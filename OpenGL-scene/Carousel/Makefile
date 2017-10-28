CC = gcc
OBJ = Carousel.o LoadShader.o LoadTexture.o Matrix.o StringExtra.o OBJParser.o List.o Setup.o
CFLAGS = -g -Wno-unused-variable -Wno-unused-parameter -Wall -Wextra
LDLIBS = -lm -lglut -lGLEW -lGL

Carousel: $(OBJ)
		$(CC) -o $@ $^ $(CFLAGS) $(LDLIBS)

clean:
	rm -f *.o Carousel 
	
run: clean Carousel
	./Carousel

.PHONY: all clean run



