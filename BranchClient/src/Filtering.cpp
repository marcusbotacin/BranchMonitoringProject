/* Branch Monitor 
 * Introspection Client
 * Marcus Botacin, 2017
 */

#include "config.h"
#include "introspection.h"
#include "disasm.h"

/* Include all introspected libraries */

#include "msv.h"
#include "nt.h"
#include "kb.h"
#include "k32.h"
#include "wow.h"
#include "wowcpu.h"
#include "wowwin.h"
#include "wownt.h"
#include "wowuser.h"

/* global */
extern imgs binary;
extern imgs libs[];
extern int n_libs;

UINT64 LAST=-1;

/* given a library, return a pointer to the introspection struct */
/* TODO: Include ptr field into lib mapping */
PFUNCTION_INTROSPECTION get_ptr(char lib[])
{
	PFUNCTION_INTROSPECTION ptr;
	if(stricmp(lib,"C:\\Windows\\System32\\msvcr110d.dll")==0)
	{
		ptr=msv;
	}else if(stricmp(lib,"C:\\Windows\\System32\\ntdll.dll")==0){
		ptr=nt;
	}else if(stricmp(lib,"C:\\Windows\\System32\\kernelbase.dll")==0){
		ptr=kb;
	}else if(stricmp(lib,"C:\\Windows\\System32\\kernel32.dll")==0){
		ptr=k32;
	}else if(stricmp(lib,"C:\\Windows\\System32\\wow64.dll")==0){
		ptr=wow64;
	}else if(stricmp(lib,"C:\\Windows\\System32\\wow64cpu.dll")==0){
		ptr=wow64cpu;
	}else if(stricmp(lib,"C:\\Windows\\System32\\wow64win.dll")==0){
		ptr=wow64win;
	}else if(stricmp(lib,"C:\\Windows\\SysWOW64\\ntdll.dll")==0){
		ptr=wow_ntdll;
	}else if(stricmp(lib,"C:\\Windows\\SysWOW64\\user32.dll")==0){
		ptr=wow_user32;
	}else{
		return NULL;
	}

}

/* given an address and a library, return function name */
void get_lib_function(char name[],char lib[],UINT64 addr)
{
	int i=0;
	/* get function pointer */
	PFUNCTION_INTROSPECTION ptr=get_ptr(lib);
	if(ptr==NULL)
	{
		strcpy(name,"Unknown Function");
		return;
	}
	
	/* until we find */
	/* Assured: It stops, since all structs have a "unknown" entry in the last position */
	while(TRUE)
	{
		/* Data on decreasing order -- we can stop at first entry */
		if(addr >= ptr[i].addr)
		{
			strcpy(name,ptr[i].name);
			UINT64 offset = addr - ptr[i].addr;
			/* Function offset case -- usual on RET */
			if(offset>0)
			{
				char str_off[64];
				sprintf(str_off,"+0x%x",offset);
				strcat(name,str_off);
			}
			return;
		}
		i++;
	}
	return;

}

/* TODO: Use a macro, instead */
UINT64 offset_calculator(UINT64 base_addr,UINT64 addr)
{
	return addr-base_addr;
}

/* Perform transition analysis */
void flow_analysis(UINT64 FROM,UINT64 TO)
{
	/* Declarations */
	char function_from[1024];
	char function_to[1024];
	UINT64 offset_from, offset_to;
	BOOL bin_FROM,bin_TO;
	int lib_FROM,lib_TO;

	/* Prior data -- try all possibilities */
	/* FROM and TO are process or library */
	/* Other cases, as exceptions, are not handled */
	bin_FROM=is_process(FROM);
	bin_TO=is_process(TO);
	lib_FROM=is_library(FROM);
	lib_TO=is_library(TO);

	/* Flow cases */

	/* FROM is Binary and TO is Library: CALL */
	if(bin_FROM && lib_TO!=-1)
	{
		offset_from=offset_calculator(binary.addr_min,FROM);
		offset_to=offset_calculator(libs[lib_TO].addr_min,TO);
		get_lib_function(function_to,libs[lib_TO].name,offset_to);
		printf("Binary %s at <0x%llx> called %s at <%s>\n",binary.name,offset_from,libs[lib_TO].name,function_to);
	}
	/* FROM is library and TO is Binary: RETURN */
	else if(lib_FROM!=-1 && bin_TO)
	{
		offset_from=offset_calculator(libs[lib_FROM].addr_min,FROM);
		offset_to=offset_calculator(binary.addr_min,TO);
		get_lib_function(function_from,libs[lib_FROM].name,offset_from);
		printf("Library %s at <%s> returned to Binary %s at <0x%llx>\n",libs[lib_FROM].name,function_from,binary.name,offset_to);
		LAST=TO;
	}
	/* Bin to Bin */
	else if(bin_FROM && bin_TO)
	{
		offset_from=offset_calculator(binary.addr_min,FROM);
		offset_to=offset_calculator(binary.addr_min,TO);
		printf("Binary %s at <0x%llx> to Binary %s at <0x%llx>\n",binary.name,offset_from,binary.name,offset_to);
		do_disasm(LAST,FROM);
		LAST=TO;
	}
	/* lib to lib */
	else if(lib_FROM!=-1 && lib_TO!=-1)
	{
		offset_from=offset_calculator(libs[lib_FROM].addr_min,FROM);
		offset_to=offset_calculator(libs[lib_TO].addr_min,TO);
		get_lib_function(function_from,libs[lib_FROM].name,offset_from);
		get_lib_function(function_to,libs[lib_TO].name,offset_to);
		//printf("Library %s at <%s> to Library %s at <%s>\n",libs[lib_FROM].name,function_from,libs[lib_TO].name,function_to);
	}
	/* Any other case is exception, not handled */
}

/* Check if address is inside a library image space */
int is_library(UINT64 addr)
{
	
	int i;
	/* For each lib */
	for(i=0;i<n_libs;i++)
	{
		if(addr>=libs[i].addr_min && addr<=libs[i].addr_max)
		{
			/* return the lib index */
			return i;
		}
	}
	return -1;
}

/* Check if a given address is on the process space */
/* Not required if PID is supplied by the driver */
BOOL is_process(UINT64 addr)
{
	if(addr>=binary.addr_min && addr<=binary.addr_max)
	{
		return TRUE;
	}
	return FALSE;
}