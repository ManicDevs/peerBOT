all:
	cd thirdparty; make all
	mkdir build/
	cp thirdparty/protobuf/test/test_protobuf build/
	cp thirdparty/multiaddr/test_multiaddr build/
	cp thirdparty/multiaddr/libmultiaddr.a build/
	cp thirdparty/multihash/libmultihash.a build/
	cp thirdparty/libp2p/libp2p.a build/

clean:
	cd thirdparty; make clean
	rm -rf build/