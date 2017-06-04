CC = gcc
CFLAGS = -O0 -I. -I../include -I../thirdparty/multihash/include -I../thirdparty/multiaddr/include -I../thirdparty/protobuf/include -I../thirdparty/libp2p/include -I../../lmdb/libraries/liblmdb -Wall

ifdef DEBUG
CFLAGS += -g3
endif

LFLAGS =
DEPS = ../include/ipfsdatastore/ds_helper.h
OBJS = ds_helper.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(OBJS)

clean:
	rm -f *.o
