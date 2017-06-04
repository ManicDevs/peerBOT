/***
 * A content id
 */

#ifndef __IPFS_CID_CID_H
#define __IPFS_CID_CID_H

#include <stddef.h>
#include "protobuf/protobuf.h"

#define CID_PROTOBUF 0x70
#define CID_CBOR 0x71
#define CID_RAW 0x72
#define CID_JSON 0x73
#define CID_ETHEREUM_BLOCK 0x90
#define CID_ETHEREUM_TX 0x91
#define CID_BITCOIN_BLOCK 0xb0
#define CID_BITCOIN_TX 0xb1
#define CID_ZCASH_BLOCK 0xc0
#define CID_ZCASH_TX 0xc1

struct Cid {
	int version;
	char codec;
	unsigned char* hash; // a multihash
	size_t hash_length;
};

struct CidSet {
    struct Cid *cid;
    struct CidSet *next;
};

/***
 * encode a Cid into a protobuf array of bytes
 * @param incoming the incoming Cid struct
 * @param buffer the buffer
 * @param max_buffer_length the length of the buffer
 * @param bytes_written the number of bytes written
 */
int ipfs_cid_protobuf_encode(struct Cid* incoming, unsigned char* buffer, size_t max_buffer_length, size_t* bytes_written);

/***
 * decode an array of bytes into a Cid structure
 * @param buffer the incming array of bytes
 * @param buffer_length the length of the buffer
 * @param output the Cid struct NOTE: all allocations are made by this function. Be sure to call free
 * @returns true(1) on success
 */
int ipfs_cid_protobuf_decode(unsigned char* buffer, size_t buffer_length, struct Cid** output);

/***
 * Returns a safe estimate of the required buffer size to encode the Cid struct
 * @param incoming the struct to encode
 * @returns the number of approximate bytes
 */
size_t ipfs_cid_protobuf_encode_size(struct Cid* incoming);

/**
 * Create a new CID based on the given hash
 * @param version the version
 * @param hash the multihash
 * @param hash_length the length of the multihash in bytes
 * @param codec the codec to be used (NOTE: For version 0, this should be CID_PROTOBUF)
 * @param cid where to put the results
 * @returns true(1) on success
 */
int ipfs_cid_new(int version, unsigned char* hash, size_t hash_length, const char codec, struct Cid** cid);

/***
 * Free the resources from a Cid
 * @param cid the struct
 * @returns 1
 */
int ipfs_cid_free(struct Cid* cid);

/***
 * Fill a Cid struct based on a base 58 encoded string
 * @param incoming the string
 * @param incoming_size the size of the string
 * @cid the Cid struct to fill
 * @return true(1) on success
 */
int ipfs_cid_decode_hash_from_base58(const unsigned char* incoming, size_t incoming_length, struct Cid** cid);

/**
 * Turn a cid into a base 58 of a multihash of the cid hash
 * @param cid the cid to work with
 * @param buffer where to put the results
 * @param max_buffer_length the maximum space reserved for the results
 * @returns true(1) on success
 */
int ipfs_cid_hash_to_base58(const unsigned char* hash, size_t hash_length, unsigned char* buffer, size_t max_buffer_length);

/***
 * Turn a multibase decoded string of bytes into a Cid struct
 * @param incoming the multibase decoded array
 * @param incoming_size the size of the array
 * @param cid the Cid structure to fill
 */
int ipfs_cid_cast(const unsigned char* incoming, size_t incoming_size, struct Cid* cid);

struct CidSet *ipfs_cid_set_new ();
void ipfs_cid_set_destroy (struct CidSet **set);
int ipfs_cid_set_add (struct CidSet *set, struct Cid *cid, int visit);
int ipfs_cid_set_has (struct CidSet *set, struct Cid *cid);
int ipfs_cid_set_remove (struct CidSet *set, struct Cid *cid);
int ipfs_cid_set_len (struct CidSet *set);
unsigned char **ipfs_cid_set_keys (struct CidSet *set);
int ipfs_cid_set_foreach (struct CidSet *set, int (*func)(struct Cid *));

#endif
