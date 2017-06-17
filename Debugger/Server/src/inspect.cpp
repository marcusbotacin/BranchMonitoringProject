/* BranchMonitoring Project 
 * Debugger Server-side
 * Marcus Botacin
 * UNICAMP - 2017
 */

/* general includes */
#include "includes.h"
/* inspection functions */
#include "inspect.h"
/* string functions and defs */
#include "string_utils.h"
/* print defs */
#include "print.h"
/* process defs */
#include "process.h"
/* debug defs */
#include "debug.h"

/* get process name */
void get_name(char pname[],int monitored_pid)
{
	/* try to open the process */
	HANDLE Handle = OpenProcess(
        PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
        PROC_NO_INHERIT,
        monitored_pid /* This is the PID, you can find one from windows task manager */
    );
	/* if successful */
	 if (Handle) 
    {
        GetModuleBaseNameA(Handle,NULL,pname,MAX_STRING);
    }
	 CloseHandle(Handle);
}

/* get loaded modules */
void PrintModules(DWORD processID,char modules[])
{
	/* setup vars */
    HMODULE hMods[MAX_MODULES];
    HANDLE hProcess;
    DWORD cbNeeded;
    unsigned int i;
	strcpy(modules,"");

	/* try to open process */
    hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
                            PROCESS_VM_READ,
                            PROC_NO_INHERIT, processID );

	/* case error */
    if (NULL == hProcess)
        return;

   /* Get a list of all loaded modules */

    if( EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
    {
        for ( i = 0; i < (cbNeeded / sizeof(HMODULE)); i++ )
        {
            CHAR szModName[MAX_PATH];

			if ( GetModuleFileNameExA( hProcess, hMods[i], szModName,
                                      sizeof(szModName) / sizeof(CHAR)))
            {
#ifdef ENABLE_DBG_PRINT
				printf("%s\n",szModName);
#endif
				strcat(modules,PROMPT_CHAR);
				strcat(modules,szModName);
				strcat(modules,STRING_NEW_LINE);
            }
        }
    }
    CloseHandle( hProcess );
}

/* Read bytes from process memory */
int ReadMem(int monitored_pid,UINT64 nbase,UINT64 nsize,char memdump[])
{
	int val;

	/* Open Process */
	HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
                            PROCESS_VM_READ,
                            PROC_NO_INHERIT, monitored_pid );
	/* Case Error */
    if (NULL == hProcess)
	{
		console_message("cannot open process");
		strcpy(memdump,EMPTY_AS_STRING);
		val=string_size(memdump);	
	}

	/* Try to read mem */
	if(ReadProcessMemory(hProcess,(LPCVOID)nbase,memdump,nsize,NULL)>ERROR)
	{
		/* case successful */
#ifdef ENABLE_DBG_PRINT
		for(int i=0;i<nsize;i++)
		{
			printf("\\x%02x",memdump[i]);
		}
		printf("\n\n");
#endif
		val=nsize;
	}else{
		/* case error */
		strcpy(memdump,EMPTY_AS_STRING);
		console_message_with_code("cannot read mem",GetLastError());
		val=string_size(memdump);
	}
	CloseHandle(hProcess);
	return val;
}

/* return the function inside a given offset */
void string_offset(char offset[],UINT64 off)
{
	int i;
#ifdef ENABLE_DBG_PRINT
	printf("looking for %x\n",off);
#endif
	char add[MAX_ADDR_AS_STRING];
	char name[MAX_STRING];
	char line[MAX_STRING];
	int len=MAX_STRING;
	strcpy(offset,FUNC_MODULE);
	FILE *p=fopen(OFF_FILE,READ_FLAG);
	while (fgets(line,len,p)) {
#ifdef ENABLE_DBG_PRINT
        printf("%s", line);
#endif

		for(i=0;i<ADDR_SIZE;i++)
		{
			add[i]=line[ADDR_OFFSET+i];
		}
		add[ADDR_SIZE]=STRING_END;

#ifdef ENABLE_DBG_PRINT
		printf("\n%s\n",add);
#endif
		if(off > _strtoi64(add,NULL,HEX))
		{
			strcpy(name,&line[ADDR_SIZE+ADDR_OFFSET+1]);
			strcpy(offset,&line[ADDR_SIZE+ADDR_OFFSET+1]);
		}
#ifdef ENABLE_DBG_PRINT
		printf("%s\n",name);
#endif
    }
	fclose(p);
}

