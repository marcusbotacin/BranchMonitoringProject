/* BranchMonitoring Project 
 * Debugger Server-side
 * Marcus Botacin
 * UNICAMP - 2017
 */

/* General Includes */
#include "includes.h"
/* Screen definitions */
#include "print.h"

/* Just print a line on the screen
 * Used to split messages
 */
void print_line(int size)
{
	/* for all positions in a given line */
	for(int pos=0;pos<size;pos++)
	{
		printf(PRINT_CHAR);
	}
	return;
}

/* print debugger logo */
void print_head()
{
	int first,last;
	/* debugger logo/message */
	char logo[]=" VoiDbg - The VOID Debugger ";
	for(int i=0;i<N_LINES;i++)
	{
		/* first line */
		if(i==1)
		{
			first=(N_COLUMNS-strlen(logo))/2;
			last=N_COLUMNS-strlen(logo)-first;
			print_line(first);
			printf("%s",logo);
			print_line(last);
			printf("\n");
		/* Other lines */
		}else{
			print_line(N_COLUMNS);
			printf("\n");
		}
	}
	return;
}

/* Just print a message on the console */
void console_message(char *msg)
{
	printf("[%s] %s\n",DBG_PRINT_NAME,msg);
}

void console_message_with_code(char *msg,int code)
{
	printf("[%s] %s:%x\n",DBG_PRINT_NAME,msg,code);
}