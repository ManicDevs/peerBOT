#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>

#include "libp2p/net/p2pnet.h"
#include "libp2p/peer/peerstore.h"
#include "ipfs/core/daemon.h"
#include "ipfs/core/ipfs_node.h"
#include "ipfs/core/bootstrap.h"
#include "ipfs/repo/fsrepo/fs_repo.h"
#include "ipfs/repo/init.h"
#include "libp2p/utils/logger.h"

int ipfs_daemon_start(char *repo_path)
{
    int count_pths = 0, retVal = 0;
    pthread_t work_pths[MAX];

    struct IpfsNodeListenParams listen_param;
    struct MultiAddress *ma = NULL;
    struct IpfsNode *local_node = NULL;

    libp2p_logger_info("daemon", "Initializing daemon...\n");

    if(!ipfs_node_online_new(repo_path, &local_node))
        goto exit;

    // Set null router param
    ma = multiaddress_new_from_string(local_node->repo->config->addresses->swarm_head->item);
    listen_param.port = multiaddress_get_ip_port(ma);
    listen_param.ipv4 = 0; // ip 0.0.0.0, all interfaces
    listen_param.local_node = local_node;

    // Create pthread for swarm listener.
    if(pthread_create(&work_pths[count_pths++], NULL, ipfs_null_listen, &listen_param))
    {
        libp2p_logger_error("daemon", "Error creating thread for ipfs null listen\n");
        goto exit;
    }

    //ipfs_bootstrap_routing(local_node);

    libp2p_logger_info("daemon", "Daemon for %s is ready on port %d\n", listen_param.local_node->identity->peer_id, listen_param.port);

    // Wait for pthreads to finish.
    while(count_pths)
    {
        if(pthread_join(work_pths[--count_pths], NULL))
        {
            libp2p_logger_error("daemon", "Error joining thread\n");
            goto exit;
        }
    }

    retVal = 1;

exit:
    libp2p_logger_debug("daemon", "Cleaning up daemon processes\n");

    // clean up
    if(ma != NULL)
        multiaddress_free(ma);

    if(local_node != NULL)
        ipfs_node_free(local_node);

    return retVal;
}

int ipfs_daemon_stop()
{
    return ipfs_null_shutdown();
}

int ipfs_daemon(int argc, char **argv)
{
    char *repo_path = NULL;

    if(!ipfs_repo_get_directory(argc, argv, &repo_path))
    {
        fprintf(stderr, "Unable to open repo: %s\n", repo_path);
        return 0;
    }

    return ipfs_daemon_start(repo_path);
}
