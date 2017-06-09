CC = gcc
CFLAGS = -O0 -I../include -I../thirdparty -I../thirdparty/multihash/include -I../thirdparty/multiaddr/include -I../thirdparty/protobuf -I../thirdparty/libp2p/include -g3 -Wall -std=c99
LFLAGS = -L../build -lp2p -lm -lmultihash -lmultiaddr -lprotobuf -lpthread
DEPS =
OBJS = \
	../pn_logger/logger.o \
	../pn_core/config/identity.o \
	../pn_core/config/config.o \
	../pn_core/core.o \
	entry.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

entry: $(OBJS)
	$(CC) -o $@ $^ $(LFLAGS) ../thirdparty/liblmdb/liblmdb.a

all: entry

clean:
	rm -f *.o
	rm -f entry
