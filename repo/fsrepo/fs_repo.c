#include <stdio.h>
#include <sys/stat.h>

#include "libp2p/crypto/encoding/base64.h"
#include "libp2p/crypto/key.h"
#include "libp2p/utils/vector.h"
#include "ipfs/blocks/blockstore.h"
#include "ipfs/datastore/ds_helper.h"
#include "libp2p/db/datastore.h"
#include "libp2p/db/filestore.h"
#include "ipfs/repo/fsrepo/fs_repo.h"
#include "libp2p/os/utils.h"
#include "ipfs/repo/fsrepo/lmdb_datastore.h"
#include "jsmn.h"
#include "multiaddr/multiaddr.h"

/** 
 * private methods
 */

/**
 * writes the config file
 * @param full_filename the full filename of the config file in the OS
 * @param config the details to put into the file
 * @returns true(1) on success, else false(0)
 */
int repo_config_write_config_file(char* full_filename, struct RepoConfig* config) {
	FILE* out_file = fopen(full_filename, "w");
	if (out_file == NULL)
		return 0;
	
	fprintf(out_file, "{\n");
	fprintf(out_file, " \"Identity\": {\n");
	fprintf(out_file, "  \"PeerID\": \"%s\",\n", config->identity->peer_id);
	// print correct format of private key
	// first put it in a protobuf
	struct PrivateKey* priv_key = libp2p_crypto_private_key_new();
	if (priv_key == NULL)
		return 0;
	priv_key->data_size = config->identity->private_key.der_length;
	priv_key->data = (unsigned char*)malloc(priv_key->data_size);
	if (priv_key->data == NULL) {
		libp2p_crypto_private_key_free(priv_key);
		return 0;
	}
	memcpy(priv_key->data, config->identity->private_key.der, priv_key->data_size);
	priv_key->type = KEYTYPE_RSA;
	size_t protobuf_size = libp2p_crypto_private_key_protobuf_encode_size(priv_key);
	unsigned char protobuf[protobuf_size];
	libp2p_crypto_private_key_protobuf_encode(priv_key, protobuf, protobuf_size, &protobuf_size);
	libp2p_crypto_private_key_free(priv_key);
	// then base 64 it
	size_t encoded_size = libp2p_crypto_encoding_base64_encode_size(protobuf_size);
	unsigned char encoded_buffer[encoded_size + 1];
	int retVal = libp2p_crypto_encoding_base64_encode(protobuf, protobuf_size, encoded_buffer, encoded_size, &encoded_size);
	if (retVal == 0)
		return 0;
	encoded_buffer[encoded_size] = 0;
	fprintf(out_file, "  \"PrivKey\": \"%s\"\n", encoded_buffer);
	fprintf(out_file, " },\n");
	fprintf(out_file, " \"Datastore\": {\n");
	fprintf(out_file, "  \"Type\": \"%s\",\n", config->datastore->type);
	fprintf(out_file, "  \"Path\": \"%s\",\n", config->datastore->path);
	fprintf(out_file, "  \"StorageMax\": \"%s\",\n", config->datastore->storage_max);
	fprintf(out_file, "  \"StorageGCWatermark\": %d,\n", config->datastore->storage_gc_watermark);
	fprintf(out_file, "  \"GCPeriod\": \"%s\",\n", config->datastore->gc_period);
	fprintf(out_file, "  \"Params\": null,\n");
	fprintf(out_file, "  \"NoSync\": %s,\n", config->datastore->no_sync ? "true" : "false");
	fprintf(out_file, "  \"HashOnRead\": %s,\n", config->datastore->hash_on_read ? "true" : "false");
	fprintf(out_file, "  \"BloomFilterSize\": %d\n", config->datastore->bloom_filter_size);
	fprintf(out_file, " },\n \"Addresses\": {\n");
	fprintf(out_file, "  \"Swarm\": [\n");
	struct Libp2pLinkedList* current = config->addresses->swarm_head;
	while (current != NULL) {
		fprintf(out_file, "  \"%s\"", (char*)current->item);
		if (current->next == NULL)
			fprintf(out_file, "\n");
		else
			fprintf(out_file, ",\n");
		current = current->next;
	}
	fprintf(out_file, "  ],\n");
	fprintf(out_file, "  \"API\": \"%s\",\n", config->addresses->api);
	fprintf(out_file, "  \"Gateway\": \"%s\"\n", config->addresses->gateway);
	fprintf(out_file, " },\n  \"Mounts\": {\n");
	fprintf(out_file, "  \"IPFS\": \"%s\",\n", config->mounts.ipfs);
	fprintf(out_file, "  \"IPNS\": \"%s\",\n", config->mounts.ipns);
	fprintf(out_file, "  \"FuseAllowOther\": %s\n", "false");
	fprintf(out_file, " },\n  \"Discovery\": {\n   \"MDNS\": {\n");
	fprintf(out_file, "   \"Enabled\": %s,\n", config->discovery.mdns.enabled ? "true" : "false");
	fprintf(out_file, "   \"Interval\": %d\n  }\n },\n", config->discovery.mdns.interval);
	fprintf(out_file, " \"Ipns\": {\n");
	fprintf(out_file, "  \"RepublishedPeriod\": \"\",\n");
	fprintf(out_file, "  \"RecordLifetime\": \"\",\n");
	fprintf(out_file, "  \"ResolveCacheSize\": %d\n", config->ipns.resolve_cache_size);
	fprintf(out_file, " },\n \"Bootstrap\": [\n");
	for(int i = 0; i < config->bootstrap_peers->total; i++) {
		struct MultiAddress* peer = libp2p_utils_vector_get(config->bootstrap_peers, i);
		fprintf(out_file, "  \"%s\"", peer->string);
		if (i < config->bootstrap_peers->total - 1)
			fprintf(out_file, ",\n");
		else
			fprintf(out_file, "\n");
	}
	fprintf(out_file, " ],\n \"Tour\": {\n  \"Last\": \"\"\n },\n");
	fprintf(out_file, " \"Gateway\": {\n");
	fprintf(out_file, "  \"HTTPHeaders\": {\n");
	for (int i = 0; i < config->gateway->http_headers->num_elements; i++) {
		fprintf(out_file, "   \"%s\": [\n    \"%s\"\n  ]", config->gateway->http_headers->headers[i]->header, config->gateway->http_headers->headers[i]->value);
		if (i < config->gateway->http_headers->num_elements - 1)
			fprintf(out_file, ",\n");
		else
			fprintf(out_file, "\n },\n");
	}
	fprintf(out_file, "  \"RootRedirect\": \"%s\"\n", config->gateway->root_redirect);
	fprintf(out_file, "  \"Writable\": %s\n", config->gateway->writable ? "true" : "false");
	fprintf(out_file, "  \"PathPrefixes\": []\n");
	fprintf(out_file, " },\n \"SupernodeRouting\": {\n");
	fprintf(out_file, "  \"Servers\": null\n },");
	fprintf(out_file, " \"API\": {\n  \"HTTPHeaders\": null\n },\n");
	fprintf(out_file, " \"Swarm\": {\n  \"AddrFilters\": null\n }\n}");
	fclose(out_file);
	return 1;
}

