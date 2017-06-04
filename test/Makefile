CC = gcc
CFLAGS = -O0 -I. -I../include -I../thirdparty -I../thirdparty/multihash/include -I../thirdparty/multiaddr/include -I../thirdparty/protobuf -I../thirdparty/libp2p/include -g3 -Wall -std=c99
LFLAGS = -L../build -lp2p -lm -lmultihash -lmultiaddr -lpthread
DEPS = cmd/ipfs/test_init.h repo/test_repo_bootstrap_peers.h repo/test_repo_config.h repo/test_repo_identity.h cid/test_cid.h
OBJS = testit.o test_helper.o \
	../blocks/block.o ../blocks/blockstore.o \
	../cid/cid.o \
	../cmd/ipfs/init.o \
	../commands/argument.o ../commands/command_option.o ../commands/command.o ../commands/cli/parse.o \
	../core/builder.o \
	../core/daemon.o \
	../core/null.o \
	../core/bootstrap.o \
	../core/ping.o \
	../core/ipfs_node.o \
	../datastore/ds_helper.o \
	../flatfs/flatfs.o \
	../importer/importer.o ../importer/exporter.o ../importer/resolver.o \
	../merkledag/merkledag.o ../merkledag/node.o \
	../multibase/multibase.o \
	../repo/init.o \
	../repo/fsrepo/fs_repo.o ../repo/fsrepo/jsmn.o ../repo/fsrepo/lmdb_datastore.o \
	../repo/config/config.o ../repo/config/identity.o \
	../repo/config/bootstrap_peers.o ../repo/config/gateway.o \
	../repo/config/addresses.o ../repo/config/swarm.o ../repo/config/peer.o \
	../routing/offline.o \
	../routing/online.o \
	../routing/k_routing.o \
	../routing/supernode.o \
	../thirdparty/ipfsaddr/ipfs_addr.o \
	../unixfs/unixfs.o \
	../util/thread_pool.o \
	../thirdparty/protobuf/protobuf.o ../thirdparty/protobuf/varint.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

test_ipfs: $(OBJS)
	$(CC) -o $@ $^ $(LFLAGS) ../build/liblmdb.a

all: test_ipfs

clean:
	rm -f *.o
	rm -f test_ipfs
