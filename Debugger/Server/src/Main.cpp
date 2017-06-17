/* Avoiding Windows.h and winsocks.h conflicts */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif





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

#include <winioctl.h>
#include "inverted_ioctl.h"

#include<TlHelp32.h>

#include<Psapi.h>


#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Kernel32.lib")
#pragma comment(lib, "Psapi.lib")

#define N_COLUMNS 80
#define N_LINES 3
#define MAX_STRING 256
#define SAMPLE_SIZE 10000

#define NOTIFY_EVENT(msg) printf("%s\n",msg);

typedef struct _OVL_WRAPPER {
	OVERLAPPED  Overlapped;
	LONG        ReturnedSequence;
} OVL_WRAPPER, *POVL_WRAPPER;

typedef struct BD
{
	UINT64 PID,TO,FROM;
}BRANCHDATA,*PBRANCHDATA;

typedef struct _OVL_WRAPPER2 {
	OVERLAPPED  Overlapped;
	BRANCHDATA bd;
} OVL_WRAPPER2, *POVL_WRAPPER2;

typedef struct st_reg
{
	UINT64 cr0,cr1,cr2,cr3,cr4;
}REGS,*PREGS;

typedef struct _OVL_WRAPPER3 {
	OVERLAPPED  Overlapped;
	REGS data;
} OVL_WRAPPER3, *POVL_WRAPPER3;

DWORD
	SendTheIoctl(POVL_WRAPPER Wrap, HANDLE driverHandle)
{
	DWORD code;

	//
	// Test the IOCTL interface
	//
	DeviceIoControl(driverHandle,
		(DWORD)IOCTL_OSR_INVERT_NOTIFICATION,
		NULL,                         // Ptr to InBuffer
		0,                            // Length of InBuffer
		&Wrap->ReturnedSequence,      // Ptr to OutBuffer
		sizeof(LONG),                 // Length of OutBuffer
		NULL,                         // BytesReturned
		&Wrap->Overlapped);           // Ptr to Overlapped structure

	code = GetLastError();

	return(code);
}

DWORD
	SendPID(int *monitored_pid,POVL_WRAPPER Wrap, HANDLE driverHandle)
{
	DWORD code;

	//
	// Test the IOCTL interface
	//
	DeviceIoControl(driverHandle,
		(DWORD)IOCTL_MARCUS_SEND_PID,
		monitored_pid,                         // Ptr to InBuffer
		sizeof(int),                            // Length of InBuffer
		&Wrap->ReturnedSequence,      // Ptr to OutBuffer
		sizeof(LONG),                 // Length of OutBuffer
		NULL,                         // BytesReturned
		&Wrap->Overlapped);           // Ptr to Overlapped structure

	code = GetLastError();

	return(code);
}

DWORD getdata(HANDLE driverHandle,POVL_WRAPPER2 Wrap)
{
	DWORD code;
	DeviceIoControl(driverHandle,
		(DWORD)IOCTL_MARCUS_GET_DATA,
		NULL,                         // Ptr to InBuffer
		0,                            // Length of InBuffer
		&Wrap->bd,      // Ptr to OutBuffer
		sizeof(BRANCHDATA),                 // Length of OutBuffer
		NULL,                         // BytesReturned
		&Wrap->Overlapped);           // Ptr to Overlapped structure

	code = GetLastError();

	//printf("%x %x",Wrap->bd.FROM,Wrap->bd.TO);

	return(code);
}

DWORD getregs(HANDLE driverHandle,POVL_WRAPPER3 Wrap)
{
	DWORD code;
	DeviceIoControl(driverHandle,
		(DWORD)IOCTL_MARCUS_GET_REG,
		NULL,                         // Ptr to InBuffer
		0,                            // Length of InBuffer
		&Wrap->data,      // Ptr to OutBuffer
		sizeof(REGS),                 // Length of OutBuffer
		NULL,                         // BytesReturned
		&Wrap->Overlapped);           // Ptr to Overlapped structure

	code = GetLastError();

	printf("%x %x",Wrap->data.cr0,Wrap->data.cr1);

	return(code);
}


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




