/* VoiDbg - The VOID Debugger
 * A Debugger based on the BranchMonitor framework
 * Marcus Botacin - 2017
 */

/* Warning, this driver does not have a routine for real time unloading */

/* General Includes */

#include "Driver.h"			/*	WDF defs				*/
#include "BranchMonitor.h"	/*	Driver defs				*/
#include "process.h"		/*	process handling defs	*/
#include "BTS.h"			/*	BTS data types defs		*/
#include "debug.h"			/*	Driver debug defs		*/
#include "IO.h"				/*	I/O defs				*/
#include "messages.h"		/*	Default prints defs		*/

/* global variables */

/* thread handler */
HANDLE thandle;

/* declare unmapped APIC pointer */
UINT32* APIC=NULL;

/* Interrupt Request Queue (copy) */
WDFQUEUE Queue2;

/* Branch Data */
ULONG64 FROM,TO;

/* PID Filtering */
int monitored_pid=INIT_NO_PID;

/* ISR handler */
PVOID nmicallbackhandler;

/* BTS Buffers
 * They are vectors, indexed by the core_id,
 * since you can set up the debbuger on any core
 */
PTDS_BASE DS_BASE[MAX_PROC];
PTBTS_BUFFER BTS_BUFFER[MAX_PROC];

/* Buffers Setup Routine */
void FILL_DS_WITH_BUFFER(PTDS_BASE DS_BASE,PTBTS_BUFFER BTS_BUFFER)
{
	/* BTS BUFFER BASE */
	DS_BASE->BUFFER_BASE=BTS_BUFFER;
	/* BTS INDEX */
	DS_BASE->BTS_INDEX=BTS_BUFFER;
	/* BTS MAX */
	DS_BASE->MAXIMUM=(PTBTS_BUFFER)(((UINT_PTR)BTS_BUFFER)+(SIZE_BTS_BUFFER*sizeof(TBTS_BUFFER)));
	/* BTS Threshold */
	DS_BASE->THRESHOLD=(PTBTS_BUFFER)(((UINT_PTR)BTS_BUFFER)+(THRESHOLD_BTS_BUFFER*sizeof(TBTS_BUFFER)));
}

/* Interrupt Routine */
BOOLEAN ISRNmiCallback(IN PVOID Context, IN BOOLEAN Handled)
{	
	PINVERTED_DEVICE_CONTEXT devContext;
	HANDLE pid;
	UNREFERENCED_PARAMETER(Context);
	UNREFERENCED_PARAMETER(Handled);
		
	/* clear APIC interrupt flag */
	set_apic(APIC_INT_VALUE);
	
	/* PID filtering */
	pid=PsGetCurrentProcessId();
	#pragma warning( disable : 4047 )
	if(monitored_pid!=INIT_NO_PID && (UINT64)monitored_pid==(UINT64)pid)
	{
		
		/* GET BTS Data */
		FROM=BTS_BUFFER[BTS_CORE][DBG_DATA_ENTRY].FROM;
		TO=BTS_BUFFER[BTS_CORE][DBG_DATA_ENTRY].TO;

		/* Fire inverted I/O */
		devContext = InvertedGetContextFromDevice(WdfIoQueueGetDevice(Queue2));
		InvertedNotify(devContext);
	}
			
	/* re-set buffer */
	FILL_DS_WITH_BUFFER(DS_BASE[BTS_CORE],BTS_BUFFER[BTS_CORE]);

	/* re-enabling BTS */
	__writemsr(MSR_DEBUGCTL,BTS_INTERRUPT_FLAGS);

	return TRUE;
}

/* Driver Entry Point */
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
    WDF_DRIVER_CONFIG config;
    NTSTATUS status;

    dbg_print("Entry Point");

    WDF_DRIVER_CONFIG_INIT(&config, InvertedEvtDeviceAdd);

    status = WdfDriverCreate(DriverObject,RegistryPath,WDF_NO_OBJECT_ATTRIBUTES,&config,WDF_NO_HANDLE);

	/* error case */
    if (!NT_SUCCESS(status)) {
        dbg_print("WdfDriverCreate failed");
    }

    return status;
}

/* attach thread to a given core */
void thread_attach_to_core(unsigned id)
{
	KAFFINITY mask;
#pragma warning( disable : 4305 )
#pragma warning( disable : 4334 )
	mask=1<<id;
	KeSetSystemAffinityThread(mask);
}

