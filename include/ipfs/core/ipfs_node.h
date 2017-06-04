#pragma once

#include "ipfs/repo/config/identity.h"
#include "ipfs/repo/fsrepo/fs_repo.h"
#include "ipfs/routing/routing.h"
#include "libp2p/peer/peerstore.h"
#include "libp2p/peer/providerstore.h"

enum NodeMode { MODE_OFFLINE, MODE_ONLINE };

struct IpfsNode {
	enum NodeMode mode;
	struct Identity* identity;
	struct FSRepo* repo;
	struct Peerstore* peerstore;
	struct ProviderStore* providerstore;
	struct IpfsRouting* routing;
	//struct Pinner pinning; // an interface
	//struct Mount** mounts;
	// TODO: Add more here
};

/***
 * build an online IpfsNode
 * @param repo_path where the IPFS repository directory is
 * @param node the completed IpfsNode struct
 * @returns true(1) on success
 */
int ipfs_node_online_new(const char* repo_path, struct IpfsNode** node);
/***
 * Free resources from the creation of an IpfsNode
 * @param node the node to free
 * @returns true(1)
 */
int ipfs_node_free(struct IpfsNode* node);