void usage(char *argv[])
{
	printf("Usage: %s -p <port> (optional)\n",argv[0]);
}

bool Parse(int argc,char *argv[],char **port)
{
	if(argc==1)
	{
		return true;
	}
	if(argc==3)
	{
		if(strcmp(argv[1],"-p")==0)
		{
			*port=argv[2];
			return true;
		}
	}
	return false;
}

int receive_sample(int Socket,void *file_in_memory)
{
	int received=0;
	int offset=0;
	do{
		received=recv(Socket,(char*)((int*)file_in_memory+offset),SAMPLE_SIZE,0);
		offset+=received;
	}while(received>0);
	return offset+1;
}

int launch_suspended_process(char *path)
{
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si,sizeof(si));
	si.cb=sizeof(si);
	ZeroMemory(&pi,sizeof(pi));
	if(!CreateProcessA(path,
		NULL,
		NULL,
		NULL,
		FALSE,
		CREATE_NEW_CONSOLE|CREATE_SUSPENDED,
		NULL,
		NULL,
		&si,
		&pi))
	{
		printf("Launch Malware Error %d\n",GetLastError());
		return 0;
	}else{
		printf("success\n");
	}

	return pi.dwProcessId;
}

void suspend(DWORD processId)
{
	HANDLE hThreadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

	THREADENTRY32 threadEntry; 
	threadEntry.dwSize = sizeof(THREADENTRY32);

	Thread32First(hThreadSnapshot, &threadEntry);

	do
	{
		if (threadEntry.th32OwnerProcessID == processId)
		{
			HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE,
				threadEntry.th32ThreadID);

			SuspendThread(hThread);
			CloseHandle(hThread);
		}
	} while (Thread32Next(hThreadSnapshot, &threadEntry));

	CloseHandle(hThreadSnapshot);
}

typedef LONG (NTAPI *NtSuspendProcess)(IN HANDLE ProcessHandle);

typedef LONG (NTAPI *NtResumeProcess)(IN HANDLE ProcessHandle);

void suspend2(DWORD processId)
{
	HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);

	NtSuspendProcess pfnNtSuspendProcess = (NtSuspendProcess)GetProcAddress(
		GetModuleHandle(L"ntdll"), "NtSuspendProcess");

	pfnNtSuspendProcess(processHandle);
	CloseHandle(processHandle);
}

void resume2(DWORD processId)
{
	HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);

	NtResumeProcess pfnNtResumeProcess = (NtSuspendProcess)GetProcAddress(
		GetModuleHandle(L"ntdll"), "NtResumeProcess");

	pfnNtResumeProcess(processHandle);
	CloseHandle(processHandle);
}

void get_name(char pname[],int monitored_pid)
{
	HANDLE Handle = OpenProcess(
        PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
        FALSE,
        monitored_pid /* This is the PID, you can find one from windows task manager */
    );
	 if (Handle) 
    {
        GetModuleBaseNameA(Handle,NULL,pname,MAX_STRING);
		//printf("%s\n",Buffer);
    }
	 CloseHandle(Handle);
}

void PrintModules( DWORD processID,char modules[])
{
    HMODULE hMods[1024];
    HANDLE hProcess;
    DWORD cbNeeded;
    unsigned int i;

	strcpy(modules,"");

    hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
                            PROCESS_VM_READ,
                            FALSE, processID );
    if (NULL == hProcess)
        return;

   // Get a list of all the modules in this process.

    if( EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
    {
        for ( i = 0; i < (cbNeeded / sizeof(HMODULE)); i++ )
        {
            CHAR szModName[MAX_PATH];

			if ( GetModuleFileNameExA( hProcess, hMods[i], szModName,
                                      sizeof(szModName) / sizeof(CHAR)))
            {
				printf("%s\n",szModName);
				strcat(modules,"> ");
				strcat(modules,szModName);
				strcat(modules,"\n");
            }
        }
    }
    
    CloseHandle( hProcess );
}