/**
 * constructs the FSRepo struct.
 * Remember: ipfs_repo_fsrepo_free must be called
 * @param repo_path the path to the repo
 * @param config the optional config file. NOTE: if passed, fsrepo_free will free resources of the RepoConfig.
 * @param repo the struct to allocate memory for
 * @returns false(0) if something bad happened, otherwise true(1)
 */
int ipfs_repo_fsrepo_new(const char* repo_path, struct RepoConfig* config, struct FSRepo** repo) {
	*repo = (struct FSRepo*)malloc(sizeof(struct FSRepo));

	if (repo_path == NULL) {
		// get the user's home directory
		char* ipfs_path = os_utils_getenv("IPFS_PATH");
		if (ipfs_path == NULL)
			ipfs_path = os_utils_get_homedir();
		char* default_subdir = "/.ipfs";
		unsigned long newPathLen = 0;
		if (strstr(ipfs_path, default_subdir) != NULL) {
			newPathLen = strlen(ipfs_path) + 1;
		} else {
			// add /.ipfs to the string
			newPathLen = strlen(ipfs_path) + strlen(default_subdir) + 2;  // 1 for slash and 1 for end
		}
		(*repo)->path = malloc(sizeof(char) * newPathLen);
		if ((*repo)->path == NULL) {
			free( (*repo));
			return 0;
		}
		if (strstr(ipfs_path, default_subdir) != NULL) {
			strcpy((*repo)->path, ipfs_path);
		} else {
			os_utils_filepath_join(os_utils_get_homedir(), default_subdir, (*repo)->path, newPathLen);
		}
	} else {
		int len = strlen(repo_path) + 1;
		(*repo)->path = (char*)malloc(len);
		strncpy((*repo)->path, repo_path, len);
	}
	// allocate other structures
	if (config != NULL)
		(*repo)->config = config;
	else {
		if (ipfs_repo_config_new(&((*repo)->config)) == 0) {
			free((*repo)->path);
			return 0;
		}
	}
	return 1;
}

