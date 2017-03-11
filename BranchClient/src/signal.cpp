/* Branch Monitor 
 * Introspection Client
 * Marcus Botacin, 2017
 */

#include "signal.h"

extern BOOL run;

/* Handle signal interruptions */
void SignalHandler(int signal)
{
	/* Handle SIGINT (CTRL+C) to finish gracefully */
	if (signal == SIGINT) {
		printf("SIGINT\n");
		/* disable execution */
		/* finish gracefully */
		run=FALSE;
	}
}