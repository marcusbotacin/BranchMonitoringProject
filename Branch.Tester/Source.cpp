/* Branch Test
 * Exercise many kinds of branch
 * Marcus Botacin, 2017 
 */

#include<stdio.h>
#include<Windows.h>

#define MAX 10
#define SLEEP_TIME 5000
#define BTS_CORE 3

BOOL attach_to_core()
{
	HANDLE proc = GetCurrentProcess();
	return SetProcessAffinityMask(proc,1<<BTS_CORE);
}

int main()
{
	int i;

	/* since my BTS implementation is not multicore yet */
	if(attach_to_core()!=TRUE)
	{
		printf("Attach Failed. Finishing...\n");
		return 0;
	}

	/* FOR: Forward and backward edges */
	for(i=0;i<MAX;i++)
	{
		/* conditional jmp */
		if(i%2==0)
		{
			/* CALL + RET */
			printf("I'm alive\n");
		}else{
			Sleep(SLEEP_TIME);
		}
	}

	return 0;
}