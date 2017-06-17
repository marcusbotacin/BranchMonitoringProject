/* Branch Monitoring Project
 * Debugger Client
 * Marcus Botacin
 * UNICAMP - 2017
 */

#include "includes.h"
#include "print.h"

/* print a line on the console */
void print_line(int size)
{
	for(int pos=0;pos<size;pos++)
	{
		printf(LINE_CHAR);
	}
	return;
}

/* print DBG logo */
void print_head()
{
	int first,last;
	char logo[]=" VoiD - The VOID Debugger ";
	for(int i=0;i<N_LINES;i++)
	{
		/* First line is the name */
		if(i==1)
		{
			/* calculate number of prints based on logo msg size */
			first=(N_COLUMNS-strlen(logo))/2;
			last=N_COLUMNS-strlen(logo)-first;
			/* line before */
			print_line(first);
			/* display dbg name in the center */
			printf("%s",logo);
			/* line after */
			print_line(last);
			printf("\n");
		}else{
			/* others, just a line */
			print_line(N_COLUMNS);
			printf("\n");
		}
	}
	return;
}

/* print debug functionalities */
void debug_options()
{
	printf(	"\t(h)elp\n"
			"\t(r)esume\n"
			"\t(s)uspend\n"
			"\t(n)ext branch\n"
			"\t(q)uit\n"
			"\t(d)ump memory\n"
			"\t(D)isasm\n"
			"\t(N)ame\n"
			"\t(l)oaded libs\n"
			"\t(b)reakpoint\n"
			"\t(c)ontinue\n"
			"\t(i)ntrospect from\n"
			"\t(I)ntrospect to\n"
			"\t(R)egister values\n"
			"\t(C)ontext\n");
}

/* DBG console/prompt message */
void console_message(char *msg)
{
	printf("> %s\n",msg);
}

/* display unimplemented feature message */
void unimplemented()
{
	console_message("unimplemented feature");
}