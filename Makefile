LIBS+= \
	-lp2p \
	-pthread

CFLAGS+= \
	-g \
	-O0 \
	-Wall \
	-Isrc/include \
	-Lbuild/

SRCS = \
	src/main.c

all: libp2p peerbot
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
	cp thirdparty/libp2p/libp2p.a build/

peerbot:
	@echo "[Peerbot Compilation]"
	@if test -f build/libp2p.a; then \
		$(CC) $(LDFLAGS) $(SRCS) $(CFLAGS) $(LIBS) -o build/$@; \
	else \
		echo "Please use only 'make'"; \
	fi

install:
	@echo "[Peerbot Installation]"
	install build/peerbot /usr/local/sbin/peerbot

clean:
	@echo "[Thirdparty Cleanup]"
	cd thirdparty; make clean
	@echo "[Peerbot Cleanup]"
	rm -f src/*.o
	rm -rf build/