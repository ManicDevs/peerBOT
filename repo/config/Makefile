CC = gcc
CFLAGS = -O0 -I. -I../../include -I../../thirdparty/multihash/include -I../../thirdparty/multiaddr/include -I../../thirdparty/protobuf/include -I../../thirdparty/libp2p/include -Wall -std=c99

ifdef DEBUG
CFLAGS += -g3
endif

LFLAGS =
DEPS = config.h datastore.h identity.h
OBJS = config.o identity.o bootstrap_peers.o gateway.o addresses.o swarm.o peer.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(OBJS)

clean:
	rm -f *.o
