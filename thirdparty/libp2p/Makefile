DEBUG = true
export DEBUG

OBJS = \
	$(wildcard conn/*.o) \
	$(wildcard crypto/*.o) \
	$(wildcard crypto/encoding/*.o) \
	$(wildcard db/*.o) \
	$(wildcard thirdparty/mbedtls/*.o) \
	hashmap/hashmap.o \
	$(wildcard net/*.o) \
	$(wildcard nodeio/*.o) \
	$(wildcard os/*.o) \
	$(wildcard peer/*.o) \
	$(wildcard record/*.o) \
	$(wildcard routing/*.o) \
	$(wildcard secio/*.o) \
	$(wildcard utils/*.o)

all: test

link: 
	ar rcs libp2p.a $(OBJS)

test: compile link
	cd test; make all

compile:
	cd conn; make all
	cd crypto; make all
	cd db; make all
	cd thirdparty; make all
	cd hashmap; make all
	cd net; make all
	cd nodeio; make all
	cd os; make all
	cd peer; make all
	cd record; make all
	cd routing; make all
	cd secio; make all
	cd utils; make all

clean:
	cd conn; make clean
	cd crypto; make clean
	cd db; make clean
	cd thirdparty; make clean
	cd hashmap; make clean
	cd net; make clean
	cd nodeio; make clean
	cd os; make clean
	cd peer; make clean
	cd record; make clean
	cd routing; make clean
	cd secio; make clean
	cd utils; make clean
	cd test; make clean
	rm -rf libp2p.a