int ReadMem(int monitored_pid,UINT64 nbase,UINT64 nsize,char memdump[])
{
	int val;
	HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
                            PROCESS_VM_READ,
                            FALSE, monitored_pid );
    if (NULL == hProcess)
	{
		printf("cannot open proc\n");
		strcpy(memdump,"> Empty buffer");
		val=strlen(memdump)+1;	
	}

	if(ReadProcessMemory(hProcess,(LPCVOID)nbase,memdump,nsize,NULL)>0)
						{
							for(int i=0;i<nsize;i++)
							{
								printf("\\x%02x",memdump[i]);
							}
							printf("\n\n");
							val=nsize;
	}else{
		strcpy(memdump,"> Empty buffer");
		printf("cannot read %x\n",GetLastError());
		val=strlen(memdump)+1;
	}

	CloseHandle(hProcess);

	return val;
}

void string_offset(char offset[],UINT64 off)
{
	printf("looking for %x\n",off);
	char add[9];
	char name[MAX_STRING];
	char line[MAX_STRING];
	int len=MAX_STRING;
	strcpy(offset,"function name");
	FILE *p=fopen("offset.txt","r");
	 while (fgets(line,len,p)) {
     
        printf("%s", line);
		add[0]=line[17];
		add[1]=line[18];
		add[2]=line[19];
		add[3]=line[20];
		add[4]=line[21];
		add[5]=line[22];
		add[6]=line[23];
		add[7]=line[24];
		add[8]='\0';
		printf("\n%s\n",add);
		if(off > _strtoi64(add,NULL,16))
		{
			strcpy(name,&line[26]);
			strcpy(offset,&line[26]);
		}
		printf("%s\n",name);
    }
	fclose(p);
	
}

void introspect(UINT64 addr,char res[],int monitored_pid)
{
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE; 
	MODULEENTRY32 me32; 
	printf("%lx\n",addr);

	hModuleSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, monitored_pid ); 
  if( hModuleSnap == INVALID_HANDLE_VALUE ) 
  {
	  printf("erro\n");
  }

  me32.dwSize = sizeof( MODULEENTRY32 ); 
 
//  Retrieve information about the first module, 
//  and exit if unsuccessful 
  if( !Module32First( hModuleSnap, &me32 ) ) 
  { 
    printf("erro\n");
  } 

  do{
	  ULONG ad=(ULONG)me32.modBaseAddr+me32.modBaseSize;
	  wprintf(me32.szModule);
	  printf("base %x\n",me32.modBaseAddr);
	  printf("size %x\n",me32.modBaseSize);
	  if(addr>(ULONG)me32.modBaseAddr && addr<ad)
	  {
		  //strcpy(res,"found");
		  sprintf(res,"%ws",me32.szModule);
		  if(strcmp(res,"ntdll.dll")==0)
		  {
			  char offset[MAX_STRING];
			  printf("ntdll detected\n");
			  string_offset(offset,addr-ad);
			  strcat(res," + ");
			  strcat(res,offset);
		  }


		  return;
	  }

	  } while( Module32Next( hModuleSnap, &me32 ) ); 

	strcpy(res,"Unknown module");
}

