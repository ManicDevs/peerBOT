CC = gcc
CFLAGS = -O0 -I. -I../include -I../thirdparty -I../thirdparty/multihash/include -I../thirdparty/multiaddr/include -I../thirdparty/protobuf -I../thirdparty/libp2p/include -Wall -std=c99

ifdef DEBUG
CFLAGS += -g3
endif

LFLAGS =
DEPS =
OBJS = merkledag.o node.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(OBJS)

clean:
	rm -f *.o
