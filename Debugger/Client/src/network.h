/* Branch Monitoring Project
 * Debugger Client
 * Marcus Botacin
 * UNICAMP - 2017
 */

/* avoid multiple includes */
#pragma once

/* Avoiding Windows.h and winsocks.h conflicts */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include "includes.h"
#include<shellapi.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

/* Open/Get socket function */
SOCKET get_connection(char *addr, char *port);