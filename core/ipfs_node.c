#include <stdlib.h>

#include "ipfs/core/ipfs_node.h"

/***
 * build an online IpfsNode
 * @param repo_path where the IPFS repository directory is
 * @param node the completed IpfsNode struct
 * @returns true(1) on success
 */
int ipfs_node_online_new(const char* repo_path, struct IpfsNode** node) {
	struct FSRepo* fs_repo = NULL;

	*node = (struct IpfsNode*)malloc(sizeof(struct IpfsNode));
	if(*node == NULL)
		return 0;

	struct IpfsNode* local_node = *node;
	local_node->identity = NULL;
	local_node->peerstore = NULL;
	local_node->providerstore = NULL;
	local_node->repo = NULL;
	local_node->routing = NULL;

	// build the struct
	if (!ipfs_repo_fsrepo_new(repo_path, NULL, &fs_repo)) {
		ipfs_node_free(local_node);
		*node = NULL;
		return 0;
	}
	// open the repo
	if (!ipfs_repo_fsrepo_open(fs_repo)) {
		ipfs_node_free(local_node);
		*node = NULL;
		return 0;
	}

	// fill in the node
	local_node->repo = fs_repo;
	local_node->identity = fs_repo->config->identity;
	local_node->peerstore = libp2p_peerstore_new(local_node->identity->peer_id);
	local_node->providerstore = libp2p_providerstore_new();
	local_node->mode = MODE_OFFLINE;
	local_node->routing = ipfs_routing_new_online(local_node, &fs_repo->config->identity->private_key, NULL);

	return 1;
}

/***
 * Free resources from the creation of an IpfsNode
 * @param node the node to free
 * @returns true(1)
 */
int ipfs_node_free(struct IpfsNode* node) {
	if (node != NULL) {
		if (node->providerstore != NULL)
			libp2p_providerstore_free(node->providerstore);
		if (node->peerstore != NULL)
			libp2p_peerstore_free(node->peerstore);
		if (node->repo != NULL)
			ipfs_repo_fsrepo_free(node->repo);
		if (node->mode == MODE_ONLINE) {
			ipfs_routing_online_free(node->routing);
		}
		free(node);
	}
	return 1;
}
