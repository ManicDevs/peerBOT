#pragma once

#include <pthread.h>

#include "ipfs/core/daemon.h"
#include "../test_helper.h"
#include "libp2p/utils/logger.h"

void* test_daemon_start(void* arg) {
	ipfs_daemon_start((char*)arg);
	return NULL;
}

int test_daemon_startup_shutdown() {
	int retVal = 0;
	pthread_t daemon_thread;
	char* ipfs_path = "/tmp/.ipfs";
	char* peer_id = NULL;

	if (!drop_and_build_repository(ipfs_path, 4001, NULL, &peer_id)) {
		fprintf(stderr, "Unable to drop and build repository at %s\n", ipfs_path);
		goto exit;
	}

	pthread_create(&daemon_thread, NULL, test_daemon_start, (void*)ipfs_path);

	ipfs_daemon_stop();

	pthread_join(daemon_thread, NULL);

	retVal = 1;
	exit:
	if (peer_id != NULL)
		free(peer_id);

	return retVal;
}
