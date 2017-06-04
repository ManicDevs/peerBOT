/***
 * a thin wrapper over a datastore for getting and putting block objects
 */
#include "libp2p/crypto/encoding/base32.h"
#include "ipfs/cid/cid.h"
#include "ipfs/blocks/block.h"
#include "ipfs/datastore/ds_helper.h"
#include "ipfs/repo/fsrepo/fs_repo.h"
#include "libp2p/os/utils.h"

/**
 * Delete a block based on its Cid
 * @param cid the Cid to look for
 * @param returns true(1) on success
 */
int ipfs_blockstore_delete(struct Cid* cid, struct FSRepo* fs_repo) {
	return 0;
}

/***
 * Determine if the Cid can be found
 * @param cid the Cid to look for
 * @returns true(1) if found
 */
int ipfs_blockstore_has(struct Cid* cid, struct FSRepo* fs_repo) {
	return 0;
}

unsigned char* ipfs_blockstore_cid_to_base32(const struct Cid* cid) {
	size_t key_length = libp2p_crypto_encoding_base32_encode_size(cid->hash_length);
	unsigned char* buffer = (unsigned char*)malloc(key_length + 1);
	if (buffer == NULL)
		return NULL;
	int retVal = ipfs_datastore_helper_ds_key_from_binary(cid->hash, cid->hash_length, &buffer[0], key_length, &key_length);
	if (retVal == 0) {
		free(buffer);
		return NULL;
	}
	buffer[key_length] = 0;
	return buffer;
}

unsigned char* ipfs_blockstore_hash_to_base32(const unsigned char* hash, size_t hash_length) {
	size_t key_length = libp2p_crypto_encoding_base32_encode_size(hash_length);
	unsigned char* buffer = (unsigned char*)malloc(key_length + 1);
	if (buffer == NULL)
		return NULL;
	int retVal = ipfs_datastore_helper_ds_key_from_binary(hash, hash_length, &buffer[0], key_length, &key_length);
	if (retVal == 0) {
		free(buffer);
		return NULL;
	}
	buffer[key_length] = 0;
	return buffer;
}

char* ipfs_blockstore_path_get(const struct FSRepo* fs_repo, const char* filename) {
	int filepath_size = strlen(fs_repo->path) +  12;
	char filepath[filepath_size];
	int retVal = os_utils_filepath_join(fs_repo->path, "blockstore", filepath, filepath_size);
	if (retVal == 0) {
		free(filepath);
		return 0;
	}
	int complete_filename_size = strlen(filepath) + strlen(filename) + 2;
	char* complete_filename = (char*)malloc(complete_filename_size);
	retVal = os_utils_filepath_join(filepath, filename, complete_filename, complete_filename_size);
	return complete_filename;
}

/***
 * Find a block based on its Cid
 * @param cid the Cid to look for
 * @param block where to put the data to be returned
 * @returns true(1) on success
 */
int ipfs_blockstore_get(const unsigned char* hash, size_t hash_size, struct Block** block, const struct FSRepo* fs_repo) {
	// get datastore key, which is a base32 key of the multihash
	unsigned char* key = ipfs_blockstore_hash_to_base32(hash, hash_size);

	char* filename = ipfs_blockstore_path_get(fs_repo, (char*)key);

	size_t file_size = os_utils_file_size(filename);
	unsigned char buffer[file_size];

	FILE* file = fopen(filename, "rb");
	size_t bytes_read = fread(buffer, 1, file_size, file);
	fclose(file);

	int retVal = ipfs_blocks_block_protobuf_decode(buffer, bytes_read, block);

	free(key);
	free(filename);

	return retVal;
}

/***
 * Put a block in the blockstore
 * @param block the block to store
 * @returns true(1) on success
 */
int ipfs_blockstore_put(struct Block* block, struct FSRepo* fs_repo) {
	// from blockstore.go line 118
	int retVal = 0;

	// Get Datastore key, which is a base32 key of the multihash,
	unsigned char* key = ipfs_blockstore_cid_to_base32(block->cid);
	if (key == NULL) {
		free(key);
		return 0;
	}

	//TODO: put this in subdirectories

	// turn the block into a binary array
	size_t protobuf_len = ipfs_blocks_block_protobuf_encode_size(block);
	unsigned char protobuf[protobuf_len];
	retVal = ipfs_blocks_block_protobuf_encode(block, protobuf, protobuf_len, &protobuf_len);
	if (retVal == 0) {
		free(key);
		return 0;
	}

	// now write byte array to file
	char* filename = ipfs_blockstore_path_get(fs_repo, (char*)key);
	if (filename == NULL) {
		free(key);
		return 0;
	}

	FILE* file = fopen(filename, "wb");
	int bytes_written = fwrite(protobuf, 1, protobuf_len, file);
	fclose(file);
	if (bytes_written != protobuf_len) {
		free(key);
		free(filename);
		return 0;
	}

	// send to Put with key (this is now done separately)
	//fs_repo->config->datastore->datastore_put(key, key_length, block->data, block->data_length, fs_repo->config->datastore);

	free(key);
	free(filename);
	return 1;
}

