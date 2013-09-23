# Define required macros here
SHELL=/bin/sh

OBJS=main.o
CFLAGS=-Wall -D_FILE_OFFSET_BITS=64
CC=gcc
INCLUDES=
LIBS=-lpthread -lrt

all: vmbench seg_disturbor
seg_disturbor: seg_disturbor.o
	${CC} ${CFLAGS} ${INCLUDES} -o $@ seg_disturbor.o ${LIBS}

vmbench:main.o
	${CC} ${CFLAGS} ${INCLUDES} -o $@ main.o ${LIBS}

clean:
	-rm -f *.o core *.core *.gch

.cpp.o:
	${CC} ${CFLAGS} ${INCLUDES} -c $<
