/* BranchMonitoring Project 
 * Debugger Server-side
 * Marcus Botacin
 * UNICAMP - 2017
 */

/* avoid conflicts */

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

/* include used libs */

#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include<stdio.h>
#include "network.h"
#include "print.h"

/* load used library */
#pragma comment(lib, "Ws2_32.lib")

/* receive a sample from the network and stores into a file */
int receive_sample(int Socket,void *file_in_memory)
{
	int received=START_BYTES;
	int offset=START_OFFSET;
	/* while receiving data */
	do{
		/* receive a given portiong */
		received=recv(Socket,(char*)((int*)file_in_memory+offset),SAMPLE_SIZE,0);
		/* update received offset */
		offset+=received;
	}while(received>MINIMUM_BYTES);
	/* return total size */
	return offset+INCREMENT_POSITION;
}

/* Open a socket and bind it to a given port */
SOCKET bind_connection(char *port)
{
	WSADATA wsaData;
	int iResult;

		/* Initialize Socket - MSDN*/
		iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != SUCCESS) {
		console_message_with_code("WSAStartup failed", iResult);
		return 1;
	}

	struct addrinfo hints, *result = NULL, *ptr = NULL;

	ZeroMemory(&hints, sizeof (hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	/* Resolve the local address and port to be used by the server */
	iResult = getaddrinfo(NULL, port, &hints, &result);
	if (iResult != SUCCESS) {
		console_message_with_code("getaddrinfo failed", iResult);
		WSACleanup();
		return 1;
	}

	SOCKET ListenSocket = INVALID_SOCKET;

	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) {
		console_message_with_code("Error at socket()", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		console_message_with_code("bind failed with error", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	/* Finally an useful thing: the socket itself */
	return ListenSocket;
}