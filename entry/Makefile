CC = gcc
CFLAGS = -O0 -I../include -I../thirdparty -I../thirdparty/multihash/include -I../thirdparty/multiaddr/include -I../thirdparty/protobuf -I../thirdparty/libp2p/include -g3 -Wall -std=c99
LFLAGS = -L../build -lm -lmultihash -lmultiaddr -lprotobuf -lp2p -lpthread -lresolv
DEPS = repo/test_repo_bootstrap_peers.h repo/test_repo_config.h repo/test_repo_identity.h cid/test_cid.h
OBJS = \
	entry.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

entry: $(OBJS)
	$(CC) -o $@ $^ $(LFLAGS) ../thirdparty/liblmdb/liblmdb.a

all: entry

clean:
	rm -f *.o
	rm -f entry
