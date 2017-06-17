/* Branch Monitoring Project
 * Debugger Client
 * Marcus Botacin
 * UNICAMP - 2017
 */


/* Avoiding Windows.h and winsocks.h conflicts */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include "includes.h"
#include "network.h"

#include<shellapi.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#pragma comment(lib, "Ws2_32.lib")
SOCKET get_connection(char *addr, char *port)
{
	
	int err;
	int wVersionRequested;


	wVersionRequested = MAKEWORD(2, 2);

	WSAData wsaData;
	err = WSAStartup(wVersionRequested, &wsaData);


		struct addrinfo *result = NULL,
                *ptr = NULL,
                hints;

ZeroMemory( &hints, sizeof(hints) );
hints.ai_family = AF_UNSPEC;
hints.ai_socktype = SOCK_STREAM;
hints.ai_protocol = IPPROTO_TCP;

int iResult;

// Resolve the server address and port
iResult = getaddrinfo(addr, port, &hints, &result);
if (iResult != 0) {
	printf("ADDR %s\n",addr);
    printf("getaddrinfo failed: %d\n", iResult);
    WSACleanup();
    return 1;
}

SOCKET ConnectSocket = INVALID_SOCKET;

ptr=result;

// Create a SOCKET for connecting to server
ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
    ptr->ai_protocol);


if (ConnectSocket == INVALID_SOCKET) {
    printf("Error at socket(): %ld\n", WSAGetLastError());
    freeaddrinfo(result);
    WSACleanup();
    return 1;
}

iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
if (iResult == SOCKET_ERROR) {
    closesocket(ConnectSocket);
    ConnectSocket = INVALID_SOCKET;
}

	freeaddrinfo(result);

if (ConnectSocket == INVALID_SOCKET) {
    printf("Unable to connect to server!\n");
    WSACleanup();
    return INVALID_SOCKET;
}

	return ConnectSocket;
}