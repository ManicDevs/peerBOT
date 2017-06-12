#include <stdlib.h>

#include "pn_core/node.h"
#include "pn_core/config/config.h"
#include "pn_routing/routing.h"

int core_node_init(struct PNNode **node, struct PNConfig *config)
{
    if(node == NULL)
        return 0;

    struct PNNode *local_node = *node;

    // Build the struct
    local_node->status = STATUS_OFFLINE;
    local_node->identity = config->identity;
    local_node->peerstore = libp2p_peerstore_new(local_node->identity->peer_id);
    local_node->providerstore = libp2p_providerstore_new();
    //local_node->routing = routing_online_new(local_node, config->identity->private_key, NULL);

    return 1;
}

/***
 * build an online PNNode
 * @param node the completed PNNode struct
 * @returns true(1) on success
 */
int core_node_new(struct PNNode **node)
{
    *node = (struct PNNode*)malloc(sizeof(struct PNNode));
    if(*node == NULL)
        return 0;

    struct PNNode *local_node = *node;

    local_node->identity = NULL;
    local_node->peerstore = NULL;
    local_node->providerstore = NULL;
    //local_node->routing = NULL;

    return 1;
}

/***
 * Free resources from the creation of an IpfsNode
 * @param node the node to free
 * @returns true(1)
 */
int core_node_free(struct PNNode *node)
{
    if(node != NULL)
    {
        if(node->providerstore != NULL)
            libp2p_providerstore_free(node->providerstore);

        if(node->peerstore != NULL)
            libp2p_peerstore_free(node->peerstore);

        free(node);
    }

    return 1;
}
