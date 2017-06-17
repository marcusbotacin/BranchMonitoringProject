/* Avoiding Windows.h and winsocks.h conflicts */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include<shellapi.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#pragma comment(lib, "Ws2_32.lib")

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

#define N_COLUMNS 80
#define N_LINES 3
#define MAX_STRING 256
#define SAMPLE_SIZE 10000

#define NOTIFY_EVENT(msg) printf("%s\n",msg);

void print_line(int size)
{
	for(int j=0;j<size;j++)
	{
		printf("*");
	}
	return;
}

void print_head()
{
	int first,last;
	char logo[]=" YAD - Yet Another Debugger ";
	for(int i=0;i<N_LINES;i++)
	{
		if(i==1)
		{
			first=(N_COLUMNS-strlen(logo))/2;
			last=N_COLUMNS-strlen(logo)-first;
			print_line(first);
			printf("%s",logo);
			print_line(last);
			printf("\n");
		}else{
			print_line(N_COLUMNS);
			printf("\n");
		}
	}
	return;
}

bool parse(int argc,char *argv[],char **sample,char **port,char **addr,char **pid)
{
	int i=1;
	bool sflag,pflag,aflag,pidflag;
	sflag=pflag=aflag=pidflag=false;
	if(argc==1)
	{
		return false;
	}
	while(i<argc)
	{
		if(strncmp(argv[i],"-s",MAX_STRING)==0)
		{
			*sample=argv[i+1];
			sflag=true;
		}
		if(strncmp(argv[i],"-p",MAX_STRING)==0)
		{
			*port=argv[i+1];
			pflag=true;
		}
		if(strncmp(argv[i],"-pid",MAX_STRING)==0)
		{
			*pid=argv[i+1];
			pidflag=true;
		}
		if(strncmp(argv[i],"-a",MAX_STRING)==0)
		{
			*addr=argv[i+1];
			aflag=true;
		}
		i+=2;
	}
	if((pidflag==true || sflag==true) && pflag==true && aflag==true)
	{
		return true;
	}else{
		return false;
	}
}

void usage(char *argv[])
{
	printf("Usage: %s -s <sample> -p <port> -a <addr>\n",argv[0]);
	printf("Usage: %s -pid <pid> -p <port> -a <addr>\n",argv[0]);
}


char* get_sample_name(char *name)
{
	int size,i;
	size=strlen(name);
	i=size-1;
	while(i>0)
	{
		if(name[i]=='\\')
		{
			return  (name+i+1);
		}
		i--;
	}
}

void send_sample(int ConnectSocket,void* file_in_memory,int size)
{
	int sent=0;
	int offset=0;
	do{
		sent=send(ConnectSocket,(char*)((int*)file_in_memory+offset),size,0);
		offset+=sent;
	}while(sent>0);
}

void debug_options()
{
	printf("\t(h)elp\n\t(r)esume\n\t(s)uspend\n\t(n)ext branch\n\t(q)uit\n\t(d)ump memory\n\t(D)isasm\n\t(N)ame\n\t(l)oaded libs\n\t(b)reakpoint\n\t(c)ontinue\n\t(i)ntrospect from\n\t(I)ntrospect to\n\t(R)egister values\n\t(C)ontext\n");
}

