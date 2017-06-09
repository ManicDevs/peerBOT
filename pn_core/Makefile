CC = gcc
CFLAGS = -O0 -I../include -I../thirdparty/libp2p/include -I../thirdparty/multiaddr/include -I../thirdparty/multihash/include -I../thirdparty/protobuf -Wall -std=c99

ifdef DEBUG
CFLAGS += -g3
endif

LFLAGS =
DEPS = pn_core/config/identity.h pn_core/core.h
OBJS = core.c

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(OBJS)
	cd config; make all

clean:
	rm -f *.o
	cd config; make clean
