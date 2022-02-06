GCC = gcc
SOURCES = $(wildcard *.c)
BINAIRES = $(patsubst %.c,%.o,${SOURCES})
FLAGS = -g -Wall 
LIBS = -lSDL2

all: blocks

blocks: ${BINAIRES}
	${GCC} $^ -o $@ ${LIBS}
%.o: %.c sdl.h
%.o: %.c blocks.h
	${GCC} -c ${FLAGS} $<
	
clean:
	rm -f *.o blocks