int main(int argc,char *argv[])
{
	char adfrom[MAX_STRING];
	char adto[MAX_STRING];
	char *name=NULL;
	char *port=NULL;
	char *sample=NULL;
	char *addr=NULL;
	char *pid=NULL;
	print_head();
	if(parse(argc,argv,&sample,&port,&addr,&pid)!=true)
	{
		usage(argv);
		return -1;
	}

	

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
    return 1;
}

	if(pid==NULL)
	{
	//NOTIFY_EVENT("Sending Sample")

	name=get_sample_name(sample);
	
	printf("Sending Sample %s\n",name);

	char opc[]="0";
	
	send(ConnectSocket,opc,strlen(opc)+1,0);

	Sleep(1000);

	send(ConnectSocket,name,strlen(name)+1,0);
	
	FILE *f = fopen(sample,"rb");

	VOID *file_in_memory = malloc(sizeof(BYTE)*SAMPLE_SIZE);
	memset(file_in_memory,0,sizeof(BYTE)*SAMPLE_SIZE);
	int size;
	size=fread(file_in_memory,sizeof(BYTE),SAMPLE_SIZE,f);
	printf("READ %d bytes\n",size);

	send_sample(ConnectSocket,file_in_memory,size);

	fclose(f);

	}else{
		//NOTIFY_EVENT("Sending PID")
		printf("> Sending PID %s\n",pid);
		send(ConnectSocket,pid,strlen(pid)+1,0);
	}

	char res[MAX_STRING];
	recv(ConnectSocket,res,MAX_STRING,0);
	int opcao;
	opcao=atoi(res);
	if(opcao==0)
	{
		printf("> Process %d suspended\n",atoi(pid));
	}else{
		printf("> Created suspended process PID %d\n",opcao);
	}

	char command;
	bool exit=false;
	char cmd[MAX_STRING];
	while(!exit)
	{
		printf("> ");
		scanf(" %c",&command);
		switch (command)
		{
		case 'C':
			printf("> Context\n");
			strcpy(cmd,"C");
			send(ConnectSocket,cmd,strlen(cmd)+1,0);
			char context[MAX_STRING];
			recv(ConnectSocket,context,MAX_STRING,0);
			printf("%s",context);
			break;
		case 'r':
			printf("> resume\n");
			strcpy(cmd,"r");
			send(ConnectSocket,cmd,strlen(cmd)+1,0);
			break;
		case 's':
			printf("> suspend\n");
			strcpy(cmd,"s");
			send(ConnectSocket,cmd,strlen(cmd)+1,0);
			break;
		case 'i':
			printf("> Introspect From\n");
			strcpy(cmd,"i");
			send(ConnectSocket,cmd,strlen(cmd)+1,0);
			char intro[MAX_STRING];
			//scanf("%s",adfrom);
			send(ConnectSocket,adfrom,strlen(adfrom)+1,0);
			recv(ConnectSocket,intro,MAX_STRING,0);
			printf("> %s\n",intro);
			break;
		case 'I':
			printf("> Instrospect To\n");
			strcpy(cmd,"I");
			send(ConnectSocket,cmd,strlen(cmd)+1,0);
			char intro2[MAX_STRING];
			//scanf("%s",adto);
			send(ConnectSocket,adto,strlen(adto)+1,0);
			recv(ConnectSocket,intro2,MAX_STRING,0);
			printf("> %s\n",intro2);
			break;
		case 'R':
			printf("> Register values\n");
			strcpy(cmd,"R");
			send(ConnectSocket,cmd,strlen(cmd)+1,0);
			char reg[MAX_STRING];
			recv(ConnectSocket,reg,MAX_STRING,0);
			printf("> %s\n",reg);
			break;
		case 'n':
			printf("> next branch\n");
			strcpy(cmd,"n");
			send(ConnectSocket,cmd,strlen(cmd)+1,0);
			char string_from[MAX_STRING],string_to[MAX_STRING];
			Sleep(1000);
			recv(ConnectSocket,string_from,MAX_STRING,0);
			strcpy(adfrom,string_from);
			Sleep(1000);
			recv(ConnectSocket,string_to,MAX_STRING,0);
			strcpy(adto,string_to);
			printf("<0x%s> BRANCH TO 0x%s\n",string_from,string_to);
			break;
		case 'h':
			printf("> help\n");
			debug_options();
			break;
		case 'q':
			printf("> quit\n");
			strcpy(cmd,"q");
			send(ConnectSocket,cmd,strlen(cmd)+1,0);
			exit=true;
			break;
		case 'd':
			strcpy(cmd,"d");
			send(ConnectSocket,cmd,strlen(cmd)+1,0);
			char base[MAX_STRING];
			char size[MAX_STRING];
			printf("> dump memory\n");
			scanf("%s",base);
			scanf("%s",size);
			send(ConnectSocket,base,strlen(base)+1,0);
			Sleep(1000);
			send(ConnectSocket,size,strlen(size)+1,0);
			char dump[1024];
			recv(ConnectSocket,dump,1024,0);
			if(strcmp(dump,"> Empty buffer")==0)
			{
				printf("%s\n",dump);
			}else{
				printf("> ");
				for(int i=0;i<_strtoi64(size,NULL,16);i++)
				{
					printf("\\x%02x",dump[i] & 0xFF);
				}
				printf("\n");
			}
			break;
		case 'D':
			printf("> Disasm code\n");
			break;
		case 'N':
			printf("> Retrieve name\n");
			strcpy(cmd,"N");
			send(ConnectSocket,cmd,strlen(cmd)+1,0);
			recv(ConnectSocket,cmd,MAX_STRING,0);
			printf("> %s\n",cmd);
			break;
		case 'l':
			printf("> list loaded libs\n");
			strcpy(cmd,"l");
			send(ConnectSocket,cmd,strlen(cmd)+1,0);
			recv(ConnectSocket,cmd,MAX_STRING,0);
			printf("%s",cmd);
			break;
		case 'b':
			printf("> breakpoint\n");
			break;
		case 'c':
			printf("> continue\n");
			break;
		default:
			printf("> error\n");
			debug_options();
			break;
		}
	}


	return 0;
}