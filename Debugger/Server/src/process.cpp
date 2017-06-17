/* BranchMonitoring Project 
 * Debugger Server-side
 * Marcus Botacin
 * UNICAMP - 2017
 */

/* General Includes */
#include "includes.h"
/* Process utils */
#include "process.h"
/* print utils */
#include "print.h"

/* Launch a process in the suspended state
 * Useful for inspecting at startup
 * and also for instrumenting before execution starts
 * can also be used for retrieving PID at beginning
 */
int launch_suspended_process(char *path)
{
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si,sizeof(si));
	si.cb=sizeof(si);
	ZeroMemory(&pi,sizeof(pi));
	if(!CreateProcessA(path,					/* Executable path */
		PROC_NO_CMDLINE,
		PROC_NO_ATTR,
		THREAD_NO_ATTR,
		PROC_NO_INHERIT,
		CREATE_NEW_CONSOLE|CREATE_SUSPENDED,	/* Suspend Flag */
		PROC_ENV,
		PROC_DIR,
		&si,
		&pi))
	{
		/* Case ERROR */
		console_message_with_code("Launch Malware Error",GetLastError());
		return 0;
	}else{
		/* Case SUCCESS */
		/* debug print */
		console_message("success");
	}

	/* PID as return */
	return pi.dwProcessId;
}

/* Suspend a running process
 * Processes must be in the suspended state
 * in order to inspections be consistent
 * First Method - Enum and suspend all threads
 * May result on deadlock
 */
void suspend(DWORD processId)
{
	/* Enum all threads */
	HANDLE hThreadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, ALL_PIDS);

	THREADENTRY32 threadEntry; 
	threadEntry.dwSize = sizeof(THREADENTRY32);

	/* Get first system thread */
	Thread32First(hThreadSnapshot, &threadEntry);

	/* Iterates over all threads */
	do
	{
		/* If thread belongs the PID we are interested */
		if (threadEntry.th32OwnerProcessID == processId)
		{
			/* Get thread handler */
			HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE,
				threadEntry.th32ThreadID);
			/* Suspend */
			SuspendThread(hThread);
			/* Close Handle */
			CloseHandle(hThread);
		}
	} while (Thread32Next(hThreadSnapshot, &threadEntry));

	/* Destroy Snapshot */
	CloseHandle(hThreadSnapshot);
}

/* Another method
 * Undocumented
 */
void suspend2(DWORD processId)
{
	/* Get process handler */
	HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);

	/* Get undocumented/unexported function */
	NtSuspendProcess pfnNtSuspendProcess = (NtSuspendProcess)GetProcAddress(
		GetModuleHandle(L"ntdll"), "NtSuspendProcess");

	/* Execute function */
	pfnNtSuspendProcess(processHandle);

	/* close process handler */
	CloseHandle(processHandle);
}

/* Resume a suspended process */
void resume2(DWORD processId)
{
	/* get process handle */
	HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);

	/* get unexported function */
	NtResumeProcess pfnNtResumeProcess = (NtSuspendProcess)GetProcAddress(
		GetModuleHandle(L"ntdll"), "NtResumeProcess");

	/* run function */
	pfnNtResumeProcess(processHandle);

	/* close proc handler */
	CloseHandle(processHandle);
}