/**
 * Cleans up memory
 * @param repo the struct to clean up
 * @returns true(1) on success
 */
int ipfs_repo_fsrepo_free(struct FSRepo* repo) {
	if (repo != NULL) {
		if (repo->path != NULL)
			free(repo->path);
		if (repo->config != NULL)
			ipfs_repo_config_free(repo->config);
		free(repo);
	}
	return 1;
}

/**
 * checks to see if the repo is initialized at the given path
 * @param full_path the path to the repo
 * @returns true(1) if the config file is there, false(0) otherwise
 */
int repo_config_is_initialized(char* full_path) {
	char* config_file_full_path;
	int retVal = repo_config_get_file_name(full_path, &config_file_full_path);
	if (!retVal)
		return 0;
	
	if (os_utils_file_exists(config_file_full_path))
		retVal = 1;
	else
		retVal = 0;
	
	free(config_file_full_path);
	return retVal;
}

/***
 * Check to see if the repo is initialized
 * @param full_path the path to the repo
 * @returns true(1) if it is initialized, false(0) otherwise.
 */
int fs_repo_is_initialized_unsynced(char* full_path) {
	return repo_config_is_initialized(full_path);
}

/**
 * checks to see if the repo is initialized
 * @param full_path the full path to the repo
 * @returns true(1) if it is initialized, otherwise false(0)
 */
int repo_check_initialized(char* full_path) {
	// note the old version of this reported an error if the repo was a .go-ipfs repo (by looking at the path)
	// this version skips that step
	return fs_repo_is_initialized_unsynced(full_path);
}

/***
 * Reads the file, placing its contents in buffer
 * NOTE: this allocates memory for buffer, and should be freed
 * @param path the path to the config file
 * @param buffer where to put the contents
 * @returns true(1) on success
 */
int _read_file(const char* path, char** buffer) {
	int file_size = os_utils_file_size(path);
	if (file_size <= 0)
		return 0;
	// allocate memory
	*buffer = malloc(file_size + 1);
	if (*buffer == NULL) {
		return 0;
	}
	memset(*buffer, 0, file_size + 1);

	// open file
	FILE* in_file = fopen(path, "r");
	// read data
	fread(*buffer, file_size, 1, in_file);

	// cleanup
	fclose(in_file);
	return 1;
}

/**
 * Find the position of a key
 * @param data the string that contains the json
 * @param tokens the tokens of the parsed string
 * @param tok_length the number of tokens there are
 * @param tag what we're looking for
 * @returns the position of the requested token in the array, or -1
 */
