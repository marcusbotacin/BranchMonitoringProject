/* Branch Monitoring Project
 * Debugger Client
 * Marcus Botacin
 * UNICAMP - 2017
 */

/* avoid multiple includes */
#pragma once

/* MAX string size */
#define MAX_STRING 256

/* MAX memory dump */
#define MAX_MEM_DUMP 1024

/* define constants */
#define DEC 10
#define HEX 16

/* defines to ease comparison */
#define EQUAL 0
#define MASK 0xFF

/* Function Prototypess */
void get_context(SOCKET ConnectSocket);
void resume(SOCKET ConnectSocket);
void suspend(SOCKET ConnectSocket);
void introspect_from(SOCKET ConnectSocket,char *adfrom);
void introspect_to(SOCKET ConnectSocket, char *adto);
void get_regs(SOCKET ConnectSocket);
void next_branch(SOCKET ConnectSocket,char *adfrom, char *adto);
void remote_exit(SOCKET ConnectSocket,bool *exit);
void dump_mem(SOCKET ConnectSocket, char *base, char *size);
void retrieve_name(SOCKET ConnectSocket);
void loaded_libs(SOCKET ConnectSocket);
void get_process_status(SOCKET ConnectSocket, char *pid);