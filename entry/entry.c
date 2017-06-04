#include <stdio.h>
#include <string.h>

#include "ipfs/repo/init.h"
#include "ipfs/importer/importer.h"
#include "ipfs/importer/exporter.h"
#include "ipfs/core/daemon.h"

#define INIT 1
#define ADD 2
#define OBJECT_GET 3
#define DNS 4
#define CAT 5
#define DAEMON 6
#define PING 7
#define GET 8

void *run_daemon(void *_argv)
{
    int argc = 3;
    char **argv = (char**)_argv;
    ipfs_daemon(argc, argv);

    return NULL;
}

/***
 * The beginning
 */
int main(int argc, char** argv)
{
    ipfs_repo_init(argc, argv);

    ipfs_ping(argc, argv);

    run_daemon(argv);

    return 0;

/*
	switch (retVal) {
	case (INIT):
		return ipfs_repo_init(argc, argv);
		break;
	case (ADD):
		ipfs_import_files(argc, argv);
		break;
	case (OBJECT_GET):
		ipfs_exporter_object_get(argc, argv);
		break;
	case(GET):
		//ipfs_exporter_get(argc, argv);
		//break;
	case (CAT):
		ipfs_exporter_object_cat(argc, argv);
		break;
	case (DNS):
		ipfs_dns(argc, argv);
		break;
	case (DAEMON):
		ipfs_daemon(argc, argv);
		break;
	case (PING):
		ipfs_ping(argc, argv);
		break;
    }
*/
}
