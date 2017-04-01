/* Marcus Botacin
 * 2017 - UNICAMP
 * BranchMonitor Transparency Tests
 * Check remote debugger presence
 */

#include<stdio.h>
#include<Windows.h>

int main()
{
	BOOL result;
	/* Check wether the process itself is being debugged or not */
	CheckRemoteDebuggerPresent(GetCurrentProcess(),&result);
	if(result)
	{
		printf("Debugged\n");
	}else{
		printf("Not Debugged\n");
	}
	return 0;
}