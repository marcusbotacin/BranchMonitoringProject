#include "disasm.h"
#include "C:\Users\marcus\Downloads\capstone-3.0.4-win64\include\capstone.h"

extern HANDLE proc;

#pragma comment(lib, "C:\\Users\\marcus\\Downloads\\capstone-3.0.4-win64\\capstone.dll")
#pragma comment(lib, "C:\\Users\\marcus\\Downloads\\capstone-3.0.4-win64\\capstone.lib")

void disasm_one_instr(UINT64 addr)
{
	uint8_t instruction_buffer[32];
	cs_insn *insn;
	size_t count;
	csh handle;
	int i,n=10;
	char buffer[32];
	printf("Disasm 1 instr from %llx\n",addr);
	if(ReadProcessMemory(proc,(LPCVOID)addr,&buffer,n,NULL)>0)
	{
		for(i=0;i<n;i++)
		{
			//printf("\\x%02x",buffer[i] & 0xFF);
			instruction_buffer[i]=buffer[i] & 0xFF;
		}
		//printf("\n\n");
	}

	if(cs_open(CS_ARCH_X86,CS_MODE_64,&handle) != CS_ERR_OK)
	{
		printf("cs erro\n");
	}else{
		count = cs_disasm(handle,instruction_buffer,n,0x1000,0,&insn);
		if(count>0)
		{
			printf("%s %s\n",insn[0].mnemonic,insn[0].op_str);
		}else{
			printf("disasm error\n");
		}
	}

}

void do_disasm(UINT64 last,UINT64 current)
{
	uint8_t instruction_buffer[1024];
	cs_insn *insn;
	size_t count;
	csh handle;
	int i;
	UINT64 n=current-last;
	char buffer[1024];
	printf("should disasm from %llx to %llx\n",last,current);
	if(ReadProcessMemory(proc,(LPCVOID)last,&buffer,n,NULL)>0)
	{
		for(i=0;i<n;i++)
		{
			//printf("\\x%02x",buffer[i] & 0xFF);
			instruction_buffer[i]=buffer[i] & 0xFF;
		}
		//printf("\n\n");
	}

	if(cs_open(CS_ARCH_X86,CS_MODE_64,&handle) != CS_ERR_OK)
	{
		printf("cs error\n");
	}else{
		count = cs_disasm(handle,instruction_buffer,n,0x1000,0,&insn);
		if(count>0)
		{
			size_t j;
			for(j=0;j<count;j++)
			{
				printf("%s %s\n",insn[j].mnemonic,insn[j].op_str);
			}
			disasm_one_instr(current);
		}else{
			printf("disasm error\n");
		}
	}
}
