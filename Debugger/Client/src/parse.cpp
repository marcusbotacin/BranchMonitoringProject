/* Branch Monitoring Project
 * Debugger Client
 * Marcus Botacin
 * UNICAMP - 2017
 */

/* General Includes */
#include "includes.h"
/* Parser defs */
#include "parse.h"
/* Option defs */
#include "options.h"

/* Parse function args */
bool parse(int argc,char *argv[],char **sample,char **port,char **addr,char **pid)
{
	int i=1; /* ignore argv[0] */
	bool sflag,pflag,aflag,pidflag;

	/* init with no argument read */
	sflag=pflag=aflag=pidflag=false;

	/* caso no args supplied */
	if(argc<MIN_ARGS)
	{
		return false;
	}

	/* iterate over all args */
	while(i<argc)
	{
		if(strncmp(argv[i],SAMPLE_OPTION,MAX_STRING)==EQUALS)
		{
			*sample=argv[i+1];
			sflag=true;
		}
		if(strncmp(argv[i],PORT_OPTION,MAX_STRING)==EQUALS)
		{
			*port=argv[i+1];
			pflag=true;
		}
		if(strncmp(argv[i],PID_OPTION,MAX_STRING)==EQUALS)
		{
			*pid=argv[i+1];
			pidflag=true;
		}
		if(strncmp(argv[i],ADDR_OPTION,MAX_STRING)==EQUALS)
		{
			*addr=argv[i+1];
			aflag=true;
		}
		/* skip argument */
		i+=PARSER_STEP;
	}

	/* all flags properly set */
	if((pidflag==true || sflag==true) && pflag==true && aflag==true)
	{
		return true;
	}else{
		return false;
	}
}

/* Display usage message */
void usage(char *argv[])
{
	printf("Usage: %s -s <sample> -p <port> -a <addr>\n",argv[0]);
	printf("Usage: %s -pid <pid> -p <port> -a <addr>\n",argv[0]);
}
