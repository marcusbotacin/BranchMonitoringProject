/* Branch Monitor 
 * Introspection Client
 * Marcus Botacin, 2017
 */

#include "signal.h"

extern BOOL run;

void SignalHandler(int signal)
{
	if (signal == SIGINT) {
		printf("SIGINT\n");
		/* disable executing */
		/* finish gracefully */
		run=FALSE;
	}
}