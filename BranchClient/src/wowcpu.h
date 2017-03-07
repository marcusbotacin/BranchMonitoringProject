/* Branch Monitor 
 * Introspection Client
 * Marcus Botacin, 2017
 */

#include "introspection.h"

/* Kernel Offsets fot the Wow64cpu.dll */
/* Wow64 is required for running x86 under x64 */

static FUNCTION_INTROSPECTION wow64cpu[]={
{"TurboDispatchJumpAddressEnd",9628},
{"TurboDispatchJumpAddressStart",9619},
{"CpuSimulate",9312},
{"CpuSuspendLocalThread",6528},
{"CpuNotifyAffinityChange",6516},
{"CpuNotifyAfterFork",6516},
{"CpuNotifyBeforeFork",6516},
{"CpuNotifyDllLoad",6516},
{"CpuPrepareForDebuggerAttach",6516},
{"CpuResetFloatingPoint",6516},
{"CpuProcessDebugEvent",6504},
{"CpuFlushInstructionCache",6472},
{"CpuNotifyDllUnload",6444},
{"CpuSetInstructionPointer",6412},
{"CpuSetStackPointer",6380},
{"CpuGetStackPointer",6348},
{"CpuSetContext",6200},
{"CpuGetContext",6108},
{"CpuResetToConsistentState",5828},
{"CpuThreadInit",5636},
{"CpuProcessTerm",5624},
{"CpuSuspendThread",5624},
{"CpuThreadTerm",5624},
{"CpuProcessInit",5000},
{"CpuInitializeStartupContext",4548},
{"Unknown",0},
};
