/* Marcus Botacin
 * UNICAMP - 2017
 * GDB Stub implementation
 * BranchMonitoring Project
 */

/* Include Block */

/* Own Headers */

/* Avoid multiple Socks includes */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "examples.h"
#include "GDB.h"
#include "network.h"

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


/* Global vars
 * TODO: Use pointers instead
 */
struct state g_state;
SOCKET ClientSocket;


int main(int argc,char *argv[])
{
	int signum=5;
	SOCKET connection;
	struct gdb_regs context;
	printf("Starting GDB Server\n");

	/* Init GDB with data */
	fill_Data(&context);
	
	/* Open Socket and get connection */
	if(connection=get_connection()>SOCKET_ERROR)
	{
		char request[REQUEST_MAX_SIZE];
		while(1)
		{
			get(request,sizeof(request));
			printf("REQUEST %c\n",request[0]);
			switch(request[0])
			{
				case '?':
				{
					printf("Currently Selected Option: ?\n");
					char reply[REPLY_SIZE];
					_snprintf(reply, sizeof(reply), "S%02x", signum);
					put_packet(g_state.fp, reply);
				}
				break;
				case 'g':
					{
						printf("Currently Selected Option: g\n");
						char reply[MAX_REPLY_SIZE];
						struct gdb_regs regs;
						copy_regs_to_gdb(&regs, &context);
						write_hex_bytes(reply, (UINT8 *) &regs, sizeof(regs));
						printf("Reply Sent: %s\n",reply);
						reply[sizeof(regs) * 2] = 0;
						put_packet(g_state.fp, reply);
						put_packet(NULL,"");
					}
					break;
				case 'm':
					{
						printf("Currently Selected Option: m\n");
						char mem_addr[MAX_MEM]={0};
						int mem_size=MAX_MEM;
						char reply[MAX_REPLY_SIZE];
						write_hex_bytes(reply, (UINT8 *) mem_addr, mem_size);
						reply[mem_size * 2] = 0;
						put_packet(g_state.fp, reply);
					}
					break;
					case 'c':
					{
						printf("Currently Selected Option: c\n");
					}
					break;
					case 's':
					{
						printf("Currently Selected Option: s\n");
					}
					break;
					default:
					{
						printf("Currently Selected Option: default\n");
						put_packet(NULL,"");
					}
					break;
					
				 
			}
		}
	}else{
		printf("connect error\n");
	}
	return 0;
}
