/* Marcus Botacin
 * 2017 - UNICAMP
 * BranchMonitor Transparency Tests
 * Check remote debugger presence
 */

#include<stdio.h>
#include<Windows.h>

/* Will run on windows 7 or newer */
#define WIN7

/* OutputDbgString.eax for each system */
#ifdef WINXP
	#define DEBUGGED 1
#else if WIN7
	#define DEBUGGED 0
#endif

#define OUTPUT_MSG "Test"

int main()
{
	unsigned int result;
	
	/* Call function */
	OutputDebugStringA(OUTPUT_MSG);

	/* Get eax value */
	__asm {
		mov result, eax;
	}
	
	/* Check debugged or not */
	if(result!=DEBUGGED)
	{
		printf("Debugged\n");
	}else{
		printf("Not Debugged\n");
	}
	
	return 0;
}