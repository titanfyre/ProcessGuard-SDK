#include <ntifs.h>
#include <wdf.h>
#include <initguid.h>
#include "ProcessGuard.h"
#include "Match.h"
DEFINE_GUID(GUID_GUARD,0x556661b8,0xb6c2,0x44e5,0xbf,0x35,0xc1,0x2e,0xdf,0x8a,0xc0,0xac);
C_ASSERT(sizeof(PG_EVENT)==1088);
typedef struct GUARD_CONTEXT {
 EX_PUSH_LOCK lock;WDFFILEOBJECT owner;PEPROCESS process;
 PG_POLICY policy;PG_STATS stats;PG_U32 head,count;
 PG_EVENT ring[PG_RING],scratch;BOOLEAN registered,cleaned;
 
} GUARD_CONTEXT;
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(GUARD_CONTEXT,Context);
static GUARD_CONTEXT* volatile g_context;
static volatile LONG claimed;
DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD AddDevice;
EVT_WDF_OBJECT_CONTEXT_CLEANUP CleanupDevice;
EVT_WDF_DEVICE_SELF_MANAGED_IO_INIT InitIo;
EVT_WDF_DEVICE_SELF_MANAGED_IO_CLEANUP CleanupIo;
EVT_WDF_DEVICE_FILE_CREATE FileCreate;
EVT_WDF_FILE_CLEANUP FileCleanup;
EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL Control;
static VOID ProcessGuardLock(GUARD_CONTEXT* c){KeEnterCriticalRegion();ExAcquirePushLockExclusive(&c->lock);}
static VOID ProcessGuardUnlock(GUARD_CONTEXT* c){ExReleasePushLockExclusive(&c->lock);KeLeaveCriticalRegion();}
static VOID EmitLocked(GUARD_CONTEXT* c,PG_EVENT* e){
 e->Sequence=++c->stats.Produced;e->Generation=c->stats.Generation;
 if(e->Decision)++c->stats.Denied;if(e->Flags&PG_UNKNOWN_NAME)++c->stats.UnknownNames;
 if(c->count==PG_RING){c->head=(c->head+1)%PG_RING;--c->count;++c->stats.Dropped;}
 c->ring[(c->head+c->count)%PG_RING]=*e;++c->count;
}
static VOID Name(PG_EVENT* e,PCUNICODE_STRING name){
 PG_U32 chars;
 if(!name||!name->Buffer||(name->Length&1)||!name->Length){e->Flags|=PG_UNKNOWN_NAME;return;}
 chars=name->Length/sizeof(WCHAR);if(chars>=PG_PATH){chars=PG_PATH-1;e->Flags|=PG_TRUNCATED;}
 if(chars&&name->Buffer[chars-1]>=0xD800&&name->Buffer[chars-1]<=0xDBFF)--chars;
 RtlCopyMemory(e->Path,name->Buffer,chars*sizeof(WCHAR));e->Chars=chars;
}
static VOID Notify(PEPROCESS process,HANDLE pid,PPS_CREATE_NOTIFY_INFO info){
 GUARD_CONTEXT* c=(GUARD_CONTEXT*)InterlockedCompareExchangePointer((PVOID volatile*)&g_context,NULL,NULL);PG_EVENT* e;PG_U32 i;PUNICODE_STRING normalized=NULL;
 if(!c||!info)return;
 ProcessGuardLock(c);i=c->stats.Active&&!c->cleaned&&c->process==PsGetCurrentProcess();ProcessGuardUnlock(c);if(!i)return;
 // ImageFileName may be an exact DOS-object path (\??\C:\...), rather
 // than a native volume path. Resolve the actual process image through the
 // documented security routine; never compare an SDK spelling to an alias.
 // Allocation/name resolution must stay outside the policy lock.
 if(!NT_SUCCESS(SeLocateProcessImageName(process,&normalized)))normalized=NULL;
 ProcessGuardLock(c);
 // Use the actual creating process object, never a caller-supplied or spoofed parent PID.
 if(!c->stats.Active||c->cleaned||c->process!=PsGetCurrentProcess()){ProcessGuardUnlock(c);if(normalized)ExFreePool(normalized);return;}
 e=&c->scratch;RtlZeroMemory(e,sizeof(*e));e->Qpc=(PG_U64)KeQueryPerformanceCounter(NULL).QuadPart;e->ProcessId=HandleToULong(pid);e->ProcessKey=PsGetProcessStartKey(process);e->ParentId=HandleToULong(info->ParentProcessId);e->Operation=1;
 Name(e,normalized?normalized:info->ImageFileName);if(!normalized)e->Flags|=PG_UNKNOWN_NAME;
 e->Status=(PG_U32)info->CreationStatus;if(!NT_SUCCESS(info->CreationStatus))e->Flags|=PG_PRIOR_FAILURE;
 if(!(e->Flags&(PG_UNKNOWN_NAME|PG_TRUNCATED|PG_PRIOR_FAILURE)))for(i=0;i<c->policy.Count;++i){
  PG_RULE* r=&c->policy.Rules[i];if(Match(r,e->Path,e->Chars,FALSE)){e->RuleId=r->Id;e->Decision=r->Action;if(e->Decision){info->CreationStatus=STATUS_ACCESS_DENIED;e->Status=(PG_U32)STATUS_ACCESS_DENIED;}break;}
 }EmitLocked(c,e);ProcessGuardUnlock(c);if(normalized)ExFreePool(normalized);
}
VOID FileCreate(WDFDEVICE device,WDFREQUEST request,WDFFILEOBJECT file){
 GUARD_CONTEXT* c=Context(device);PEPROCESS process=NULL;NTSTATUS status;
 status=PsLookupProcessByProcessId(ULongToHandle(WdfRequestGetRequestorProcessId(request)),&process);
 if(NT_SUCCESS(status)){
  ProcessGuardLock(c);
  if(c->cleaned)status=STATUS_DELETE_PENDING;else if(c->owner)status=STATUS_SHARING_VIOLATION;else {c->owner=file;c->process=process;process=NULL;}
  ProcessGuardUnlock(c);if(process)ObDereferenceObject(process);
 }WdfRequestComplete(request,status);
}
VOID FileCleanup(WDFFILEOBJECT file){
 GUARD_CONTEXT* c=Context(WdfFileObjectGetDevice(file));PEPROCESS process=NULL;
 ProcessGuardLock(c);
 if(c->owner==file){c->stats.Active=0;c->stats.RuleCount=0;RtlZeroMemory(&c->policy,sizeof(c->policy));c->owner=NULL;process=c->process;c->process=NULL;c->head=c->count=0;}
 ProcessGuardUnlock(c);if(process)ObDereferenceObject(process);
}
VOID Control(WDFQUEUE queue,WDFREQUEST request,size_t output,size_t input,ULONG code){
 GUARD_CONTEXT* c=Context(WdfIoQueueGetDevice(queue));PVOID in=NULL,out=NULL;NTSTATUS status=STATUS_SUCCESS;ULONG_PTR written=0;PG_U32 i;
 if(code!=PG_SET&&code!=PG_STOP&&code!=PG_STATUS&&code!=PG_RECEIVE){WdfRequestComplete(request,STATUS_INVALID_DEVICE_REQUEST);return;}
 if(input!=(code==PG_SET?sizeof(PG_POLICY):sizeof(PG_REQUEST))||output!=(code==PG_STATUS?sizeof(PG_STATS):code==PG_RECEIVE?sizeof(PG_BATCH):0)){WdfRequestComplete(request,STATUS_INFO_LENGTH_MISMATCH);return;}
 status=WdfRequestRetrieveInputBuffer(request,input,&in,NULL);if(!NT_SUCCESS(status)){WdfRequestComplete(request,status);return;}
 if(code==PG_SET){
  PG_POLICY* p=in;if(!PG_ValidPolicy(p)){WdfRequestComplete(request,STATUS_INVALID_PARAMETER);return;}
  for(i=0;i<p->Count;++i)if(!AllowedScope(&p->Rules[i])){WdfRequestComplete(request,STATUS_INVALID_PARAMETER);return;}
 }else{PG_REQUEST* r=in;if(r->Version!=PG_VERSION||r->Size!=sizeof(*r)||r->Reserved[0]||r->Reserved[1]){WdfRequestComplete(request,STATUS_INVALID_PARAMETER);return;}}
 if(output){status=WdfRequestRetrieveOutputBuffer(request,output,&out,NULL);if(!NT_SUCCESS(status)){WdfRequestComplete(request,status);return;}}
 ProcessGuardLock(c);
 if(c->cleaned)status=STATUS_DELETE_PENDING;
 else if(c->owner!=WdfRequestGetFileObject(request)||!c->process)status=STATUS_ACCESS_DENIED;
 else if(code==PG_SET){
  c->policy=*(PG_POLICY*)in;c->stats.Active=1;c->stats.RuleCount=c->policy.Count;++c->stats.Generation;
 }else if(code==PG_STOP){c->stats.Active=0;c->stats.RuleCount=0;RtlZeroMemory(&c->policy,sizeof(c->policy));}
 else if(code==PG_STATUS){*(PG_STATS*)out=c->stats;((PG_STATS*)out)->Available=c->count;written=sizeof(PG_STATS);}
 else{PG_BATCH* b=out;RtlZeroMemory(b,sizeof(*b));b->Count=c->count<PG_READ?c->count:PG_READ;
  for(i=0;i<b->Count;++i){b->Events[i]=c->ring[c->head];c->head=(c->head+1)%PG_RING;}c->count-=b->Count;b->Stats=c->stats;b->Stats.Available=c->count;written=sizeof(*b);
 }ProcessGuardUnlock(c);WdfRequestCompleteWithInformation(request,status,written);
}
VOID CleanupDevice(WDFOBJECT object){UNREFERENCED_PARAMETER(object);InterlockedExchange(&claimed,0);}
_Use_decl_annotations_ VOID CleanupIo(WDFDEVICE device){
 GUARD_CONTEXT* c=Context(device);ProcessGuardLock(c);c->cleaned=TRUE;c->stats.Active=0;ProcessGuardUnlock(c);
 // The documented unregister waits for callbacks before context destruction.
 if(c->registered){PsSetCreateProcessNotifyRoutineEx(Notify,TRUE);c->registered=FALSE;}
 InterlockedCompareExchangePointer((PVOID volatile*)&g_context,NULL,c);
}
_Use_decl_annotations_ NTSTATUS InitIo(WDFDEVICE device){
 GUARD_CONTEXT* c=Context(device);NTSTATUS status;
 InterlockedExchangePointer((PVOID volatile*)&g_context,c);
 status=PsSetCreateProcessNotifyRoutineEx(Notify,FALSE);
 if(!NT_SUCCESS(status)){InterlockedCompareExchangePointer((PVOID volatile*)&g_context,NULL,c);return status;}
 c->registered=TRUE;return STATUS_SUCCESS;
}
NTSTATUS AddDevice(WDFDRIVER driver,PWDFDEVICE_INIT init){
 WDFDEVICE device;WDF_OBJECT_ATTRIBUTES attr;WDF_FILEOBJECT_CONFIG files;WDF_IO_QUEUE_CONFIG queue;WDF_PNPPOWER_EVENT_CALLBACKS pnp;GUARD_CONTEXT* c;NTSTATUS status;UNICODE_STRING sddl;LARGE_INTEGER frequency;
 UNREFERENCED_PARAMETER(driver);if(InterlockedCompareExchange(&claimed,1,0))return STATUS_DEVICE_BUSY;
 WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&pnp);pnp.EvtDeviceSelfManagedIoInit=InitIo;pnp.EvtDeviceSelfManagedIoCleanup=CleanupIo;WdfDeviceInitSetPnpPowerEventCallbacks(init,&pnp);
 WdfDeviceInitSetDeviceType(init,0x8000);WdfDeviceInitSetCharacteristics(init,FILE_DEVICE_SECURE_OPEN|FILE_AUTOGENERATED_DEVICE_NAME,FALSE);
 RtlInitUnicodeString(&sddl,L"D:P(A;;GA;;;SY)(A;;GA;;;BA)");status=WdfDeviceInitAssignSDDLString(init,&sddl);if(!NT_SUCCESS(status)){InterlockedExchange(&claimed,0);return status;}
 WDF_FILEOBJECT_CONFIG_INIT(&files,FileCreate,NULL,FileCleanup);WDF_OBJECT_ATTRIBUTES_INIT(&attr);attr.ExecutionLevel=WdfExecutionLevelPassive;WdfDeviceInitSetFileObjectConfig(init,&files,&attr);
 WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attr,GUARD_CONTEXT);attr.ExecutionLevel=WdfExecutionLevelPassive;attr.EvtCleanupCallback=CleanupDevice;
 status=WdfDeviceCreate(&init,&attr,&device);if(!NT_SUCCESS(status)){InterlockedExchange(&claimed,0);return status;}
 c=Context(device);c->stats.Version=PG_VERSION;c->stats.Size=sizeof(PG_STATS);c->stats.Capacity=PG_RING;KeQueryPerformanceCounter(&frequency);c->stats.Frequency=(PG_U64)frequency.QuadPart;
 ExInitializePushLock(&c->lock);
 WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&queue,WdfIoQueueDispatchSequential);queue.EvtIoDeviceControl=Control;queue.PowerManaged=WdfFalse;
 status=WdfIoQueueCreate(device,&queue,WDF_NO_OBJECT_ATTRIBUTES,WDF_NO_HANDLE);if(!NT_SUCCESS(status))return status;
 return WdfDeviceCreateDeviceInterface(device,&GUID_GUARD,NULL);
}
NTSTATUS DriverEntry(PDRIVER_OBJECT object,PUNICODE_STRING registry){WDF_DRIVER_CONFIG config;WDF_DRIVER_CONFIG_INIT(&config,AddDevice);return WdfDriverCreate(object,registry,WDF_NO_OBJECT_ATTRIBUTES,&config,WDF_NO_HANDLE);}