int _find_token(const char* data, const jsmntok_t* tokens, int tok_length, int start_from, const char* tag) {
	for(int i = start_from; i < tok_length; i++) {
		jsmntok_t curr_token = tokens[i];
		if ( curr_token.type == JSMN_STRING) {
			// convert to string
			int str_len = curr_token.end - curr_token.start;
			char str[str_len + 1];
			strncpy(str, &data[curr_token.start], str_len );
			str[str_len] = 0;
			if (strcmp(str, tag) == 0)
				return i;
		}
	}
	return -1;
}

/**
 * Retrieves the value of a key / value pair from the JSON data
 * @param data the full JSON string
 * @param tokens the array of tokens
 * @param tok_length the number of tokens
 * @param search_from start search from this token onward
 * @param tag what to search for (NOTE: If null, read from search_from)
 * @param result where to put the result. NOTE: allocates memory that must be freed
 * @returns true(1) on success
 */
int _get_json_string_value(char* data, const jsmntok_t* tokens, int tok_length, int search_from, const char* tag, char** result) {
	int pos = 0;
	jsmntok_t* curr_token = NULL;

	if (tag == NULL) {
		pos = search_from;
		if (pos >= 0)
			curr_token = (jsmntok_t*)&tokens[pos];
	}
	else {
		pos = _find_token(data, tokens, tok_length, search_from, tag);
		if (pos >= 0)
			curr_token = (jsmntok_t*)&tokens[pos + 1];
	}

	if (curr_token == NULL)
		return 0;

	if (curr_token->type == JSMN_PRIMITIVE) {
		// a null
		*result = NULL;
	}
	if (curr_token->type != JSMN_STRING)
		return 0;
	// allocate memory
	int str_len = curr_token->end - curr_token->start;
	*result = malloc(str_len + 1);
	if (*result == NULL)
		return 0;
	// copy in the string
	strncpy(*result, &data[curr_token->start], str_len);
	(*result)[str_len] = 0;
	return 1;
}

/**
 * Retrieves the value of a key / value pair from the JSON data
 * @param data the full JSON string
 * @param tokens the array of tokens
 * @param tok_length the number of tokens
 * @param search_from start search from this token onward
 * @param tag what to search for
 * @param result where to put the result
 * @returns true(1) on success
 */
int _get_json_int_value(char* data, const jsmntok_t* tokens, int tok_length, int search_from, const char* tag, int* result) {
	int pos = _find_token(data, tokens, tok_length, search_from, tag);
	if (pos < 0)
		return 0;
	jsmntok_t curr_token = tokens[pos+1];
	if (curr_token.type != JSMN_PRIMITIVE)
		return 0;
	// allocate memory
	int str_len = curr_token.end - curr_token.start;
	char str[str_len + 1];
	// copy in the string
	strncpy(str, &data[curr_token.start], str_len);
	str[str_len] = 0;
	if (strcmp(str, "true") == 0)
		*result = 1;
	else if (strcmp(str, "false") == 0)
		*result = 0;
	else if (strcmp(str, "null") == 0) // what should we do here?
		*result = 0;
	else // its a real number
		*result = atoi(str);
	return 1;
}

/***
 * Opens the config file and puts the data into the FSRepo struct
 * @param repo the FSRepo struct
 * @returns 0 on failure, otherwise 1
 */
