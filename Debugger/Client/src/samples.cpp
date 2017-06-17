/* Branch Monitoring Project
 * Debugger Client
 * Marcus Botacin
 * UNICAMP - 2017
 */

/* Generic include */
#include "includes.h"
/* Defs for handling samples */
#include "samples.h"
/* Options defs */
#include "options.h"
/* network defs */
#include "network_defs.h"
/* string utils */
#include "string_utils.h"

/* get sample name from the complete path */
/* Alternative implementations:
 * C: use strtok("\\")
 * C++: use split("\\")
 */
char* get_sample_name(char *name)
{
	int size,i;
	size=strlen(name);
	i=size-1;
	/* going backward */
	while(i>END_OF_FILE)
	{
		/* at the first separator */
		if(name[i]==SLASH)
		{
			/* return string pointer */
			return  (name+i+1);
		}
		/* going back */
		i--;
	}
}

/* send sample to the server */
/* functionality not available on gdb */
void send_sample(int ConnectSocket,void* file_in_memory,int size)
{
	int sent=MIN_BYTES;
	int offset=MIN_BYTES;
	/* can't send whole file at once 
	 * send chunks each time
	 */
	do{
		sent=send(ConnectSocket,(char*)((int*)file_in_memory+offset),size,SOCKET_FLAGS);
		/* increase offset by the number of sent bytes */
		offset+=sent;
	}while(sent>MIN_BYTES);
	/* while having bytes to send */
}

/* load binary from file to memory */
PVOID file_to_mem(char *sample,int *size)
{
	FILE *f = fopen(sample,"rb");
	VOID *file_in_memory = malloc(sizeof(BYTE)*SAMPLE_SIZE);
	memset(file_in_memory,MIN_BYTES,sizeof(BYTE)*SAMPLE_SIZE);
	*size=fread(file_in_memory,sizeof(BYTE),SAMPLE_SIZE,f);
	printf("READ %d bytes\n",*size);
	return file_in_memory;
}

/* send sample through the network */
void send_sample_routine(SOCKET ConnectSocket,char *sample)
{
	int size;
	char *name;
	char opc[]=ZERO_STRING;
	VOID *file;
	name=get_sample_name(sample);
	printf("Sending Sample %s\n",name);
	send(ConnectSocket,opc,string_size(opc),SOCKET_FLAGS);
	Sleep(WAIT_TIME);
	send(ConnectSocket,name,string_size(name),SOCKET_FLAGS);
	file = file_to_mem(sample,&size);
	send_sample(ConnectSocket,file,size);
}