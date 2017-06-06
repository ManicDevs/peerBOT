#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "libp2p/net/p2pnet.h"
#include "libp2p/record/message.h"
#include "libp2p/net/multistream.h"
#include "libp2p/utils/logger.h"
#include "ipfs/core/daemon.h"
#include "ipfs/routing/routing.h"
#include "ipfs/core/ipfs_node.h"
#include "libp2p/secio/secio.h"
#include "libp2p/nodeio/nodeio.h"
#include "libp2p/routing/dht_protocol.h"
#include "ipfs/merkledag/merkledag.h"
#include "ipfs/merkledag/node.h"
#include "ipfs/util/thread_pool.h"

#define BUF_SIZE 4096

// this should be set to 5 for normal operation, perhaps higher for debugging purposes
#define DEFAULT_NETWORK_TIMEOUT 5

static int null_shutting_down = 0;

/***
 * Compare incoming to see if they are requesting a protocol upgrade
 * @param incoming the incoming string
 * @param incoming_size the size of the incoming string
 * @param test the protocol string to compare it with (i.e. "/secio" or "/nodeio"
 * @returns true(1) if there was a match, false(0) otherwise
 */
int protocol_compare(unsigned char *incoming, size_t incoming_size, const char *test)
{
    int test_size = strlen(test);

    if(incoming_size >= test_size && strncmp((char*)incoming, test, test_size) == 0)
        return 1;

    return 0;
}

/**
 * We've received a connection. Find out what they want
 */
void ipfs_null_connection(void *ptr)
{
    struct null_connection_params *connection_param = NULL;

    connection_param = (struct null_connection_params*) ptr;

    // TODO: when should we exit the for loop and disconnect?

    struct SessionContext session;
    session.insecure_stream = libp2p_net_multistream_stream_new(connection_param->file_descriptor, connection_param->ip, connection_param->port);
    session.default_stream = session.insecure_stream;
    session.datastore = connection_param->local_node->repo->config->datastore;
    session.filestore = connection_param->local_node->repo->config->filestore;

    libp2p_logger_log("null", LOGLEVEL_INFO, "Connection %d, count %d\n", connection_param->file_descriptor, *(connection_param->count));

    if(libp2p_net_multistream_negotiate(session.insecure_stream)) {

        for(;;)
        {
            // check if they're looking for an upgrade (i.e. secio)
            unsigned char *results = NULL;
            size_t bytes_read = 0;

            if(!session.default_stream->read(&session, &results, &bytes_read, DEFAULT_NETWORK_TIMEOUT))
            {
                libp2p_logger_debug("null", "stream transaction read returned false\n");
                break;
            }

            if(null_shutting_down)
                break;

            if(bytes_read == 0) // timeout
                continue;

            libp2p_logger_debug("null", "Read %lu bytes from a stream tranaction\n", bytes_read);
            if(protocol_compare(results, bytes_read, "/secio"))
            {
                libp2p_logger_debug("null", "Attempting secure io connection...\n");
                if(!libp2p_secio_handshake(&session, &connection_param->local_node->identity->private_key, 1))
                {
                    // rejecting connection
                    libp2p_logger_debug("null", "Secure IO connection failed\n");
                    free(results);
                    break;
                }
            }
            else if(protocol_compare(results, bytes_read, "/nodeio"))
            {
                libp2p_logger_debug("null", "Attempting a nodeio connection.\n");
                if(!libp2p_nodeio_handshake(&session))
                {
                    free(results);
                    break;
                }

                // loop through file requests
                int _continue = 1;
                while(_continue)
                {
                    size_t hash_length = 0;
                    unsigned char *hash;

                    _continue = session.default_stream->read(&session, &hash, &hash_length, DEFAULT_NETWORK_TIMEOUT);
                    if(hash_length < 20)
                    {
                        _continue = 0;
                        continue;
                    }
                    else
                    {
                        // try to get the Node
                        struct HashtableNode *node = NULL;

                        if(!ipfs_merkledag_get(hash, hash_length, &node, connection_param->local_node->repo))
                        {
                            _continue = 0;
                            continue;
                        }

                        size_t results_size = ipfs_hashtable_node_protobuf_encode_size(node);
                        unsigned char results[results_size];

                        if (!ipfs_hashtable_node_protobuf_encode(node, results, results_size, &results_size))
                        {
                            _continue = 0;
                            continue;
                        }

                        // send it to the requestor
                        session.default_stream->write(&session, results, results_size);
                    }
                }
            }
            else if(protocol_compare(results, bytes_read, "/ipfs/kad/"))
            {
                libp2p_logger_log("null", LOGLEVEL_DEBUG, "Attempting kademlia connection...\n");
                if(!libp2p_routing_dht_handshake(&session))
                {
                    libp2p_logger_log("null", LOGLEVEL_DEBUG, "kademlia connection handshake failed\n");
                    free(results);
                    break;
                }

                // this handles 1 transaction
                libp2p_routing_dht_handle_message(&session, connection_param->local_node->peerstore, connection_param->local_node->providerstore);
                libp2p_logger_log("null", LOGLEVEL_DEBUG, "kademlia message handled\n");
            }
            else
            {
                libp2p_logger_error("null", "There was a problem with this connection. It is nothing I can handle. Disconnecting.\n");
                break;
            }
            free(results);
        }
    }
    else
        libp2p_logger_log("null", LOGLEVEL_DEBUG, "Multistream negotiation failed\n");

    if(session.default_stream != NULL)
        session.default_stream->close(&session);


    if(session.insecure_stream != NULL)
        libp2p_net_multistream_stream_free(session.insecure_stream);

    (*(connection_param->count))--; // update counter.
    if(connection_param->ip != NULL)
        free(connection_param->ip);

    free(connection_param);

    return;
}