void get_context(char *context,int monitored_pid)
{
	strcpy(context,"Empty");
	HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD,monitored_pid);
	 if (h != INVALID_HANDLE_VALUE) {
  THREADENTRY32 te;
  te.dwSize = sizeof(te);
  if (Thread32First(h, &te)) {
   do {
     if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) +
                      sizeof(te.th32OwnerProcessID)) {

		if(te.th32OwnerProcessID==monitored_pid)
		{
				printf("Process %d Thread %d\n",te.th32OwnerProcessID, te.th32ThreadID);
				HANDLE t=OpenThread(THREAD_ALL_ACCESS|THREAD_QUERY_INFORMATION,FALSE,te.th32ThreadID);
				CONTEXT cont = {0};
				cont.ContextFlags=CONTEXT_FULL;
				//SuspendThread(t);
				if(GetThreadContext(t,&cont)>0)
				{
					printf("open\n");
					char line[MAX_STRING];
					printf("EAX %llx\n",cont.Rax);
					sprintf(line,"> R10 %x\n> R11 %x\n> R12 %x\n> R13 %x\n> R14 %x\n> R15 %x\n> RAX %x\n> RBX %x\n> RCX %x\n> RDX %x\n> RSP %x\n> EFLAG %x\n",cont.R10,cont.R11,cont.R12,cont.R13,cont.R14,cont.R15,cont.Rax,cont.Rbx,cont.Rcx,cont.Rdx,cont.Rsp,cont.EFlags);
					strcpy(context,line);
				}else{
					strcpy(context,"Empty");
					printf("error\n");
				}
				//ResumeThread(t);
				CloseHandle(t);
		}
     }
   te.dwSize = sizeof(te);
   } while (Thread32Next(h, &te));
  }
  CloseHandle(h);
 }
	
}