int fs_repo_open_config(struct FSRepo* repo) {
	int retVal;
	char* data;
	size_t full_filename_length = strlen(repo->path) + 8;
	char full_filename[full_filename_length];
	retVal = os_utils_filepath_join(repo->path, "config", full_filename, full_filename_length);
	if (retVal == 0)
		return 0;
	retVal = _read_file(full_filename, &data);
	// parse the data
	jsmn_parser parser;
	jsmn_init(&parser);
	int num_tokens = 256;
	jsmntok_t tokens[num_tokens];
	num_tokens = jsmn_parse(&parser, data, strlen(data), tokens, 256);
	if (num_tokens <= 0) {
		free(data);
		return 0;
	}
	// fill FSRepo struct
	// allocation done by fsrepo_new... repo->config = malloc(sizeof(struct RepoConfig));
	// Identity
	int curr_pos = _find_token(data, tokens, num_tokens, 0, "Identity");
	if (curr_pos < 0) {
		free(data);
		return 0;
	}
	// the next should be the array, then string "PeerID"
	//NOTE: the code below compares the peer id of the file with the peer id generated
	// by the key. If they don't match, we fail.
	char* peer_id = NULL;
	_get_json_string_value(data, tokens, num_tokens, curr_pos, "PeerID", &peer_id);
	char* priv_key_base64;
	// then PrivKey
	_get_json_string_value(data, tokens, num_tokens, curr_pos, "PrivKey", &priv_key_base64);
	retVal = repo_config_identity_build_private_key(repo->config->identity, priv_key_base64);
	if (retVal == 0 || strcmp(peer_id, repo->config->identity->peer_id) != 0) {
		free(data);
		free(priv_key_base64);
		free(peer_id);
		return 0;
	}
	free(peer_id);
	// now the datastore
	//int datastore_position = _find_token(data, tokens, num_tokens, 0, "Datastore");
	_get_json_string_value(data, tokens, num_tokens, curr_pos, "Type", &repo->config->datastore->type);
	_get_json_string_value(data, tokens, num_tokens, curr_pos, "Path", &repo->config->datastore->path);
	_get_json_string_value(data, tokens, num_tokens, curr_pos, "StorageMax", &repo->config->datastore->storage_max);
	_get_json_int_value(data, tokens, num_tokens, curr_pos, "StorageGCWatermark", &repo->config->datastore->storage_gc_watermark);
	_get_json_string_value(data, tokens, num_tokens, curr_pos, "GCPeriod", &repo->config->datastore->gc_period);
	_get_json_string_value(data, tokens, num_tokens, curr_pos, "Params", &repo->config->datastore->params);
	_get_json_int_value(data, tokens, num_tokens, curr_pos, "NoSync", &repo->config->datastore->no_sync);
	_get_json_int_value(data, tokens, num_tokens, curr_pos, "HashOnRead", &repo->config->datastore->hash_on_read);
	_get_json_int_value(data, tokens, num_tokens, curr_pos, "BloomFilterSize", &repo->config->datastore->bloom_filter_size);

	// get addresses. First is Swarm array, then Api, then Gateway
	curr_pos = _find_token(data, tokens, num_tokens, curr_pos, "Addresses");
	if (curr_pos < 0) {
		free(data);
		return 0;
	}
	// get swarm addresses
	int swarm_pos = _find_token(data, tokens, num_tokens, curr_pos, "Swarm") + 1;
	if (tokens[swarm_pos].type != JSMN_ARRAY)
		return 0;
	int swarm_size = tokens[swarm_pos].size;
	swarm_pos++;
	repo->config->addresses->swarm_head = NULL;
	struct Libp2pLinkedList* last = NULL;
	for(int i = 0; i < swarm_size; i++) {
		struct Libp2pLinkedList* current = libp2p_utils_linked_list_new();
		if (!_get_json_string_value(data, tokens, num_tokens, swarm_pos + i, NULL, (char**)&current->item))
			break;
		if (repo->config->addresses->swarm_head == NULL) {
			repo->config->addresses->swarm_head = current;
		} else {
			last->next = current;
		}
		last = current;
	}
	_get_json_string_value(data, tokens, num_tokens, curr_pos, "API", &repo->config->addresses->api);
	_get_json_string_value(data, tokens, num_tokens, curr_pos, "Gateway", &repo->config->addresses->gateway);

	// bootstrap peers
	swarm_pos = _find_token(data, tokens, num_tokens, curr_pos, "Bootstrap");
	if (swarm_pos >= 0) {
		swarm_pos++;
		if (tokens[swarm_pos].type != JSMN_ARRAY) {
			free(data);
			return 0;
		}
		swarm_size = tokens[swarm_pos].size;
		repo->config->bootstrap_peers = libp2p_utils_vector_new(swarm_size);
		swarm_pos++;
		for(int i = 0; i < swarm_size; i++) {
			char* val = NULL;
			if (!_get_json_string_value(data, tokens, num_tokens, swarm_pos + i, NULL, &val))
				break;
			struct MultiAddress* cur = multiaddress_new_from_string(val);
			if (cur == NULL)
				continue;
			libp2p_utils_vector_add(repo->config->bootstrap_peers, cur);
			free(val);
		}
	}
	// free the memory used reading the json file
	free(data);
	free(priv_key_base64);
	return 1;
}

