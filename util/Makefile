CC = gcc
CFLAGS = -O0 -I. -I../include -I../thirdparty/multihash/include -I../thirdparty/multiaddr/include -I../thirdparty/protobuf/include -I../thirdparty/libp2p/include -Wall

ifdef DEBUG
CFLAGS += -g3
endif

LFLAGS =
DEPS =
OBJS = errs.o time.o thread_pool.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(OBJS)

clean:
	rm -f *.o
