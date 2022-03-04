GCC = gcc
SOURCES = $(wildcard *.c)
BINAIRES = $(patsubst %.c,%.o,${SOURCES})
FLAGS = -Wall -O3
LIBS = -lSDL2 -lSDL2_ttf -lm

all: blocks

blocks: ${BINAIRES}
	${GCC} $^ -o $@ ${LIBS}
%.o: %.c sdl.h
%.o: %.c blocks.h
	${GCC} -c ${FLAGS} $<
	
clean:
	rm -f *.o blocks

