/* VoiDbg - The VOID Debugger
 * A Debugger based on the BranchMonitor framework
 * Marcus Botacin - 2017
 */

/* Include Libraries */
#include <ntddk.h> 
#include <wdf.h>

/* I/O defs */
#include "inverted_ioctl.h"

/* Register struct */
typedef struct st_reg
{
	UINT64 cr0,cr1,cr2,cr3,cr4;
}REGS,*PREGS;

/* Prototypes */

DRIVER_INITIALIZE DriverEntry;

DECLARE_CONST_UNICODE_STRING(
INVERTED_DEVICE_PROTECTION,
    L"D:P(A;;GA;;;SY)(A;;GRGWGX;;;BA)(A;;GRGWGX;;;WD)(A;;GRGWGX;;;RC)");
extern const UNICODE_STRING  INVERTED_DEVICE_PROTECTION;

typedef struct _INVERTED_DEVICE_CONTEXT  {
    WDFQUEUE    NotificationQueue;
    LONG       Sequence;
} INVERTED_DEVICE_CONTEXT, * PINVERTED_DEVICE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(INVERTED_DEVICE_CONTEXT, InvertedGetContextFromDevice)

DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD InvertedEvtDeviceAdd;
EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL InvertedEvtIoDeviceControl;

VOID InvertedNotify(PINVERTED_DEVICE_CONTEXT DevContext);