/***
 * set function pointers in the datastore struct to lmdb
 * @param repo contains the information
 * @returns true(1) on success
 */
int fs_repo_setup_lmdb_datastore(struct FSRepo* repo) {
	return repo_fsrepo_lmdb_cast(repo->config->datastore);
}

/***
 * opens the repo's datastore, and puts a reference to it in the FSRepo struct
 * @param repo the FSRepo struct
 * @returns 0 on failure, otherwise 1
 */
int fs_repo_open_datastore(struct FSRepo* repo) {
	int argc = 0;
	char** argv = NULL;

	if (strncmp(repo->config->datastore->type, "lmdb", 4) == 0) {
		// this is a LightningDB. Open it.
		int retVal = fs_repo_setup_lmdb_datastore(repo);
		if (retVal == 0)
			return 0;
	} else {
		// add new datastore types here
		return 0;
	}

	int retVal = repo->config->datastore->datastore_open(argc, argv, repo->config->datastore);

	// do specific datastore cleanup here if needed

	return retVal;
}

/**
 * For interface of Filestore. Retrieves a node from the filestore
 * @param hash the hash to pull
 * @param hash_length the length of the hash
 * @param node_obj where to put the results
 * @param filestore a reference to the filestore struct
 * @returns true(1) on success, false(0) otherwise
 */
int ipfs_repo_fsrepo_node_get(const unsigned char* hash, size_t hash_length, void** node_obj, size_t *node_size, const struct Filestore* filestore) {
	struct FSRepo* fs_repo = (struct FSRepo*)filestore->handle;
	struct HashtableNode* node = NULL;
	int retVal = ipfs_repo_fsrepo_node_read(hash, hash_length, &node, fs_repo);
	if (retVal == 1) {
		*node_size = ipfs_hashtable_node_protobuf_encode_size(node);
		*node_obj = malloc(*node_size);
		retVal = ipfs_hashtable_node_protobuf_encode(node, *node_obj, *node_size, node_size);
	}
	if (node != NULL)
		ipfs_hashtable_node_free(node);
	return retVal;
}

/**
 * public methods
 */

/**
 * opens a fsrepo
 * @param repo the repo struct. Should contain the path. This method will do the rest
 * @return 0 if there was a problem, otherwise 1
 */
int ipfs_repo_fsrepo_open(struct FSRepo* repo) {
	//TODO: lock
	// check if initialized
	if (!repo_check_initialized(repo->path)) {
		return 0;
	}
	//TODO: lock the file (remember to unlock)
	//TODO: check the version, and make sure it is correct
	//TODO: make sure the directory is writable
	//TODO: open the config
	if (!fs_repo_open_config(repo)) {
		return 0;
	}

	// open the datastore
	if (!fs_repo_open_datastore(repo)) {
		return 0;
	}
	
	// init the filestore
	repo->config->filestore->handle = repo;
	repo->config->filestore->node_get = ipfs_repo_fsrepo_node_get;

	return 1;
}

/***
 * checks to see if the repo is initialized
 * @param repo_path the path to the repo
 * @returns true(1) if it is initialized, otherwise false(0)
 */
int fs_repo_is_initialized(char* repo_path) {
	//TODO: lock things up so that someone doesn't try an init or remove while this call is in progress
	// don't forget to unlock
	return fs_repo_is_initialized_unsynced(repo_path);
}

int ipfs_repo_fsrepo_datastore_init(struct FSRepo* fs_repo) {
	// make the directory
	if (repo_fsrepo_lmdb_create_directory(fs_repo->config->datastore) == 0)
		return 0;

	// fill in the function prototypes
	return repo_fsrepo_lmdb_cast(fs_repo->config->datastore);
}

