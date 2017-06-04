CC = gcc

CFLAGS = -O0 -I. -I../include -I../thirdparty/libp2p/include -Wall

ifdef DEBUG
CFLAGS += -g3
endif

LFLAGS =
DEPS = ../include/flatfs/flatfs.h
OBJS = flatfs.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(OBJS)

clean:
	rm -f *.o