/* delay execution for a while */
void delay_exec(INT64 interval)
{
	LARGE_INTEGER _interval;
	_interval.QuadPart=interval;
	KeDelayExecutionThread(KernelMode,FALSE,&_interval);
}

/* map APIC */
void map_apic()
{
	PHYSICAL_ADDRESS pa;
	pa.QuadPart=PERF_COUNTER_APIC;
	APIC=(UINT32*)MmMapIoSpace(pa,sizeof(UINT32),MmNonCached);
	if(APIC!=NULL)
	{
			dbg_print("APIC mapped");
			*APIC=0x400;
			
	}
	else{
		dbg_print("APIC map error");
	}
}

/* unmap apic */
void unmap_apic()
{
	/* already mapped */
	if(APIC!=NULL)
	{
		MmUnmapIoSpace(APIC,sizeof(UINT32));
		dbg_print("APIC was unmapped");
	}
}

/* set APIC value */
void set_apic(INT32 value)
{
	/* already mapped */
	if(APIC!=NULL)
	{
		*APIC=value;
	}
}

/* register NMI callback */
void register_NMI_callback()
{
	nmicallbackhandler=KeRegisterNmiCallback(&ISRNmiCallback,NULL);
	if(nmicallbackhandler==NULL)
	{
		dbg_print("NMI callback register error");
	}else{
		dbg_print("NMI callback registered");
	}
}

/* BTS Setup Thread */
VOID BTSThread(_In_ PVOID StartContext)
{
	/* avoid warnings */
	UNREFERENCED_PARAMETER(StartContext);

	/* delay execution for a while
	 * TODO: As an alternative, we can create thread in SUSPENDED mode
	 */
	delay_exec(DELAY_INTERVAL);

	/* User notification */
	dbg_print("Thread Started");

	/* Attach thread to core */
	thread_attach_to_core(BTS_CORE);
	dbg_print("Thread attached to core");
	
	/* Register NMI Callback */
	register_NMI_callback();
	
	/* map APIC */
	map_apic();

	/* set APIC value */
	set_apic(APIC_INT_VALUE);

	DS_BASE[BTS_CORE]=(PTDS_BASE)ExAllocatePoolWithTag(NonPagedPool,sizeof(TDS_BASE),POOL_TAG);
	BTS_BUFFER[BTS_CORE]=(PTBTS_BUFFER)ExAllocatePoolWithTag(NonPagedPool,sizeof(TBTS_BUFFER)*SIZE_BTS_BUFFER,POOL_TAG);

	if(DS_BASE!=NULL && BTS_BUFFER != NULL)
	{
		FILL_DS_WITH_BUFFER(DS_BASE[BTS_CORE],BTS_BUFFER[BTS_CORE]);
		dbg_print("Buffers were set");

		__writemsr(MSR_DS_AREA,(UINT_PTR)DS_BASE[BTS_CORE]);
		__writemsr(MSR_DEBUGCTL,BTS_INTERRUPT_FLAGS);
	}
}

