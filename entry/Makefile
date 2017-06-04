CC = gcc
CFLAGS = -O0 -I../include -I../thirdparty -I../thirdparty/multihash/include -I../thirdparty/multiaddr/include -I../thirdparty/protobuf -I../thirdparty/libp2p/include -g3 -Wall -std=c99
LFLAGS = -L../build -lm -lmultihash -lmultiaddr -lprotobuf -lp2p -lpthread -lresolv
DEPS = cmd/ipfs/test_init.h repo/test_repo_bootstrap_peers.h repo/test_repo_config.h repo/test_repo_identity.h cid/test_cid.h
OBJS = \
	../importer/importer.o ../importer/exporter.o ../importer/resolver.o \
	../unixfs/unixfs.o \
	../merkledag/merkledag.o ../merkledag/node.o \
	../repo/init.o \
	../repo/fsrepo/fs_repo.o ../repo/fsrepo/jsmn.o ../repo/fsrepo/lmdb_datastore.o \
	../repo/config/config.o ../repo/config/identity.o \
	../repo/config/bootstrap_peers.o ../repo/config/gateway.o \
	../repo/config/addresses.o ../repo/config/swarm.o ../repo/config/peer.o \
	../blocks/block.o ../blocks/blockstore.o \
	../cid/cid.o \
	../cmd/ipfs/init.o \
	../commands/argument.o ../commands/command_option.o ../commands/command.o ../commands/cli/parse.o \
	../util/errs.o \
	../util/time.o \
	../util/thread_pool.o \
	../routing/*.o \
	../core/builder.o ../core/daemon.o ../core/null.o ../core/ping.o ../core/bootstrap.o ../core/ipfs_node.o \
	../datastore/ds_helper.o \
	../datastore/key.o \
	../flatfs/flatfs.o \
	../path/path.o \
	../multibase/multibase.o \
	../namesys/*.o \
	../pin/pin.o \
	../thirdparty/ipfsaddr/ipfs_addr.o \
	../thirdparty/protobuf/protobuf.o ../thirdparty/protobuf/varint.o \
	entry.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

entry: $(OBJS)
	$(CC) -o $@ $^ $(LFLAGS) ../thirdparty/liblmdb/liblmdb.a

all: entry

clean:
	rm -f *.o
	rm -f entry
