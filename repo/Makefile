CC = gcc
CFLAGS = -O0 -I. -I../include -I../thirdparty/multihash/include -I../thirdparty/multiaddr/include -I../thirdparty/protobuf/include -I../thirdparty/libp2p/include -std=c99

ifdef DEBUG
CFLAGS += -g3
endif

DEPS =
OBJS = init.c

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(OBJS)
	cd config; make all;
	cd fsrepo; make all;
clean:
	rm -f *.o
	cd config; make clean;
	cd fsrepo; make clean;
