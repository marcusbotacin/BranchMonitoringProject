/* Branch Monitor
 * Marcus Botacin
 * 2017
 */

#include "config.h"

#define SYSTEM_WIDE -1

/* define which action will be performed on a given thread */
enum thread_action
{
	LOAD_BTS,
	UNLOAD_BTS,
	LOAD_WORKER,
	UNLOAD_WORKER,
};

void control_thread(int action, int core);
VOID ThreadStart(_In_ PVOID StartContext);
VOID ThreadStop(_In_ PVOID StartContext);
VOID WorkerThread(_In_ PVOID StartContext);

void thread_attach_to_core(unsigned id);