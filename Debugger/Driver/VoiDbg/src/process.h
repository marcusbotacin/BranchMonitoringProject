/* VoiDbg - The VOID Debugger
 * A Debugger based on the BranchMonitor framework
 * Marcus Botacin - 2017
 */

/* constant defs */
#define INIT_NO_PID 0

/* External function import */
extern NTSTATUS ZwQueryInformationProcess(
  _In_       HANDLE ProcessHandle,
  _In_       PROCESSINFOCLASS ProcessInformationClass,
  _Out_      PVOID ProcessInformation,
  _In_       ULONG ProcessInformationLength,
  _Out_opt_  PULONG ReturnLength
);