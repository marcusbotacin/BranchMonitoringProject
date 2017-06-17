/* BranchMonitoring Project 
 * Debugger Server-side
 * Marcus Botacin
 * UNICAMP - 2017
 */

/* Avoid multiple includes */
#pragma once

/* Avoid Include Conflicts */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

/* define defaults *
#define SOCK_DEFAULT_PORT "5000"

/* define socket flags */
#define SOCKET_FLAGS 0

/* define sleep time when required */
#define SLEEP_TIME 1000

/* define socket options */
#define SUCCESS 0

/* Sizes and offsets Definitions */
#define SAMPLE_SIZE 10000
#define START_OFFSET 0
#define MINIMUM_BYTES 0
#define START_BYTES 0
#define INCREMENT_POSITION 1

/* Connection Functions */
int receive_sample(int Socket,void *file_in_memory);
SOCKET bind_connection(char *port);