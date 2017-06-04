#ifndef peer_h
#define peer_h


/***
 * given a public key, find the peer id
 * @param public_key the public key in bytes
 * @param id the resultant peer id
 * @returns true(1) on success
 */
int repo_config_peer_id_from_public_key(char* public_key, char* id);

#endif /* peer_h */
