/* BranchMonitoring Project 
 * Debugger Server-side
 * Marcus Botacin
 * UNICAMP - 2017
 */

/* general includes */
#include "includes.h"
/* data types defs */
#include "data.h"
/* I/O control */
#include "inverted_ioctl.h"
/* print config */
#include "print.h"

/* Send the I/O request which will be completed in the future */
DWORD SendTheIoctl(POVL_WRAPPER Wrap, HANDLE driverHandle)
{
	DWORD code;

	DeviceIoControl(driverHandle,
		(DWORD)IOCTL_VOIDBG_INVERT_NOTIFICATION,
		DEFAULT_IO_IN_BUFFER,                       // Ptr to InBuffer
		DEFAULT_IO_IN_BUFFER_SIZE,                  // Length of InBuffer
		&Wrap->ReturnedSequence,					// Ptr to OutBuffer
		sizeof(LONG),								// Length of OutBuffer
		DEFAULT_IO_RESULTING_BYTES,                 // BytesReturned
		&Wrap->Overlapped);							// Ptr to Overlapped structure

	/* Get IOCTL code */
	code = GetLastError();

	/* return IOCTL code */
	return code;
}

/*Send current monitored PID to be filtered to the kernel driver */
DWORD SendPID(int *monitored_pid,POVL_WRAPPER Wrap, HANDLE driverHandle)
{
	DWORD code;

	DeviceIoControl(driverHandle,
		(DWORD)IOCTL_VOIDBG_SEND_PID,
		monitored_pid,							// Ptr to InBuffer
		sizeof(int),							// Length of InBuffer
		&Wrap->ReturnedSequence,				// Ptr to OutBuffer
		sizeof(LONG),							// Length of OutBuffer
		DEFAULT_IO_RESULTING_BYTES,             // BytesReturned
		&Wrap->Overlapped);						// Ptr to Overlapped structure

	/* Get IOCTL code */
	code = GetLastError();

	/* return IOCTL code */
	return code;
}

/* get the branch data from the kernel driver */
DWORD getdata(HANDLE driverHandle,POVL_WRAPPER2 Wrap)
{
	DWORD code;
	DeviceIoControl(driverHandle,
		(DWORD)IOCTL_VOIDBG_GET_DATA,
		DEFAULT_IO_IN_BUFFER,                   // Ptr to InBuffer
		DEFAULT_IO_IN_BUFFER_SIZE,              // Length of InBuffer
		&Wrap->bd,								// Ptr to OutBuffer
		sizeof(BRANCHDATA),						// Length of OutBuffer
		DEFAULT_IO_RESULTING_BYTES,             // BytesReturned
		&Wrap->Overlapped);						// Ptr to Overlapped structure

	/* get IOCTL code */
	code = GetLastError();

	/* debug print */
#ifdef ENABLE_DBG_PRINT
	printf("%x %x",Wrap->bd.FROM,Wrap->bd.TO);
#endif

	/* return IOCTL code */
	return code;
}

/* Retrieves register values from the kernel driver */
DWORD getregs(HANDLE driverHandle,POVL_WRAPPER3 Wrap)
{
	DWORD code;
	DeviceIoControl(driverHandle,
		(DWORD)IOCTL_VOIDBG_GET_REG,
		DEFAULT_IO_IN_BUFFER,							// Ptr to InBuffer
		DEFAULT_IO_IN_BUFFER_SIZE,						// Length of InBuffer
		&Wrap->data,									// Ptr to OutBuffer
		sizeof(REGS),									// Length of OutBuffer
		DEFAULT_IO_RESULTING_BYTES,						// BytesReturned
		&Wrap->Overlapped);								// Ptr to Overlapped structure

	/* I/O Control code */
	code = GetLastError();

	/* debug print */
#ifdef ENABLE_DBG_PRINT
	printf("%x %x",Wrap->data.cr0,Wrap->data.cr1);
#endif

	/* return the IOCTL code */
	return code;
}