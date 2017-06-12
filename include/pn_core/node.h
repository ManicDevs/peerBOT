#ifndef _PN_CORE_NODE_H_
#define _PN_CORE_NODE_H_

#include "pn_core/config/identity.h"
#include "pn_routing/routing.h"
#include "libp2p/peer/peerstore.h"
#include "libp2p/peer/providerstore.h"

enum PNNodeStatus { STATUS_OFFLINE, STATUS_ONLINE };

struct PNNode
{
    enum PNNodeStatus status;
    struct PNConfig *config;
    struct PNRouting *routing;
    struct PNIdentity *identity;
    struct Peerstore *peerstore;
    struct ProviderStore *providerstore;
};

int core_node_init(struct PNNode **node, struct PNConfig *config);

int core_node_new(struct PNNode **node);

int core_node_free(struct PNNode *node);

#endif
