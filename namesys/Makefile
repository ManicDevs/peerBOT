CC = gcc
CFLAGS = -O0 -I. -I../include -I../thirdparty -I../thirdparty/multihash/include -I../thirdparty/multiaddr/include -I../thirdparty/protobuf -I../thirdparty/libp2p/include -Wall

ifdef DEBUG
CFLAGS += -g3
endif

LFLAGS =
DEPS =
OBJS = base.o dns.o isdomain.o namesys.o proquint.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(OBJS)

clean:
	rm -f *.o