void *ipfs_null_listen(void *ptr)
{
    int socketfd, s, count = 0;
    threadpool thpool = thpool_init(25);

    struct IpfsNodeListenParams *listen_param;
    struct null_connection_params *connection_param;

    listen_param = (struct IpfsNodeListenParams*)ptr;

    if((socketfd = socket_listen(socket_tcp4(), &(listen_param->ipv4), &(listen_param->port))) <= 0)
    {
        libp2p_logger_error("null", "Failed to init null router. Address: %d, Port: %d\n", listen_param->ipv4, listen_param->port);
        return (void*)2;
    }

    libp2p_logger_error("null", "Ipfs listening on %d\n", listen_param->port);

    for(;;)
    {
        int numDescriptors = socket_read_select4(socketfd, 2);

        if (null_shutting_down)
            break;

        if(numDescriptors > 0)
        {
            s = socket_accept4(socketfd, &(listen_param->ipv4), &(listen_param->port));
            if(count >= CONNECTIONS)
            { // limit reached.
                close (s);
                continue;
            }

            count++;
            connection_param = malloc(sizeof(struct null_connection_params));
            if(connection_param)
            {
                connection_param->file_descriptor = s;
                connection_param->count = &count;
                connection_param->local_node = listen_param->local_node;
                connection_param->port = listen_param->port;
                connection_param->ip = malloc(INET_ADDRSTRLEN);

                if(inet_ntop(AF_INET, &(listen_param->ipv4), connection_param->ip, INET_ADDRSTRLEN) == NULL)
                {
                    free(connection_param->ip);
                    connection_param->ip = NULL;
                    connection_param->port = 0;
                }

                // Create pthread for ipfs_null_connection.
                thpool_add_work(thpool, ipfs_null_connection, connection_param);
            }
        }
    }

    thpool_destroy(thpool);

    close(socketfd);

    return (void*)2;
}

int ipfs_null_shutdown()
{
    null_shutting_down = 1;

    return 1;
}
