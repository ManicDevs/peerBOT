#ifndef test_repo_config_h
#define test_repo_config_h
#include <sys/stat.h>
#include "ipfs/repo/config/config.h"
#include "ipfs/repo/fsrepo/fs_repo.h"
#include "libp2p/os/utils.h"

int test_repo_config_new() {
	struct RepoConfig* repoConfig;
	int retVal = ipfs_repo_config_new(&repoConfig);
	if (retVal == 0)
		return 0;

	retVal = ipfs_repo_config_free(repoConfig);
	if (retVal == 0)
		return 0;

	return 1;
}

int test_repo_config_init() {
	int retVal = 0;
	struct RepoConfig* repoConfig = NULL;
	char* config_dir = "/tmp/.ipfs";
	char* peer_id = NULL;

	if (!drop_repository(config_dir)) {
		fprintf(stderr, "Unable to delete repository\n");
		goto exit;
	}

	if (!ipfs_repo_config_new(&repoConfig)) {
		fprintf(stderr, "Unable to initialize repo structure\n");
		goto exit;
	}

	if (!ipfs_repo_config_init(repoConfig, 2048, config_dir, 4001, NULL)) {
		fprintf(stderr, "unable to initialize new repo\n");
		goto exit;
	}
	
	// now tear it apart to check for anything broken

	// addresses
	if (repoConfig->addresses == NULL) {
		fprintf(stderr, "Addresses is null\n");
		goto exit;
	}

	/* API not implemented yet
	if (repoConfig->addresses->api == NULL) {
		fprintf(stderr, "Addresses->API is null\n");
		goto exit;
	}

	if (strncmp(repoConfig->addresses->api, "/ip4/127.0.0.1/tcp/5001", 23) != 0)
		goto exit;
	*/

	/* Gateway not implemented yete
	if (repoConfig->addresses->gateway == NULL)
		goto exit;

	if (strncmp(repoConfig->addresses->gateway, "/ip4/127.0.0.1/tcp/8080", 23) != 0)
		goto exit;
	*/
	
	/* No swarms added yet
	if (repoConfig->addresses->swarm_head == NULL
			|| repoConfig->addresses->swarm_head->next == NULL
			|| repoConfig->addresses->swarm_head->next->next != NULL) {
		goto exit;
	}
	
	if (strcmp((char*)repoConfig->addresses->swarm_head->item, "/ip4/0.0.0.0/tcp/4001") != 0)
		goto exit;
	
	if (strcmp((char*)repoConfig->addresses->swarm_head->next->item, "/ip6/::/tcp/4001") != 0)
		goto exit;
	*/
	
	// datastore
	if (strncmp(repoConfig->datastore->path, "/tmp/.ipfs/datastore", 32) != 0)
		goto exit;

	retVal = 1;
	exit:
	if (repoConfig != NULL)
		ipfs_repo_config_free(repoConfig);
	if (peer_id != NULL)
		free(peer_id);
	
	return retVal;
}

/***
 * test the writing of the config file
 */
int test_repo_config_write() {
	// make sure the directory is there
	if (!os_utils_file_exists("/tmp/.ipfs")) {
		mkdir("/tmp/.ipfs", S_IRWXU);
	}
	// first delete the existing one
	unlink("/tmp/.ipfs/config");
	
	// now build a new one
	struct RepoConfig* repoConfig;
	ipfs_repo_config_new(&repoConfig);
	if (!ipfs_repo_config_init(repoConfig, 2048, "/tmp/.ipfs", 4001, NULL)) {
		ipfs_repo_config_free(repoConfig);
		return 0;
	}
	
	if (!fs_repo_write_config_file("/tmp/.ipfs", repoConfig)) {
		ipfs_repo_config_free(repoConfig);
		return 0;
	}

	ipfs_repo_config_free(repoConfig);
	
	// check to see if the file exists
	return os_utils_file_exists("/tmp/.ipfs/config");
}

#endif /* test_repo_config_h */