/***
 * Put a struct UnixFS in the blockstore
 * @param unix_fs the structure
 * @param fs_repo the repo to place the strucure in
 * @param bytes_written the number of bytes written to the blockstore
 * @returns true(1) on success
 */
int ipfs_blockstore_put_unixfs(const struct UnixFS* unix_fs, const struct FSRepo* fs_repo, size_t* bytes_written) {
	// from blockstore.go line 118
	int retVal = 0;

	// Get Datastore key, which is a base32 key of the multihash,
	unsigned char* key = ipfs_blockstore_hash_to_base32(unix_fs->hash, unix_fs->hash_length);
	if (key == NULL) {
		free(key);
		return 0;
	}

	//TODO: put this in subdirectories

	// turn the block into a binary array
	size_t protobuf_len = ipfs_unixfs_protobuf_encode_size(unix_fs);
	unsigned char protobuf[protobuf_len];
	retVal = ipfs_unixfs_protobuf_encode(unix_fs, protobuf, protobuf_len, &protobuf_len);
	if (retVal == 0) {
		free(key);
		return 0;
	}

	// now write byte array to file
	char* filename = ipfs_blockstore_path_get(fs_repo, (char*)key);
	if (filename == NULL) {
		free(key);
		return 0;
	}

	FILE* file = fopen(filename, "wb");
	*bytes_written = fwrite(protobuf, 1, protobuf_len, file);
	fclose(file);
	if (*bytes_written != protobuf_len) {
		free(key);
		free(filename);
		return 0;
	}

	// send to Put with key (this is now done separately)
	//fs_repo->config->datastore->datastore_put(key, key_length, block->data, block->data_length, fs_repo->config->datastore);

	free(key);
	free(filename);
	return 1;
}

/***
 * Find a UnixFS struct based on its hash
 * @param hash the hash to look for
 * @param hash_length the length of the hash
 * @param unix_fs the struct to fill
 * @param fs_repo where to look for the data
 * @returns true(1) on success
 */
int ipfs_blockstore_get_unixfs(const unsigned char* hash, size_t hash_length, struct UnixFS** block, const struct FSRepo* fs_repo) {
	// get datastore key, which is a base32 key of the multihash
	unsigned char* key = ipfs_blockstore_hash_to_base32(hash, hash_length);

	char* filename = ipfs_blockstore_path_get(fs_repo, (char*)key);

	size_t file_size = os_utils_file_size(filename);
	unsigned char buffer[file_size];

	FILE* file = fopen(filename, "rb");
	size_t bytes_read = fread(buffer, 1, file_size, file);
	fclose(file);

	int retVal = ipfs_unixfs_protobuf_decode(buffer, bytes_read, block);

	free(key);
	free(filename);

	return retVal;
}

/***
 * Put a struct Node in the blockstore
 * @param node the structure
 * @param fs_repo the repo to place the strucure in
 * @param bytes_written the number of bytes written to the blockstore
 * @returns true(1) on success
 */
int ipfs_blockstore_put_node(const struct HashtableNode* node, const struct FSRepo* fs_repo, size_t* bytes_written) {
	// from blockstore.go line 118
	int retVal = 0;

	// Get Datastore key, which is a base32 key of the multihash,
	unsigned char* key = ipfs_blockstore_hash_to_base32(node->hash, node->hash_size);
	if (key == NULL) {
		free(key);
		return 0;
	}

	//TODO: put this in subdirectories

	// turn the block into a binary array
	size_t protobuf_len = ipfs_hashtable_node_protobuf_encode_size(node);
	unsigned char protobuf[protobuf_len];
	retVal = ipfs_hashtable_node_protobuf_encode(node, protobuf, protobuf_len, &protobuf_len);
	if (retVal == 0) {
		free(key);
		return 0;
	}

	// now write byte array to file
	char* filename = ipfs_blockstore_path_get(fs_repo, (char*)key);
	if (filename == NULL) {
		free(key);
		return 0;
	}

	FILE* file = fopen(filename, "wb");
	*bytes_written = fwrite(protobuf, 1, protobuf_len, file);
	fclose(file);
	if (*bytes_written != protobuf_len) {
		free(key);
		free(filename);
		return 0;
	}

	free(key);
	free(filename);
	return 1;
}

/***
 * Find a UnixFS struct based on its hash
 * @param hash the hash to look for
 * @param hash_length the length of the hash
 * @param unix_fs the struct to fill
 * @param fs_repo where to look for the data
 * @returns true(1) on success
 */
int ipfs_blockstore_get_node(const unsigned char* hash, size_t hash_length, struct HashtableNode** node, const struct FSRepo* fs_repo) {
	// get datastore key, which is a base32 key of the multihash
	unsigned char* key = ipfs_blockstore_hash_to_base32(hash, hash_length);

	char* filename = ipfs_blockstore_path_get(fs_repo, (char*)key);

	size_t file_size = os_utils_file_size(filename);
	unsigned char buffer[file_size];

	FILE* file = fopen(filename, "rb");
	size_t bytes_read = fread(buffer, 1, file_size, file);
	fclose(file);

	int retVal = ipfs_hashtable_node_protobuf_decode(buffer, bytes_read, node);

	free(key);
	free(filename);

	return retVal;
}

