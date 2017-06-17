/* Branch Monitoring Project
 * Debugger Client
 * Marcus Botacin
 * UNICAMP - 2017
 */

/* General Includes */
#include "includes.h"
/* Function definitions */
#include "functions.h"
/* network definitions */
#include "network_defs.h"
/* string utils */
#include "string_utils.h"
/* options defs */
#include "options.h"

/* ask for context data */
void get_context(SOCKET ConnectSocket)
{
	char cmd[MAX_STRING];
	char context[MAX_STRING];
	str_from_char(CTX_CHAR,context);
	send(ConnectSocket,cmd,string_size(cmd),SOCKET_FLAGS);
	recv(ConnectSocket,context,MAX_STRING,SOCKET_FLAGS);
	printf("%s",context);
}

/* Ask for remote process resume */
void resume(SOCKET ConnectSocket)
{
	char cmd[MAX_STRING];
	str_from_char(RESUME_CHAR,cmd);
	send(ConnectSocket,cmd,string_size(cmd),SOCKET_FLAGS);
}

/* suspend the remote process */
void suspend(SOCKET ConnectSocket)
{
	char cmd[MAX_STRING];
	str_from_char(SUSP_CHAR,cmd);
	send(ConnectSocket,cmd,string_size(cmd),SOCKET_FLAGS);
}

/* ask for introspection data on FROM address */
void introspect_from(SOCKET ConnectSocket,char *adfrom)
{
	char cmd[MAX_STRING];
	str_from_char(IFROM_CHAR,cmd);
	send(ConnectSocket,cmd,string_size(cmd),SOCKET_FLAGS);
	char intro[MAX_STRING];
	send(ConnectSocket,adfrom,string_size(adfrom),SOCKET_FLAGS);
	recv(ConnectSocket,intro,MAX_STRING,SOCKET_FLAGS);
	printf("> %s\n",intro);
}

/* introspect into TO address */
void introspect_to(SOCKET ConnectSocket, char *adto)
{
	char cmd[MAX_STRING];
	str_from_char(ITO_CHAR,cmd);
	send(ConnectSocket,cmd,string_size(cmd),SOCKET_FLAGS);
	char intro2[MAX_STRING];
	send(ConnectSocket,adto,string_size(adto),SOCKET_FLAGS);
	recv(ConnectSocket,intro2,MAX_STRING,SOCKET_FLAGS);
	printf("> %s\n",intro2);
}

/* get registers */
void get_regs(SOCKET ConnectSocket)
{
	char cmd[MAX_STRING];
	str_from_char(REG_CHAR,cmd);
	send(ConnectSocket,cmd,string_size(cmd),SOCKET_FLAGS);
	char reg[MAX_STRING];
	recv(ConnectSocket,reg,MAX_STRING,SOCKET_FLAGS);
	printf("> %s\n",reg);
}

/* step one branch */
void next_branch(SOCKET ConnectSocket,char *adfrom, char *adto)
{
	char cmd[MAX_STRING];
	str_from_char(NEXT_CHAR,cmd);
	send(ConnectSocket,cmd,string_size(cmd),SOCKET_FLAGS);
	char string_from[MAX_STRING],string_to[MAX_STRING];
	Sleep(WAIT_TIME);
	recv(ConnectSocket,string_from,MAX_STRING,SOCKET_FLAGS);
	strcpy(adfrom,string_from);
	Sleep(WAIT_TIME);
	recv(ConnectSocket,string_to,MAX_STRING,SOCKET_FLAGS);
	strcpy(adto,string_to);
	printf("<0x%s> BRANCH TO 0x%s\n",string_from,string_to);
}

/* send finish command */
void remote_exit(SOCKET ConnectSocket,bool *exit)
{
	char cmd[MAX_STRING];
	str_from_char(QUIT_CHAR,cmd);
	send(ConnectSocket,cmd,string_size(cmd),SOCKET_FLAGS);
	*exit=true;
}

/* request memory dump */
void dump_mem(SOCKET ConnectSocket, char *base, char *size)
{
	char dump[MAX_MEM_DUMP];
	char cmd[MAX_STRING];
	str_from_char(DUMP_CHAR,cmd);
	send(ConnectSocket,cmd,string_size(cmd),SOCKET_FLAGS);
	send(ConnectSocket,base,string_size(base),SOCKET_FLAGS);
	Sleep(WAIT_TIME);
	send(ConnectSocket,size,string_size(size),SOCKET_FLAGS);
	recv(ConnectSocket,dump,MAX_MEM_DUMP,SOCKET_FLAGS);
	if(strcmp(dump,"> Empty buffer")==EQUAL)
	{
		printf("%s\n",dump);
	}else{
		printf("> ");
		for(int i=0;i<_strtoi64(size,NULL,HEX);i++)
		{
			printf("\\x%02x",dump[i] & MASK);
		}
		printf("\n");
	}
}

/* get remote process name */
void retrieve_name(SOCKET ConnectSocket)
{
	char cmd[MAX_STRING];
	str_from_char(NAME_CHAR,cmd);
	send(ConnectSocket,cmd,string_size(cmd),SOCKET_FLAGS);
	recv(ConnectSocket,cmd,MAX_STRING,SOCKET_FLAGS);
	printf("> %s\n",cmd);
}

/* list loaded libraries */
void loaded_libs(SOCKET ConnectSocket)
{
	char cmd[MAX_STRING];
	str_from_char(LIST_CHAR,cmd);
	send(ConnectSocket,cmd,string_size(cmd),SOCKET_FLAGS);
	recv(ConnectSocket,cmd,MAX_STRING,SOCKET_FLAGS);
	printf("%s",cmd);
}

/* Check what happened to the remote process */
void get_process_status(SOCKET ConnectSocket, char *pid)
{
	int option;
	char res[MAX_STRING];
	recv(ConnectSocket,res,MAX_STRING,SOCKET_FLAGS);
	option=atoi(res);
	if(option==SUSPENDED_PROCESS)
	{
		printf("> Process %d suspended\n",atoi(pid));
	}else{
		printf("> Created suspended process PID %d\n",option);
	}
}