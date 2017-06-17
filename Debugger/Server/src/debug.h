/* BranchMonitoring Project 
 * Debugger Server-side
 * Marcus Botacin
 * UNICAMP - 2017
 */

/* Avoid multiple includes */
#pragma once

/* General Includes */
#include "includes.h"
/* Data Types */
#include "data.h"

/* define base values */
# define DECIMAL 10
# define HEX 16

/* define max memory dump size */
#define MAX_DUMP 1024

#define MAX_ADDR_AS_STRING 9

/* Debugging Functions */
void next_branch(SOCKET ClientSocket,HANDLE hFile,int monitored_pid, HANDLE completionPortHandle, POVL_WRAPPER2 wrap2);
void memdump(SOCKET ClientSocket,int monitored_pid);
void register_values(SOCKET ClientSocket,HANDLE hFile);
void introspection(SOCKET ClientSocket, int monitored_pid);
void list_modules(SOCKET ClientSocket,int monitored_pid);
void getname(SOCKET ClientSocket,int monitored_pid);
void getcontext(SOCKET ClientSocket,int monitored_pid);