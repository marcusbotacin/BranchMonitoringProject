/* Branch Monitor 
 * Introspection Client
 * Marcus Botacin, 2017
 */

#include "config.h"
#include "addresses.h"
#include "parse.h"

/* Usage Information */

void Usage(char *argv[])
{
	printf("Usage: %s -bin <name> <pid> <base> <size> -lib <name> <base> <size>\n",argv[0]);
	exit(0);
}


/* Parser Inputs:
	binary: monitored binary info -- same struct as lib, but a dedicated variable -- since only one binary is monitored at once
	libs: vector of linked libraries to the monitored binary
	argc,argv: main inputs
	lib_count: return the number of linked libraries parsed
	pid: return the identified PID to be monitored
	*/

int parse(pimgs binary,pimgs libs,int argc,char *argv[],int *lib_count,int *pid)
{
	int status=NO_BINARY;
	int i;

	/* Minimum number of args is -bin ... */
	if(argc<6)
	{
		Usage(argv);
	}

	/* traverse arg list */
	for(i=1;i<argc;i+=4)
	{
		/* Case binary */
		if(strcmp(argv[i],"-bin")==0)
		{
			binary->name=argv[i+1];
			*pid=atoi(argv[i+2]);
			binary->addr_min=_strtoui64_l(argv[i+3],NULL,10,NULL);
			binary->addr_max=_strtoui64_l(argv[i+4],NULL,10,NULL);
			binary->addr_max+=binary->addr_min;
			i++; /* binary has an additional arg (PID), so sync is required */
			status=FOUND_NO_ERROR;
		}
		/* Case libs */
		if(strcmp(argv[i],"-lib")==0)
		{
			libs[*lib_count].name=argv[i+1];
			libs[*lib_count].addr_min=_strtoui64_l(argv[i+2],NULL,10,NULL);
			libs[*lib_count].addr_max=_strtoui64_l(argv[i+3],NULL,10,NULL);
			libs[*lib_count].addr_max+=libs[*lib_count].addr_min;
			(*lib_count)++;
			status=FOUND_NO_ERROR;
		}
	}

	return status;
}