#ifndef BOOTSERVICES_H
#define BOOTSERVICES_H

#include "Base.h"

// From UEFI Spec 7.3.9: EFI_BOOT_SERVICES.OpenProtocol()
typedef EFI_STATUS (*EFI_OPEN_PROTOCOL)(
    EFI_HANDLE, // Handle
    EFI_GUID *, // Protocol
    void **,    // Interface
    EFI_HANDLE, // AgentHandle
    EFI_HANDLE, // ControllerHandle
    uint32_t    // Attributes
);

// From UEFI Spec 7.3.10: EFI_BOOT_SERVICES.CloseProtocol()
typedef EFI_STATUS (*EFI_CLOSE_PROTOCOL)(
    EFI_HANDLE, // Handle
    EFI_GUID *, // Protocol
    EFI_HANDLE, // AgentHandle
    EFI_HANDLE  // ControllerHandle 
);

// From UEFI Spec 7.4.6: EFI_BOOT_SERVICES.ExitBootServices()
typedef EFI_STATUS (*EFI_EXIT_BOOT_SERVICES) (
    EFI_HANDLE, // ImageHandle
    UINTN       // MapKey
);

// From UEFI Spec 7.4.3: EFI_BOOT_SERVICES.UnloadImage()
typedef EFI_STATUS (*EFI_IMAGE_UNLOAD) (
    EFI_HANDLE // ImageHandle
);

// From UEFI Spec 7.3.7: EFI_BOOT_SERVICES.HandleProtocol()
typedef EFI_STATUS (*EFI_HANDLE_PROTOCOL) (
    EFI_HANDLE, // Handle
    EFI_GUID *, // GUID
    void **    // Interface
);

// From UEFI Spec 7.5.2: EFI_BOOT_SERVICES.Stall()
typedef EFI_STATUS (*EFI_STALL) (
    UINTN // Microseconds
);

typedef enum {
    EfiReservedMemoryType,
    EfiLoaderCode,
    EfiLoaderData,
    EfiBootServicesCode,
    EfiBootServicesData,
    EfiRuntimeServicesCode,
    EfiRuntimeServicesData,
    EfiConventionalMemory,
    EfiUnusableMemory,
    EfiACPIReclaimMemory,
    EfiACPIMemoryNVS,
    EfiMemoryMappedIO,
    EfiMemoryMappedIOPortSpace,
    EfiPalCode,
    EfiPersistentMemory,
    EfiUnacceptedMemoryType,
    EfiMaxMemoryType
} EFI_MEMORY_TYPE;

typedef enum {
    AllocateAnyPages,
    AllocateMaxAddress,
    AllocateAddress,
    MaxAllocateType
} EFI_ALLOCATE_TYPE;

typedef struct {
    uint32_t                Type;
    EFI_PHYSICAL_ADDRESS    PhysicalStart;
    EFI_VIRTUAL_ADDRESS     VirtualStart;
    UINTN                   NumberOfPages;
    UINTN                   Attribute;
} EFI_MEMORY_DESCRIPTOR;

// 7.2.1: EFI_BOOT_SERVICES.AllocatePages()
typedef EFI_STATUS (*EFI_ALLOCATE_PAGES) (
    EFI_ALLOCATE_TYPE       Type,
    EFI_MEMORY_TYPE         MemoryType,
    UINTN                   Pages,
    EFI_PHYSICAL_ADDRESS    *Memory
);

// 7.2.2: EFI_BOOT_SERVICES.FreePages()
typedef EFI_STATUS (*EFI_FREE_PAGES) (
    EFI_PHYSICAL_ADDRESS Memory,
    UINTN                Pages
);

// 7.2.3: EFI_BOOT_SERVICES.GetMemoryMap()
typedef EFI_STATUS (*EFI_GET_MEMORY_MAP) (
    UINTN                   *MemoryMapSize,
    EFI_MEMORY_DESCRIPTOR   *MemoryMap,
    UINTN                   *MapKey,
    UINTN                   *DescriptorSize,
    uint32_t                *DescriptorVersion
);

// 7.2.4: EFI_BOOT_SERVICES.AllocatePool()
typedef EFI_STATUS (*EFI_ALLOCATE_POOL) (
    EFI_MEMORY_TYPE, // PoolType
    UINTN,           // Size
    void**           // Buffer
);

// 7.2.5: EFI_BOOT_SERVICES.FreePool()
typedef EFI_STATUS (*EFI_FREE_POOL) (
    void* Buffer
);

// 7.4.2: EFI_BOOT_SERVICES.StartImage()
typedef EFI_STATUS (*EFI_IMAGE_START) (
    EFI_HANDLE  ImageHandle,
    UINTN       *ExitDataSize,
    CHAR16      **ExitData
);

// 7.5.1: EFI_BOOT_SERVICES.SetWatchdogTimer()
typedef EFI_STATUS (*EFI_SET_WATCHDOG_TIMER) (
    UINTN  Timeout,
    UINTN  WatchdogCode,
    UINTN  DataSize,
    CHAR16 *WatchdogData
);

// From UEFI Spec 4.4.1: EFI_BOOT_SERVICES
typedef struct {
    EFI_TABLE_HEADER Hdr;

    // Take Priority
    void *RaiseTPL, *RestoreTPL;

    // Memory Services
    EFI_ALLOCATE_PAGES AllocatePages;
    EFI_FREE_PAGES FreePages; 
    EFI_GET_MEMORY_MAP GetMemoryMap; 
    EFI_ALLOCATE_POOL AllocatePool;
    EFI_FREE_POOL FreePool;

    // Event & Timer Services
    void *CreateEvent, *SetTimer, *WaitForEvent, *SignalEvent, *CloseEvent, *CheckEvent;

    // Protocol Handler Services
    void *InstallProtocolInterface, *ReinstallProtocolInterface, *UninstallProtocolInterface;
    EFI_HANDLE_PROTOCOL HandleProtocol;
    void *Reserved;
    void *RegisterProtocolNotify, *LocateHandle, *LocateDevicePath;
    void *InstallConfigurationTable;

    // Image Services
    EFI_IMAGE_UNLOAD LoadImage;
    void *StartImage, *Exit;
    EFI_IMAGE_UNLOAD UnloadImage;
    EFI_EXIT_BOOT_SERVICES ExitBootServices;

    // Miscellaneous Services
    void *GetNextMonotonicCount;
    EFI_STALL Stall;
    EFI_SET_WATCHDOG_TIMER SetWatchdogTimer;

    // DriverSupport Services
    void *ConnectController, *DisconnectController;

    // Open and Close Protocol Services
    EFI_OPEN_PROTOCOL OpenProtocol;
    EFI_CLOSE_PROTOCOL CloseProtocol;
    void* OpenProtocolInformation;

} EFI_BOOT_SERVICES;

#endif // BOOTSERVICES_H