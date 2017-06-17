/* Branch Monitoring Project
 * Debugger Client
 * Marcus Botacin
 * UNICAMP - 2017
 */

/* avoid multiple includes */
#pragma once

/* General Include */
#include "includes.h"

/* Size definition */
#define SAMPLE_SIZE 10000

/* function prototypes */
char* get_sample_name(char *name);
void send_sample(int ConnectSocket,void* file_in_memory,int size);
PVOID file_to_mem(char *sample,int *size);
void send_sample_routine(SOCKET ConnectSocket,char *sample);