/* VoiDbg - The VOID Debugger
 * A Debugger based on the BranchMonitor framework
 * Marcus Botacin - 2017
 */

/* avoid multiple includes */
#pragma once

/* Default user messages */

#define GET_OUT_BUFFER_ERROR "Cannot get output buffer"
#define GET_IN_BUFFER_ERROR "Cannot get input buffer"
#define NO_CACHED_IO "No cached I/O request"
#define INVALID_IOCTL "Received an invalid IOCTL"
#define IOCTL_DISPATCH "Entering IOCTL Dispatch"
#define SEND_PID_MSG "Called SEND_PID IOCTL"
#define PID_OK_MSG "PID Received"
#define GET_REG_MSG "Called GET_REG IOCTL"
#define BUFFER_SIZE_ERROR "Buffer has not enough size"
#define GET_DATA_MSG "Called GET_DATA IOCTL"
#define BRANCH_DATA_MSG "Returning Branch Data"
#define CACHE_REQUEST "Asked to cache i/o request"
#define CACHE_REQUEST_ERROR "Cannot cache i/o request"