/* Add device routine - setup */
NTSTATUS InvertedEvtDeviceAdd(WDFDRIVER Driver, PWDFDEVICE_INIT DeviceInit)
{
    NTSTATUS status;
    WDF_OBJECT_ATTRIBUTES objAttributes;
    WDFDEVICE device;
    WDF_IO_QUEUE_CONFIG queueConfig;
    PINVERTED_DEVICE_CONTEXT devContext;
	
    DECLARE_CONST_UNICODE_STRING(nativeDeviceName,NATIVE_NAME);
    DECLARE_CONST_UNICODE_STRING(userDeviceName,USER_NAME);
    
    UNREFERENCED_PARAMETER(Driver);
    
    WDF_OBJECT_ATTRIBUTES_INIT(&objAttributes);

    WDF_OBJECT_ATTRIBUTES_SET_CONTEXT_TYPE(&objAttributes,INVERTED_DEVICE_CONTEXT);

    status = WdfDeviceInitAssignName(DeviceInit, &nativeDeviceName);

	/* error case */
    if (!NT_SUCCESS(status)) {
        dbg_print("WdfDeviceInitAssignName failed");
        return status;
    }

    status = WdfDeviceInitAssignSDDLString(DeviceInit,&INVERTED_DEVICE_PROTECTION);

	/* error case */
    if (!NT_SUCCESS(status)) {
        dbg_print("WdfDeviceInitAssignSDDLString failed");
        return status;
    }

    status = WdfDeviceCreate(&DeviceInit,&objAttributes,&device);

	/* error case */
    if (!NT_SUCCESS(status)) {
        dbg_print("WdfDeviceCreate failed");
        return status;
    }

    devContext = InvertedGetContextFromDevice(device);    
    devContext->Sequence = THRESHOLD_BTS_BUFFER;
    
    status = WdfDeviceCreateSymbolicLink(device, &userDeviceName);

    if (!NT_SUCCESS(status)) {
        dbg_print("WdfDeviceCreateSymbolicLink failed");
        return status;
    }

    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&queueConfig,WdfIoQueueDispatchParallel);

    queueConfig.EvtIoDeviceControl = InvertedEvtIoDeviceControl;
    queueConfig.PowerManaged = WdfFalse;

    status = WdfIoQueueCreate(device,&queueConfig,WDF_NO_OBJECT_ATTRIBUTES,WDF_NO_HANDLE);

    if (!NT_SUCCESS(status)) {
        dbg_print("WdfIoQueueCreate for default queue failed");
        return status;
    }

    WDF_IO_QUEUE_CONFIG_INIT(&queueConfig,WdfIoQueueDispatchManual);

    queueConfig.PowerManaged = WdfFalse;

    status = WdfIoQueueCreate(device,&queueConfig,WDF_NO_OBJECT_ATTRIBUTES,&devContext->NotificationQueue);

    if (!NT_SUCCESS(status)) {
        dbg_print("WdfIoQueueCreate for manual queue failed");
        return status;
    }

	/* set up BTS on a given core */
	PsCreateSystemThread(&thandle,GENERIC_ALL,NULL,NULL,NULL,BTSThread,NULL);

    return status;
}

/* get register values */
void get_reg_values(PREGS reg)
{
	reg->cr0=__readcr0();
	reg->cr1=__readcr2();
	reg->cr2=__readcr3();
	reg->cr3=__readcr4();
	reg->cr4=__readcr8();
}

/* Given an I/O requirest, fill branch struct with branch data */
void get_branch_data(PBRANCHDATA branch_buffer)
{
	/* Get FROM, TO address from global var */
	branch_buffer->FROM=FROM;
	branch_buffer->TO=TO;
	/* No PID information needs to be supplied,
	 * since we are monitoring only one process at time
	 */
	branch_buffer->PID=INIT_NO_PID;
}

