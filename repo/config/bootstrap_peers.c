#include <stdio.h>
#include <stdlib.h>

#include "ipfs/repo/config/bootstrap_peers.h"
#include "multiaddr/multiaddr.h"

int repo_config_bootstrap_peers_retrieve(struct Libp2pVector** list) {

	char* default_bootstrap_addresses[] = {
		"/ip4/127.0.0.1/tcp/4001/ipfs/QmaCpDMGvV2BGHeYERUEnRQAwe3N8SzbUtfsmvsqQLuvuJ",
	};
	*list = libp2p_utils_vector_new(1);


	for(int i = 0; i < 9; i++) {
		struct MultiAddress* currAddr = multiaddress_new_from_string(default_bootstrap_addresses[i]);
		libp2p_utils_vector_add(*list, currAddr);
	}

	return 1;
}

int repo_config_bootstrap_peers_free(struct Libp2pVector* list) {
	if (list != NULL) {
		for(int i = 0; i < list->total; i++) {
			struct MultiAddress* currAddr = libp2p_utils_vector_get(list, i);
			multiaddress_free(currAddr);
		}
		libp2p_utils_vector_free(list);
	}
	return 1;
}
