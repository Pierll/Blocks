FLAGS = -lSDL2-2.0 -g
SOURCES = $(wildcard *.c)
BINAIRES = $(patsubst %.c,%.o,${SOURCES})


all: tetris

tetris: ${BINAIRES}
	gcc  $^ -o $@ -lSDL2

#%.o: %.c sdl.h
#%.o: %.c tetris.h

	gcc  -c $< 
	
clean:
	rm *.o
	rm tetris