int ipfs_repo_fsrepo_blockstore_init(const struct FSRepo* fs_repo) {
	size_t full_path_size = strlen(fs_repo->path) + 15;
	char full_path[full_path_size];
	int retVal = os_utils_filepath_join(fs_repo->path, "blockstore", full_path, full_path_size);
	if (retVal == 0)
		return 0;

#ifdef __MINGW32__
	if (mkdir(full_path) != 0)
#else
	if (mkdir(full_path, S_IRWXU) != 0)
#endif
		return 0;
	return 1;
}

/**
 * Initializes a new FSRepo at the given path with the provided config
 * @param path the path to use
 * @param config the information for the config file
 * @returns true(1) on success
 */
int ipfs_repo_fsrepo_init(struct FSRepo* repo) {
	// TODO: Do a lock so 2 don't do this at the same time
	
	// return error if this has already been done
	if (fs_repo_is_initialized_unsynced(repo->path))
		return 0;
	
	int retVal = fs_repo_write_config_file(repo->path, repo->config);
	if (retVal == 0)
		return 0;
	
	retVal = ipfs_repo_fsrepo_datastore_init(repo);
	if (retVal == 0)
		return 0;
	
	retVal = ipfs_repo_fsrepo_blockstore_init(repo);
	if (retVal == 0)
		return 0;

	// write the version to a file for migrations (see repo/fsrepo/migrations/mfsr.go)
	//TODO: mfsr.RepoPath(repo_path).WriteVersion(RepoVersion)
	return 1;
}

/**
 * write the config file to disk
 * @param path the path to the file
 * @param config the config structure
 * @returns true(1) on success
 */
int fs_repo_write_config_file(char* path, struct RepoConfig* config) {
	if (fs_repo_is_initialized(path))
		return 0;
	
	char* buff = NULL;
	if (!repo_config_get_file_name(path, &buff))
		return 0;
	
	int retVal = repo_config_write_config_file(buff, config);
	
	free(buff);
	
	return retVal;
}

/***
 * Write a block to the datastore and blockstore
 * @param block the block to write
 * @param fs_repo the repo to write to
 * @returns true(1) on success
 */
int ipfs_repo_fsrepo_block_write(struct Block* block, const struct FSRepo* fs_repo) {
	/**
	 * What is put in the blockstore is the block.
	 * What is put in the datastore is the multihash (the Cid) as the key,
	 * and the base32 encoded multihash as the value.
	 */
	int retVal = 1;
	retVal = ipfs_blockstore_put(block, fs_repo);
	if (retVal == 0)
		return 0;
	// take the cid, base32 it, and send both to the datastore
	size_t fs_key_length = 100;
	unsigned char fs_key[fs_key_length];
	retVal = ipfs_datastore_helper_ds_key_from_binary(block->cid->hash, block->cid->hash_length, fs_key, fs_key_length, &fs_key_length);
	if (retVal == 0)
		return 0;
	retVal = fs_repo->config->datastore->datastore_put(block->cid->hash, block->cid->hash_length, fs_key, fs_key_length, fs_repo->config->datastore);
	if (retVal == 0)
		return 0;
	return 1;
}

/***
 * Write a unixfs to the datastore and blockstore
 * @param unix_fs the struct to write
 * @param fs_repo the repo to write to
 * @param bytes_written number of bytes written to the repo
 * @returns true(1) on success
 */
int ipfs_repo_fsrepo_unixfs_write(const struct UnixFS* unix_fs, const struct FSRepo* fs_repo, size_t* bytes_written) {
	/**
	 * What is put in the blockstore is the block.
	 * What is put in the datastore is the multihash (the Cid) as the key,
	 * and the base32 encoded multihash as the value.
	 */
	int retVal = 1;
	retVal = ipfs_blockstore_put_unixfs(unix_fs, fs_repo, bytes_written);
	if (retVal == 0)
		return 0;
	// take the hash, base32 it, and send both to the datastore
	size_t fs_key_length = 100;
	unsigned char fs_key[fs_key_length];
	retVal = ipfs_datastore_helper_ds_key_from_binary(unix_fs->hash, unix_fs->hash_length, fs_key, fs_key_length, &fs_key_length);
	if (retVal == 0)
		return 0;
	retVal = fs_repo->config->datastore->datastore_put(unix_fs->hash, unix_fs->hash_length, fs_key, fs_key_length, fs_repo->config->datastore);
	if (retVal == 0)
		return 0;
	return 1;
}

