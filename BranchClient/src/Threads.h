/* Branch Monitor 
 * Introspection Client
 * Marcus Botacin, 2017
 */

#include "config.h"

/* Two kind of threads: Producer and Consumer
 * Producer -- reads driver data and put on a queue
 * Producer must be a dedicated thread due to throughput issues
 * Consumer - Introspect on collected data
 * Only 1 Consumer thread -- due to ordering issues
 */

DWORD WINAPI ThreadProducer(_In_ LPVOID lpParameter);
DWORD WINAPI ThreadConsumer(_In_ LPVOID lpParameter);