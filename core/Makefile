CC = gcc
CFLAGS = -O0 -I. -I../include -I../thirdparty -I../thirdparty/multihash/include -I../thirdparty/multiaddr/include -I../thirdparty/protobuf -I../thirdparty/libp2p/include -Wall -std=c99

ifdef DEBUG
CFLAGS += -g3
endif

LFLAGS =
DEPS = builder.h ipfs_node.h
OBJS = builder.o daemon.o null.o ping.o bootstrap.o ipfs_node.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(OBJS)

clean:
	rm -f *.o