/* introspect into given address */
void introspect(UINT64 addr,char res[],int monitored_pid)
{
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE; 
	MODULEENTRY32 me32; 

#ifdef ENABLE_DBG_PRINT
	printf("%lx\n",addr);
#endif

  hModuleSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, monitored_pid ); 
  if( hModuleSnap == INVALID_HANDLE_VALUE ) 
  {
	  console_message("Snapshot error");
  }

  me32.dwSize = sizeof( MODULEENTRY32 ); 
 
  /*  Retrieve information about the first module and exit if unsuccessful */
  if( !Module32First( hModuleSnap, &me32 ) ) 
  { 
    console_message("ModuleFirst error");
  } 

  /* for all modules */
  do{
	  ULONG ad=(ULONG)me32.modBaseAddr+me32.modBaseSize;
	  wprintf(me32.szModule);

#ifdef ENABLE_DBG_PRINT
	  printf("base %x\n",me32.modBaseAddr);
	  printf("size %x\n",me32.modBaseSize);
#endif

	  /* if the supplied address is inside this module */
	  if(addr>(ULONG)me32.modBaseAddr && addr<ad)
	  {
		  sprintf(res,"%ws",me32.szModule);

		  /* if this module has additional introspection information */
		  if(strcmp(res,NTDLL_NAME)==0)
		  {
			  char offset[MAX_STRING];
#ifdef ENABLE_DBG_PRINT
			  printf("ntdll detected\n");
#endif

			  /* build offset string */
			  string_offset(offset,addr-ad);
			  strcat(res,STR_OFFSET_ADD_SYMBOL);
			  strcat(res,offset);
		  }
		  return;
	  }
	  } while( Module32Next( hModuleSnap, &me32 ) ); 

	/* if you are here, introspection have failed */
	strcpy(res,UKN_MODULE);
}

/* get thread context */
void get_context(char *context,int monitored_pid)
{
	/* Start having no result*/
	strcpy(context,EMPTY_AS_STRING);
	/* enumerate threads from the given PID */
	HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD,monitored_pid);
	/* case success */
	if (h != INVALID_HANDLE_VALUE) {
		THREADENTRY32 te;
		te.dwSize = sizeof(te);
		if (Thread32First(h, &te)) {
			do {
				if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) +
                      sizeof(te.th32OwnerProcessID)) {
					if(te.th32OwnerProcessID==monitored_pid)
					{
#ifdef ENABLE_DBG_PRINT
						printf("Process %d Thread %d\n",te.th32OwnerProcessID, te.th32ThreadID);
#endif 
						HANDLE t=OpenThread(THREAD_ALL_ACCESS|THREAD_QUERY_INFORMATION,FALSE,te.th32ThreadID);
						CONTEXT cont = {0};
						cont.ContextFlags=CONTEXT_FULL;
						if(GetThreadContext(t,&cont)>0)
						{
#ifdef ENABLE_DBG_PRINT
							printf("open\n");
#endif
							char line[MAX_STRING];
#ifdef ENABLE_DBG_PRINT
							printf("EAX %llx\n",cont.Rax);
#endif
							/* serialize context as string */
							sprintf(line,"> R10 %x\n> R11 %x\n> R12 %x\n> R13 %x\n> R14 %x\n> R15 %x\n> RAX %x\n> RBX %x\n> RCX %x\n> RDX %x\n> RSP %x\n> EFLAG %x\n",cont.R10,cont.R11,cont.R12,cont.R13,cont.R14,cont.R15,cont.Rax,cont.Rbx,cont.Rcx,cont.Rdx,cont.Rsp,cont.EFlags);
							strcpy(context,line);
						}else{
							strcpy(context,EMPTY_AS_STRING);
#ifdef ENABLE_DBG_PRINT
							printf("error\n");
#endif
						}
					CloseHandle(t);
				}
			}
		te.dwSize = sizeof(te);
	} while (Thread32Next(h, &te));
  }
  CloseHandle(h);
 }
}
