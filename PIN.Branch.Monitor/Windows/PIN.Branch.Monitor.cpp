/* Branch Monitoring Project 
 * Marcus Botacin - UNICAMP - 2017
 * PIN implementation of Branch Monitor
 * Used for simple comparisons and validation purposes
 * Inspired on PIN example: instcount0.cpp
*/

/* Include Block */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "pin.H"

/* Output file */
ofstream OutFile;

/* Last Branch Address Record */
void* last=0;

/* Print Direct Branches 
 * Direct Branches -> we can retrieve target address without indirect calculation
 * Just print Source and Target addresses and branch instruction disasm
*/

VOID print_direct_branch(VOID *Source, string * Target, string * instr)
{
	OutFile << "From: " << Source << " To: "<< *Target << " Disasm of 1 instr: " << *instr << endl;
}

/* Print Indirect Branches 
 * Can't calculate target address without additional effort
 * Return target could be calculated by using a Call Stack
 * Mostly used for RET instructions
 * Print Source and instr disasm without target address
*/

VOID print_indirect_branch(VOID *Source, string * str)
{
	OutFile << "From: " << Source << " Disasm of 1 instr: " << *str << endl;
}

/* string_to_number
 * Convert string* to a pointer
 * Aux. function
*/

void* string_to_number(string * s)
{
	return reinterpret_cast<void*>(_strtoul_l(s->c_str(),NULL,0,0));
}

/* Given a buffer of instruction bytes, print them properly
 * byte buffer: values
 * number of bytes: n
*/

# define BYTEMASK 0xFF /* Mask to capture only one byte */
# define STARTING_VALUES 0 /* initial block and bytes */
# define NEXT_BYTE 8 /* bit shift */

VOID PrintBytes(ADDRINT *values, size_t n)
{
	size_t i=STARTING_VALUES; /* number of printed bytes */
	int block=STARTING_VALUES; /* byte block index -- each 8 bytes are stored on a single ADDRINT block entry */

	/* fist byte block */
	UINT64 byte = values[block];

	/* each one of 8 bytes on a given block */
	int inner_byte = STARTING_VALUES;

	/* byte to be printed */
	UINT64 pbyte;


	pbyte = byte & BYTEMASK;
	OutFile << "0x"<<hex<<pbyte;
	byte = byte >> NEXT_BYTE;
	inner_byte++;
	i++;

	while(i<n)
	{
		if(inner_byte<8)
		{
			pbyte = byte & BYTEMASK;
			OutFile << " 0x"<<hex<<pbyte;
			byte = byte >> NEXT_BYTE;
			inner_byte++;
			i++;
		}else
		{
			inner_byte=STARTING_VALUES;
			block++;
			byte = values[block];
		}
	}
	OutFile<<endl;
}

/* Disasm instruction block
 * starting at last address
 * until the current addr
*/

#define MAXIMUM_BYTES 0x30
#define MINIMUM_BYTES 0

VOID Disasm(void *last, void *current)
{
	/* Block size calculation */
	UINT64 bytes = (UINT64)current - (UINT64)last;
	size_t n;

	/* Disasm only small blocks/gadgets */
	if(bytes > MINIMUM_BYTES && bytes < MAXIMUM_BYTES)
	{
		OutFile << "Disasm of 0x" << bytes << " bytes from " << last << ": ";
		/* Read memory to retrieve executed code (instruction bytes) */
		ADDRINT values[MAXIMUM_BYTES];
		/* if successfully read */
		if((n=PIN_SafeCopy(&values, last, bytes))>0)
		{
			/* print instruction bytes */
			PrintBytes(values,n);
		}else{
			/* Otherwise, just print error message */
			OutFile << "Disasm error" << endl;
		}
	}
}

/* handle direct branches
 * perform disasm + print
*/

VOID handle_direct_branch(VOID *Source, string * Target, string * instr)
{
	void* current;
	current = Source;
	/* print branch information */
	print_direct_branch(Source, Target, instr);
	/* Disasm previously executed block
	 * starting on the previous ''last'' address
	 * until the ''current'' address
  	*/
	Disasm(last,current);
	/* set the new ''last'' branch */
	last = string_to_number(Target);
}

/* Split string 
 * Aux. function
 * Split into whitespace
 * Expected input: Instr' 'address
 * Expected Output: Instr on instr, address on addr string
*/

VOID split(string dis, string * instr, string * addr)
{
	/* making string to be a stream */
	istringstream iss(dis);
	string sub;
	/* Get instr */
	iss >> sub;
	*instr = sub;
	/* Get address */
	iss >> sub;
	*addr = sub;
}


/* Instrumentation function */
VOID Instruction(INS ins, VOID *v)
{
	/* Only branch instructions, since we are emulating the branch monitor */
	if(INS_IsBranchOrCall(ins))
	{
		/* Direct Branch */
		if(INS_IsDirectBranchOrCall(ins))
		{
			/* I don't want to handle all branch types to get PIN parameters
			 * PIN could provide branch target through IARGS
			 * I opt for disasming the branch instruction and retrieving the address from the string
			*/

			/* Branch instruction disasm */
			string dis = INS_Disassemble(ins);

			/* Spliting string into Instruction+Address */
			string instr,addr;
			split(dis,&instr,&addr);

			/* Call handle_branch function with the proper parameters */
			INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)handle_direct_branch, IARG_INST_PTR, IARG_PTR, new string(addr), IARG_PTR, new string(instr), IARG_END);
		}
		/* Indirect Branches */	
		else{
			/* Just call indirect_print */
			INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)print_indirect_branch, IARG_INST_PTR, IARG_PTR, new string(INS_Disassemble(ins)), IARG_END);
		}
	}

}


/* Original parameters */
KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
		"o", "inscount.out", "specify output file name");

/* At the end, close file */
VOID Fini(INT32 code, VOID *v)
{
	OutFile.close();
}

/* Entry Point */
int main(int argc, char * argv[])
{
	/* Initialize pin, no args except the binary */
	if (PIN_Init(argc, argv)) return -1;

	/* Open log file */
	OutFile.open(KnobOutputFile.Value().c_str());

	/* instruction-level instrumentation */
	INS_AddInstrumentFunction(Instruction, 0);

	/* Application exit callback */
	PIN_AddFiniFunction(Fini, 0);

	/* Start the program */
	PIN_StartProgram();

	return 0;
}
