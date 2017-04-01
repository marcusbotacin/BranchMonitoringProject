/* Marcus Botacin
 * 2017 - UNICAMP
 * BranchMonitor Transparency Tests
 * Check debugger presence
 */

#include<stdio.h>
#include<Windows.h>

int main()
{
	/* Just check API return */
	if(IsDebuggerPresent())
	{
		printf("Debugged\n");
	}else{
		printf("Not Debugged\n");
	}
	return 0;
}