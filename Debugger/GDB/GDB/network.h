/* Marcus Botacin
 * UNICAMP - 2017
 * GDB Stub implementation
 * BranchMonitoring Project
 */

#pragma once

#include "network.h"
/* Avoid multiple Socks includes */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

/* C/Windows headers */

#include<assert.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include <windows.h>
#include<shellapi.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include<TlHelp32.h>
#include<Psapi.h>

/* Libraries includes */

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Kernel32.lib")
#pragma comment(lib, "Psapi.lib")


/* Open the socket and receives the connection request
 * This function is the major change from the Linux version
 */
SOCKET get_connection();

/* constants */

#define SOCKET_ERROR 0
#define REQUEST_MAX_SIZE 100
#define REPLY_SIZE 10
#define MAX_REPLY_SIZE 1000
#define MAX_MEM 10