/***
 * Write a unixfs to the datastore and blockstore
 * @param unix_fs the struct to write
 * @param fs_repo the repo to write to
 * @param bytes_written number of bytes written to the repo
 * @returns true(1) on success
 */
int ipfs_repo_fsrepo_node_write(const struct HashtableNode* node, const struct FSRepo* fs_repo, size_t* bytes_written) {
	/**
	 * What is put in the blockstore is the node.
	 * What is put in the datastore is the multihash as the key,
	 * and the base32 encoded multihash as the value.
	 */
	int retVal = 1;
	retVal = ipfs_blockstore_put_node(node, fs_repo, bytes_written);
	if (retVal == 0)
		return 0;
	// take the hash, base32 it, and send both to the datastore
	size_t fs_key_length = 100;
	unsigned char fs_key[fs_key_length];
	retVal = ipfs_datastore_helper_ds_key_from_binary(node->hash, node->hash_size, fs_key, fs_key_length, &fs_key_length);
	if (retVal == 0)
		return 0;
	retVal = fs_repo->config->datastore->datastore_put(node->hash, node->hash_size, fs_key, fs_key_length, fs_repo->config->datastore);
	if (retVal == 0)
		return 0;
	return 1;
}

int ipfs_repo_fsrepo_node_read(const unsigned char* hash, size_t hash_length, struct HashtableNode** node, const struct FSRepo* fs_repo) {
	int retVal = 0;

	// get the base32 hash from the database
	// We do this only to see if it is in the database
	size_t fs_key_length = 100;
	unsigned char fs_key[fs_key_length];
	retVal = fs_repo->config->datastore->datastore_get((const char*)hash, hash_length, fs_key, fs_key_length, &fs_key_length, fs_repo->config->datastore);
	if (retVal == 0) // maybe it doesn't exist?
		return 0;
	// now get the block from the blockstore
	retVal = ipfs_blockstore_get_node(hash, hash_length, node, fs_repo);
	return retVal;
}



int ipfs_repo_fsrepo_block_read(const unsigned char* hash, size_t hash_length, struct Block** block, const struct FSRepo* fs_repo) {
	int retVal = 0;

	// get the base32 hash from the database
	// We do this only to see if it is in the database
	size_t fs_key_length = 100;
	unsigned char fs_key[fs_key_length];
	retVal = fs_repo->config->datastore->datastore_get((const char*)hash, hash_length, fs_key, fs_key_length, &fs_key_length, fs_repo->config->datastore);
	if (retVal == 0) // maybe it doesn't exist?
		return 0;
	// now get the block from the blockstore
	retVal = ipfs_blockstore_get(hash, hash_length, block, fs_repo);
	return retVal;
}

int ipfs_repo_fsrepo_unixfs_read(const unsigned char* hash, size_t hash_length, struct UnixFS** unix_fs, const struct FSRepo* fs_repo) {
	int retVal = 0;

	// get the base32 hash from the database
	// We do this only to see if it is in the database
	size_t fs_key_length = 100;
	unsigned char fs_key[fs_key_length];
	retVal = fs_repo->config->datastore->datastore_get((const char*)hash, hash_length, fs_key, fs_key_length, &fs_key_length, fs_repo->config->datastore);
	if (retVal == 0) // maybe it doesn't exist?
		return 0;
	// now get the block from the blockstore
	retVal = ipfs_blockstore_get_unixfs(hash, hash_length, unix_fs, fs_repo);
	return retVal;
}

