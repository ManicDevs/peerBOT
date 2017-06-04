#include "libp2p/crypto/encoding/base32.h"
#include "ipfs/datastore/ds_helper.h"
#include "ipfs/blocks/block.h"
#include "ipfs/repo/config/config.h"
#include "ipfs/repo/fsrepo/fs_repo.h"

#include "../test_helper.h"

#include <dirent.h>
#include <sys/stat.h>

/**
 * create a repository and put a record in the datastore and a block in the blockstore
 */
int test_ipfs_datastore_put() {
	struct Block* block;
	int retVal = 0;
	const unsigned char* input = (unsigned char*)"Hello, world!";

	// build the ipfs repository, then shut it down, so we can start fresh
	retVal = drop_and_build_repository("/tmp/.ipfs", 4001, NULL, NULL);
	if (retVal == 0)
		return 0;

	// build the block
	retVal = ipfs_blocks_block_new(&block);
	if (retVal == 0)
		return 0;

	retVal = ipfs_blocks_block_add_data(input, strlen((char*)input), block);
	if (retVal == 0) {
		ipfs_blocks_block_free(block);
		return 0;
	}

	// generate the key
	size_t key_length = libp2p_crypto_encoding_base32_encode_size(block->data_length);
	unsigned char key[key_length];
	retVal = ipfs_datastore_helper_ds_key_from_binary(block->data, block->data_length, &key[0], key_length, &key_length);
	if (retVal == 0) {
		ipfs_blocks_block_free(block);
		return 0;
	}

	// open the repository
	struct FSRepo* fs_repo;
	retVal = ipfs_repo_fsrepo_new("/tmp/.ipfs", NULL, &fs_repo);
	if (retVal == 0) {
		ipfs_blocks_block_free(block);
		return 0;
	}
	retVal = ipfs_repo_fsrepo_open(fs_repo);
	if (retVal == 0) {
		ipfs_blocks_block_free(block);
		return 0;
	}
	// send to Put with key
	retVal = fs_repo->config->datastore->datastore_put((const unsigned char*)key, key_length, block->data, block->data_length, fs_repo->config->datastore);
	if (retVal == 0) {
		ipfs_blocks_block_free(block);
		return 0;
	}

	// save the block

	// check the results

	// clean up
	ipfs_repo_fsrepo_free(fs_repo);
	ipfs_blocks_block_free(block);

	return 1;
}
