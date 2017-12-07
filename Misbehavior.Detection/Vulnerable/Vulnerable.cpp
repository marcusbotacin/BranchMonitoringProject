// BranchMonitoring Framework
// Application Misbehavior Detection
// Vulnerable Application Example
// Marcus Botacin - 2017
// Federal University of Paraná (UFPR)

#include<stdio.h>
#include<string.h>

// size which will be overwritten
#define MAX_STRING sizeof(int)

int main(int argc, char *argv[])
{
    // note that variable order in stack is important
	char string[MAX_STRING];
	int loop=0;
	int opt=0;
	
	do{
        // read a number
		scanf("%d",&opt);
		if (opt>0)
		{
            // if greater than zero
			printf("Greater than zero\n");
		}else if(opt<0)
		{
            // if smaller than zero
			printf("Smaller than zero\n");
            // the zero case
		}else{
			printf("Bad choice\n");
			// overflowing string changes loop var
			scanf("%s",string);
			
		}
	}while(!loop);
    // loop while this variable is zero
	
    // this part should never be executed, since var loop should not change
	printf("Should never be executed\n");

	return 0;
}
