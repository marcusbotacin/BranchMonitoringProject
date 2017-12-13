/* Branch Monitor - Multi-Core version
 * Marcus Botacin - 2017
 * Federal University of Paraná (UFPR)
 */

#include "config.h"

/* define which action will be performed on a given thread */
enum thread_action
{
	LOAD_BTS,
	UNLOAD_BTS
};

void control_thread(int action, int core);
VOID ThreadStart(_In_ PVOID StartContext);
VOID ThreadStop(_In_ PVOID StartContext);

void thread_attach_to_core(unsigned id);