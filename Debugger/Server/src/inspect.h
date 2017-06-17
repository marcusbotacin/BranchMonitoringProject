/* BranchMonitoring Project 
 * Debugger Server-side
 * Marcus Botacin
 * UNICAMP - 2017
 */

/* Avoid multiple includes */
#pragma once

/* General Include */
#include "includes.h"

/* MAX String Size */
#define MAX_STRING 256

/* offset file */
#define OFF_FILE "offset.txt"

/* addr offset defs */
#define ADDR_SIZE 8
#define ADDR_OFFSET 17

/* maximum number of loaded modules */
#define MAX_MODULES 1024

/* Inspection Function Definitions */
void get_name(char pname[],int monitored_pid);
void PrintModules( DWORD processID,char modules[]);
int ReadMem(int monitored_pid,UINT64 nbase,UINT64 nsize,char memdump[]);
void string_offset(char offset[],UINT64 off);
void introspect(UINT64 addr,char res[],int monitored_pid);
void get_context(char *context,int monitored_pid);
