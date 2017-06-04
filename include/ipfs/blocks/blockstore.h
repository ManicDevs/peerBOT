/***
 * a thin wrapper over a datastore for getting and putting block objects
 */

#ifndef __IPFS_BLOCKS_BLOCKSTORE_H__
#define __IPFS_BLOCKS_BLOCKSTORE_H__

#include "ipfs/cid/cid.h"
#include "ipfs/repo/fsrepo/fs_repo.h"

/**
 * Delete a block based on its Cid
 * @param cid the Cid to look for
 * @param returns true(1) on success
 */
int ipfs_blockstore_delete(struct Cid* cid, struct FSRepo* fs_repo);

/***
 * Determine if the Cid can be found
 * @param cid the Cid to look for
 * @returns true(1) if found
 */
int ipfs_blockstore_has(struct Cid* cid, struct FSRepo* fs_repo);

/***
 * Find a block based on its Cid
 * @param cid the Cid to look for
 * @param block where to put the data to be returned
 * @returns true(1) on success
 */
int ipfs_blockstore_get(const unsigned char* hash, size_t hash_length, struct Block** block, const struct FSRepo* fs_repo);

/***
 * Put a block in the blockstore
 * @param block the block to store
 * @returns true(1) on success
 */
int ipfs_blockstore_put(struct Block* block, const struct FSRepo* fs_repo);

/***
 * Put a struct UnixFS in the blockstore
 * @param unix_fs the structure
 * @param fs_repo the repo to place the strucure in
 * @param bytes_written the number of bytes written to the blockstore
 * @returns true(1) on success
 */
int ipfs_blockstore_put_unixfs(const struct UnixFS* unix_fs, const struct FSRepo* fs_repo, size_t* bytes_written);

/***
 * Find a UnixFS struct based on its hash
 * @param hash the hash to look for
 * @param hash_length the length of the hash
 * @param unix_fs the struct to fill
 * @param fs_repo where to look for the data
 * @returns true(1) on success
 */
int ipfs_blockstore_get_unixfs(const unsigned char* hash, size_t hash_length, struct UnixFS** block, const struct FSRepo* fs_repo);

/**
 * Put a struct Node in the blockstore
 */
int ipfs_blockstore_put_node(const struct HashtableNode* node, const struct FSRepo* fs_repo, size_t* bytes_written);
int ipfs_blockstore_get_node(const unsigned char* hash, size_t hash_length, struct HashtableNode** node, const struct FSRepo* fs_repo);

#endif