/* I/O Dispatch Routine */
VOID InvertedEvtIoDeviceControl(WDFQUEUE Queue,
            WDFREQUEST Request,
            size_t OutputBufferLength,
            size_t InputBufferLength,
            ULONG IoControlCode)
{
	/* register values vars */
	PREGS reg;
	/* branch data vars */
	PBRANCHDATA branch_buffer;
	/* general vars */
    PINVERTED_DEVICE_CONTEXT devContext;
    NTSTATUS status;
    ULONG_PTR info;
	PVOID buffer;
	NTSTATUS code;
	
	/* Avoid some warnings */
    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);

	/* Initial Setup */
	Queue2=Queue;

	/* Get device context */
    devContext = InvertedGetContextFromDevice(WdfIoQueueGetDevice(Queue));

	/* empty status to start */
    status = STATUS_INVALID_PARAMETER;
    info = NO_DATA_RETURNED;

	/* notify user */
    dbg_print(IOCTL_DISPATCH);

	/* Check Operation Code */
    switch(IoControlCode) {
		/* Case receiving PID from client */
		case IOCTL_VOIDBG_SEND_PID:
		{
				/* user notification */
				dbg_print(SEND_PID_MSG);
				/* get data (pid) */
				code=WdfRequestRetrieveInputBuffer(Request,sizeof(int),&buffer,NULL);
				/* case: error */
				if(!NT_SUCCESS(code))
				{
					dbg_print(GET_IN_BUFFER_ERROR);
					monitored_pid=INIT_NO_PID;
				}else
				{
					monitored_pid=*(int*)buffer;
					dbg_print(PID_OK_MSG);
				}
			
			status = STATUS_SUCCESS;
			break;
		}
		/* case client is asking for registers values */
		case IOCTL_VOIDBG_GET_REG:
			{
				/* user notification */
				dbg_print(GET_REG_MSG);	

				/* Check supplied buffer size */
				if(OutputBufferLength < sizeof(REGS)) {
					dbg_print(BUFFER_SIZE_ERROR);
					break;
				}

				/* get output buffer */
				code=WdfRequestRetrieveOutputBuffer(Request,sizeof(REGS),&buffer,NULL);
				if(!NT_SUCCESS(code))
				{
					dbg_print(GET_OUT_BUFFER_ERROR);
				}else
				{
					/* buffer is OK, get register values */
					reg=(PREGS)buffer;
					get_reg_values(reg);
				}
				info=sizeof(REGS);
				status=STATUS_SUCCESS;
				break;
			}

		/* case get branch data */
		case IOCTL_VOIDBG_GET_DATA:
			{
				/* notify user */
				dbg_print(GET_DATA_MSG);	

				/* Check buffer size */
				if(OutputBufferLength < sizeof(BRANCHDATA)) {
					dbg_print(BUFFER_SIZE_ERROR);
					break;
				}

				/* get buffer */
				code=WdfRequestRetrieveOutputBuffer(Request,sizeof(BRANCHDATA),&buffer,NULL);
				/* error case */
				if(!NT_SUCCESS(code))
				{
					dbg_print(GET_OUT_BUFFER_ERROR);
				}else
				/* success case */
				{
					/* user notification */
					dbg_print(BRANCH_DATA_MSG);
					branch_buffer=(PBRANCHDATA)buffer;
					get_branch_data(branch_buffer);
				}
				info=sizeof(BRANCHDATA);
				status=STATUS_SUCCESS;
				break;
			}

		/* Asked to cache an I/O request */
        case IOCTL_VOIDBG_INVERT_NOTIFICATION: {

			/* User notification */
			dbg_print(CACHE_REQUEST);

			/* buffer size check */
            if(OutputBufferLength < sizeof(LONG)) {
                dbg_print(GET_OUT_BUFFER_ERROR);
                break;
            }

			/* try to cache request */
            status = WdfRequestForwardToIoQueue(Request,devContext->NotificationQueue);

			/* check if request was cached */
            if(!NT_SUCCESS(status)) {
				dbg_print(CACHE_REQUEST_ERROR);
                break;
            }
            return;
        }

		/* case an interrupt occured */										
        case IOCTL_VOIDBG_INVERT_EVENT_OCCURRED: {
			/* fires inverted I/O call */
            InvertedNotify(devContext);
            status = STATUS_SUCCESS;
            break;
        }

		/* Any other case is considered an error */
        default: {
			dbg_print(INVALID_IOCTL);
		    break;
        }
    }

    WdfRequestCompleteWithInformation(Request,status,info);    
}

/* Inverted I/O Implementation
 * This function implements the interrupt notification
 */
VOID InvertedNotify(PINVERTED_DEVICE_CONTEXT DevContext)
{
    NTSTATUS status;
    ULONG_PTR info;
    WDFREQUEST notifyRequest;
    PULONG  bufferPointer;
    LONG valueToReturn;

	/* Get cached I/O Request */
    status = WdfIoQueueRetrieveNextRequest(DevContext->NotificationQueue,&notifyRequest);

	/* Case no request available */
    if(!NT_SUCCESS(status)) {    
        dbg_print(NO_CACHED_IO);
        return;
    }

	/* get the output buffer */
    status = WdfRequestRetrieveOutputBuffer(notifyRequest,sizeof(LONG),(PVOID*)&bufferPointer,NULL); 

	/* case cannot get buffer */
    if(!NT_SUCCESS(status)) {
        dbg_print(GET_OUT_BUFFER_ERROR);
        status = STATUS_SUCCESS;
		/* No Data Returned */
        info = NO_DATA_RETURNED;
    } else {
		/* Data to Return */
		/* We are returning the number of BTS entries */
        valueToReturn = InterlockedExchangeAdd(&DevContext->Sequence,THRESHOLD_BTS_BUFFER);
        *bufferPointer = valueToReturn;
        status = STATUS_SUCCESS;
		/* return data size */
        info = sizeof(THRESHOLD_BTS_BUFFER);
    }

	/* Complete I/O */
    WdfRequestCompleteWithInformation(notifyRequest, status, info);
}

/* Implements the debug print */
void dbg_print(char *msg)
{
#ifdef DBG_PRINTS
	DbgPrint("[%s] %s",PRINT_NAME,msg);
#else
	return;
#endif
}