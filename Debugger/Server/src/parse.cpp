/* BranchMonitoring Project 
 * Debugger Server-side
 * Marcus Botacin
 * UNICAMP - 2017
 */

#include "includes.h"
#include "parse.h"

/* display usage message */
void usage(char *argv[])
{
	printf("Usage: %s -p <port> (optional)\n",argv[0]);
}

/* parse arguments */
bool Parse(int argc,char *argv[],char **port)
{
	/* case no arguments passed */
	if(argc==1)
	{
		/* will run using default configs */
		return true;
	}
	/* case min number of args */
	if(argc==3)
	{
		/* get port number */
		if(strcmp(argv[1],"-p")==EQUAL)
		{
			*port=argv[2];
			return true;
		}
	}
	/* any other case, failure */
	return false;
}