DEBUG = true
export DEBUG

all: libp2p prebuild peerbot
	@if test -f build/peerbot; then \
		echo "[All Compiled]"; \
	else \
		echo "[Not Compiled]"; \
	fi

libp2p:
	@echo "[Libp2p Compilation]"
	cd thirdparty; make all
	mkdir -p build/
	cp thirdparty/protobuf/test/test_protobuf build/
	cp thirdparty/multiaddr/test_multiaddr build/
	cp thirdparty/multiaddr/libmultiaddr.a build/
	cp thirdparty/multihash/libmultihash.a build/
	cp thirdparty/protobuf/libprotobuf.a build/
	cp thirdparty/libp2p/libp2p.a build/

prebuild:
	@echo "[Peerbot Compilation]"
	cp thirdparty/liblmdb/liblmdb.a build/
	cd thirdparty/ipfsaddr; make all
	cd blocks; make all
	cd cid; make all
	cd commands; make all
	cd core; make all
	cd importer; make all
	cd merkledag; make all
	cd multibase; make all
	cd pin; make all
	cd repo; make all
	cd flatfs; make all
	cd datastore; make all
	cd thirdparty; make all
	cd unixfs; make all
	cd routing; make all
	cd namesys; make all
	cd path; make all
	cd util; make all

peerbot:
	@if test ! -f build/libp2p.a; then \
		echo "Please use only 'make'"; \
		exit 1; \
	fi
	cd entry; make all
	cp entry/entry build/peerbot

install:
	@echo "[Peerbot Installation]"
	install build/peerbot /usr/local/sbin/peerbot

clean:
	@echo "[Thirdparty Cleanup]"
	cd thirdparty; make clean
	@echo "[Peerbot Cleanup]"
	cd thirdparty/ipfsaddr; make clean
	cd blocks; make clean
	cd cid; make clean
	cd commands; make clean
	cd core; make clean
	cd importer; make clean
	cd merkledag; make clean
	cd multibase; make clean
	cd pin; make clean
	cd repo; make clean
	cd flatfs; make clean
	cd datastore; make clean
	cd thirdparty; make clean
	cd unixfs; make clean
	cd routing; make clean
	cd namesys; make clean
	cd path; make clean
	cd util; make clean
	cd entry; make clean
	rm -rf build/
