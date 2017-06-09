CC = gcc
CFLAGS = -O0 -I../../include -I../../thirdparty/libp2p/include -I../../thirdparty/multiaddr/include -I../../thirdparty/multihash/include -I../../thirdparty/protobuf -Wall -std=c99

ifdef DEBUG
CFLAGS += -g3
endif

LFLAGS =
DEPS = pn_core/config/config.h pn_core/config/identity.h
OBJS = config.o identity.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(OBJS)

clean:
	rm -f *.o
