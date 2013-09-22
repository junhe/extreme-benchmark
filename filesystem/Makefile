# Define required macros here
SHELL=/bin/sh

OBJS=main.o
CFLAGS=-Wall -D_FILE_OFFSET_BITS=64
CC=gcc
INCLUDES=
LIBS=-lpthread -lrt
fsbench:${OBJS}
	${CC} ${CFLAGS} ${INCLUDES} -o $@ ${OBJS} ${LIBS}

clean:
	-rm -f *.o core *.core *.gch

.cpp.o:
	${CC} ${CFLAGS} ${INCLUDES} -c $<
