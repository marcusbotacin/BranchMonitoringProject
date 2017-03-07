/* Branch Monitor 
 * Introspection Client
 * Marcus Botacin, 2017
 */

#include "introspection.h"

/* Function Offsets for the wow64.dll */
/* Wow64 is required for running x86 under x64 */

static FUNCTION_INTROSPECTION wow64[]={
{"Wow64PrepareForException",194976},
{"Wow64EmulateAtlThunk",52744},
{"Wow64CheckIfNXEnabled",52392},
{"Wow64PrepareForDebuggerAttach",50892},
{"Wow64SystemServiceEx",49804},
{"Wow64SystemService",49784},
{"Wow64FreeThreadHeap",49628},
{"Wow64AllocThreadHeap",49580},
{"Wow64FreeHeap",49348},
{"Wow64AllocateHeap",49316},
{"Wow64LdrpInitialize",46272},
{"Wow64SuspendLocalThread",46256},
{"Wow64AllocateTemp",42996},
{"Wow64ShallowThunkSIZE_T32TO64",42608},
{"Wow64ShallowThunkSIZE_T64TO32",42576},
{"Wow64ShallowThunkAllocObjectAttributes32TO64_FNC",42324},
{"Wow64ShallowThunkAllocSecurityQualityOfService32TO64_FNC",42140},
{"Wow64RaiseException",38508},
{"Wow64StartupContextToContextX86",37268},
{"Wow64LogPrint",33136},
{"Wow64GetWow64ImageOption",32648},
{"Wow64OpenConfigKey",32628},
{"Wow64KiUserCallbackDispatcher",31960},
{"Wow64ApcRoutine",31684},
{"Unknown",0},
};