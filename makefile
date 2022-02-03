GCC = gcc
SOURCES = $(wildcard *.c)
BINAIRES = $(patsubst %.c,%.o,${SOURCES})
FLAGS = -g -Wall 
LIBS = -lSDL2

all: tetris

tetris: ${BINAIRES}
	${GCC} $^ -o $@ ${LIBS}
%.o: %.c sdl.h
%.o: %.c tetris.h
	${GCC} -c ${FLAGS} $<
	
clean:
	rm -f *.o tetris