int main(int argc,char *argv[])
{
	BRANCHDATA data;
	WSADATA wsaData;
	int iResult;
	char DEFAULT_PORT[]="5000";
	char *port=DEFAULT_PORT;

	print_head();



	if(Parse(argc,argv,&port)==false)
	{
		usage(argv);
		return -1;
	}

	printf("PORT %s\n",port);

	NOTIFY_EVENT("Debug Started")

		/* Initialize Socket - MSDN*/
		iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}

	struct addrinfo *result = NULL, *ptr = NULL, hints;

	ZeroMemory(&hints, sizeof (hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	/* Resolve the local address and port to be used by the server */
	iResult = getaddrinfo(NULL, port, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	SOCKET ListenSocket = INVALID_SOCKET;

	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	/* A daemon runs all time */
	while(1){

		if ( listen( ListenSocket, SOMAXCONN ) == SOCKET_ERROR ) {
			printf( "Listen failed with error: %ld\n", WSAGetLastError() );
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}

		SOCKET ClientSocket;

		ClientSocket = INVALID_SOCKET;

		/* Accept a client socket */
		ClientSocket = accept(ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET) {
			printf("accept failed: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}

		HANDLE hFile,completionPortHandle;
		hFile=CreateFile(L"\\\\.\\DebugDriver",
                            GENERIC_READ|GENERIC_WRITE,  // Access rights requested
                            0,                           // Share access - NONE
                            0,                           // Security attributes - not used!
                            OPEN_EXISTING,               // Device must exist to open it.
                            FILE_FLAG_OVERLAPPED,        // Open for overlapped I/O
                            0);                          // extended attributes - not used!
			
		if(hFile==INVALID_HANDLE_VALUE) /* Check */
		{
			printf("driver handler error\n");
			return 0;
		}

		completionPortHandle = CreateIoCompletionPort(hFile,
			NULL,
			0,
			0);

		if(completionPortHandle == NULL) {
			printf("completion erro\n");
		}

		int monitored_pid;

		printf("Handling New Connection\n");
		char opc[MAX_STRING];
		recv(ClientSocket,opc,MAX_STRING,0);
		//printf("received %s\n",opc);
		int opcao;
		opcao=atoi(opc);
		if(opcao==0)
		{
			/* upload */
			NOTIFY_EVENT("Receiving Sample Upload")
				char name[MAX_STRING];
			recv(ClientSocket,name,MAX_STRING,0);
			printf("Writting File %s\n",name);

			char final_name[MAX_STRING];
			char base[]="C:\\DBG\\";
			strcpy(final_name,base);
			strcat(final_name,name);
			printf("Open %s\n",final_name);

			void *file_in_memory = malloc(sizeof(BYTE)*SAMPLE_SIZE);



			FILE *f = fopen(final_name,"wb");

			int size;

			memset(file_in_memory,0,sizeof(BYTE)*SAMPLE_SIZE);

			size=receive_sample(ClientSocket,file_in_memory);

			printf("size %d\n",size);

			fwrite(file_in_memory,sizeof(BYTE),size,f);

			fclose(f);

			int created_pid;
			created_pid=launch_suspended_process(final_name);
			printf("Created PID %d\n",created_pid);
			//char new_pid[]="1234";
			char new_pid[MAX_STRING];
			sprintf(new_pid,"%d",created_pid);
			send(ClientSocket,new_pid,strlen(new_pid)+1,0);
			printf("Created Suspended Process PID %s\n",new_pid);

			monitored_pid=created_pid;

		}else{
			NOTIFY_EVENT("Stopping Process")
				/* running proc*/
				char answer[]="0";
			//suspend(opcao);
			suspend2(opcao);
			send(ClientSocket,answer,strlen(answer)+1,0);
			printf("Suspended Process PID %d\n",opcao);
			//resume2(opcao);

			monitored_pid=opcao;

		}

		POVL_WRAPPER wrap;
		POVL_WRAPPER2 wrap2;
		POVL_WRAPPER3 wrap3;
		DWORD code;
		wrap = (POVL_WRAPPER)malloc(sizeof(OVL_WRAPPER));
				memset(wrap, 0, sizeof(OVL_WRAPPER));

		wrap2 = (POVL_WRAPPER2)malloc(sizeof(OVL_WRAPPER2));
				memset(wrap2,0,sizeof(OVL_WRAPPER2));

		wrap3 = (POVL_WRAPPER3)malloc(sizeof(OVL_WRAPPER3));
				memset(wrap3,0,sizeof(OVL_WRAPPER3));

		printf("sending pid IOCTL\n");
		SendPID(&monitored_pid,wrap,hFile);
		printf("pid IOCTL sent\n");

		char command[MAX_STRING];
		bool exit=false;
		while(!exit)
		{
			recv(ClientSocket,command,MAX_STRING,0);
			if(strcmp(command,"C")==0)
			{
				char context[MAX_STRING];
				get_context(context,monitored_pid);
				send(ClientSocket,context,strlen(context)+1,0);
			}
			if(strcmp(command,"q")==0)
			{
				printf("receive quit\n");
				exit=true;
			}
			if(strcmp(command,"r")==0)
			{
				printf("receive resume\n");
				resume2(monitored_pid);
			}
			if(strcmp(command,"N")==0)
			{
				printf("get name\n");
				char pname[MAX_STRING];
				memset(pname,0,MAX_STRING);
				get_name(pname,monitored_pid);
				send(ClientSocket,pname,strlen(pname)+1,0);
			}
			if(strcmp(command,"l")==0)
			{
				char modules[MAX_STRING];
				printf("list modules\n");
				PrintModules(monitored_pid,modules);
				send(ClientSocket,modules,strlen(modules)+1,0);
			}
			if(strcmp(command,"i")==0 || strcmp(command,"I")==0)
			{
				UINT64 addr;
				char end[MAX_STRING];
				printf("introspection\n");
				recv(ClientSocket,end,MAX_STRING,0);
				addr=_strtoi64(end,NULL,16);
				char res[MAX_STRING];
				introspect(addr,res,monitored_pid);
				send(ClientSocket,res,strlen(res)+1,0);
			}
			if(strcmp(command,"R")==0)
			{
				printf("Register values\n");
				
			//	getregs(hFile,wrap3);

				REGS r;

				DeviceIoControl(hFile,
		(DWORD)IOCTL_MARCUS_GET_REG,
		NULL,                         // Ptr to InBuffer
		0,                            // Length of InBuffer
		&r,      // Ptr to OutBuffer
		sizeof(REGS),                 // Length of OutBuffer
		NULL,                         // BytesReturned
		NULL);           // Ptr to Overlapped structure
				printf("%x %x %x\n",r.cr0,r.cr1,r.cr2);
				char res2[MAX_STRING];
				//printf("%x %x %x\n",wrap3->data.cr0,wrap3->data.cr1,wrap3->data.cr2);
				sprintf(res2,"CR0: 0x%08x\n> CR2: 0x%08x\n> CR3: 0x%08x\n> CR4: 0x%08x\n> CR8: 0x%08x",r.cr0,r.cr1,r.cr2,r.cr3,r.cr4);
				send(ClientSocket,res2,strlen(res2)+1,0);
			}
			if(strcmp(command,"s")==0)
			{
				printf("receive suspend\n");
				suspend2(monitored_pid);
			}
			if(strcmp(command,"d")==0)
			{
				ULONG64 nbase,nsize;
				char base[MAX_STRING];
				char size[MAX_STRING];
				printf("dump\n");
				recv(ClientSocket,base,MAX_STRING,0);
				Sleep(1000);
				recv(ClientSocket,size,MAX_STRING,0);
				printf("%s %s\n",base,size);
				nbase=_strtoi64(base,NULL,16);
				nsize=_strtoi64(size,NULL,16);
				printf("%llx %llx\n",nbase,nsize);
				char memdump[1024];
				int val;
				val=ReadMem(monitored_pid,nbase,nsize,memdump);
				send(ClientSocket,memdump,val,0);
			}
			if(strcmp(command,"n")==0)
			{
				printf("receive next\n");
				

				printf("sending ioctl\n");

				

				wrap = (POVL_WRAPPER)malloc(sizeof(OVL_WRAPPER));
				memset(wrap, 0, sizeof(OVL_WRAPPER));

				code = SendTheIoctl(wrap,hFile);

				printf("IOCTL sent\n");

				code = GetLastError();

				if(code != ERROR_IO_PENDING)  {

					printf("DeviceIoControl failed with error 0x%x\n", code);

					return(0);

				}

				DWORD byteCount = 0;
				ULONG_PTR compKey = 0;
				OVERLAPPED* overlapped = NULL;
				wrap = NULL;
				// Wait for a completion notification.
				overlapped = NULL;

				printf("waiting for event\n");
				resume2(monitored_pid);

				BOOL worked = GetQueuedCompletionStatus(
					completionPortHandle,                // Completion port handle
					&byteCount,                // Bytes transferred
					&compKey,                  // Completion key... don't care
					&overlapped,               // OVERLAPPED structure
					INFINITE);                 // Notification time-out interval

				//
				// If it's our notification ioctl that's just been completed...
				// don't do anything special.
				// 

				

				if (byteCount == 0) {
					printf("zero\n");
				}

				if (overlapped == NULL) {

					// An unrecoverable error occurred in the completion port.
					// Wait for the next notification.
					printf("NULL\n");
				}

				//
				// Because the wrapper structure STARTS with the OVERLAPPED structure,
				// the pointers are the same.  It would be nicer to use
				// CONTAINING_RECORD here... however you do that in user-mode.
				// 
				wrap = (POVL_WRAPPER)overlapped;
				code = GetLastError();
				printf(">>> Notification received.  Sequence = %d\n",
					wrap->ReturnedSequence);

				suspend2(monitored_pid);

				//getdata(hFile,&data);
				//printf("FROM %x TO %x\n",data.FROM,data.TO);

				
				getdata(hFile,wrap2);
				//printf("%x %x\n",wrap2->bd.FROM,wrap2->bd.TO);

				char from_string[MAX_STRING];
				char to_string[MAX_STRING];

				sprintf(from_string,"%x",wrap2->bd.FROM);
				sprintf(to_string,"%x",wrap2->bd.TO);

				Sleep(1000);
				send(ClientSocket,from_string,strlen(from_string)+1,0);
				Sleep(1000);
				send(ClientSocket,to_string,strlen(to_string)+1,0);
				Sleep(1000);

			}
		}

		CloseHandle(hFile);
		CloseHandle(completionPortHandle);

	}



	return 0;
}