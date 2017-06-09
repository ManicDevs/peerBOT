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
	cd pn_logger; make all
	cd pn_core; make all

peerbot:
	@if test ! -f build/libp2p.a; then \
		echo "Please use only 'make'"; \
		exit 1; \
	fi
	cd pn_entry; make all
	cp pn_entry/entry build/peerbot

install:
	@echo "[Peerbot Installation]"
	install build/peerbot /usr/local/sbin/peerbot

clean:
	@echo "[Thirdparty Cleanup]"
	cd thirdparty; make clean
	@echo "[Peerbot Cleanup]"
	cd pn_entry; make clean
	cd pn_core; make clean
	cd pn_logger; make clean
	rm -rf build/
