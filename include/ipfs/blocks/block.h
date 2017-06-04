/***
 * IPFS has the notion of storage blocks.
 * Raw data with a multihash key (the Cid)
 */

#ifndef __IPFS_BLOCKS_BLOCK_H__
#define __IPFS_BLOCKS_BLOCK_H__

#include "ipfs/cid/cid.h"

struct Block {
	struct Cid* cid;
	unsigned char* data;
	size_t data_length;
};

/***
 * Create a new block based on the incoming data.
 * @param data the data to base the block on
 * @param data_size the length of the data array
 * @param block a pointer to the struct Block that will be created
 * @returns true(1) on success
 */
int ipfs_blocks_block_new(struct Block** block);

int ipfs_blocks_block_add_data(const unsigned char* data, size_t data_size, struct Block* block);

/***
 * Free resources used by the creation of a block
 * @param block the block to free
 * @returns true(1) on success
 */
int ipfs_blocks_block_free(struct Block* block);

/**
 * Determine the approximate size of an encoded block
 * @param block the block to measure
 * @returns the approximate size needed to encode the protobuf
 */
size_t ipfs_blocks_block_protobuf_encode_size(const struct Block* block);

/**
 * Encode the Block into protobuf format
 * @param block the block to encode
 * @param buffer the buffer to fill
 * @param max_buffer_size the max size of the buffer
 * @param bytes_written the number of bytes used
 * @returns true(1) on success
 */
int ipfs_blocks_block_protobuf_encode(const struct Block* block, unsigned char* buffer, size_t max_buffer_size, size_t* bytes_written);

/***
 * Decode from a protobuf stream into a Block struct
 * @param buffer the buffer to pull from
 * @param buffer_length the length of the buffer
 * @param block the block to fill
 * @returns true(1) on success
 */
int ipfs_blocks_block_protobuf_decode(const unsigned char* buffer, const size_t buffer_length, struct Block** block);

#endif
