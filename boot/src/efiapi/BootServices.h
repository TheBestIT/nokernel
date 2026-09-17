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

// From UEFI Spec 4.4.1: EFI_BOOT_SERVICES
typedef struct {
    EFI_TABLE_HEADER Hdr;

    // Take Priority
    void *RaiseTPL, *RestoreTPL;

    // Memory Services
    void *AllocatePages, *FreePages, *GetMemoryMap, *AllocatePool, *FreePool;

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
    void *GetNextMonotonicCount, *Stall, *SetWatchdogTimer;

    // DriverSupport Services
    void *ConnectController, *DisconnectController;

    // Open and Close Protocol Services
    EFI_OPEN_PROTOCOL OpenProtocol;
    EFI_CLOSE_PROTOCOL CloseProtocol;
    void* OpenProtocolInformation;

} EFI_BOOT_SERVICES;

_Static_assert(offsetof(EFI_BOOT_SERVICES, ExitBootServices) == 232, "BS layout");
_Static_assert(offsetof(EFI_BOOT_SERVICES, OpenProtocol)     == 280, "BS layout");
_Static_assert(offsetof(EFI_BOOT_SERVICES, CloseProtocol)    == 288, "BS layout");

#endif // BOOTSERVICES_H