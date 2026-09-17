#ifndef PROTOCOLS_H
#define PROTOCOLS_H

#include "Base.h"
#include "BootServices.h"

// from GNU efiapi.h
#define EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL  0x00000001
#define EFI_OPEN_PROTOCOL_GET_PROTOCOL        0x00000002
#define EFI_OPEN_PROTOCOL_TEST_PROTOCOL       0x00000004
#define EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER 0x00000008
#define EFI_OPEN_PROTOCOL_BY_DRIVER           0x00000010
#define EFI_OPEN_PROTOCOL_EXCLUSIVE           0x00000020

typedef struct {
    EFI_TABLE_HEADER Hdr;
    CHAR16      *FirmwareVendor;
    uint32_t     FirmwareRevision;
    EFI_HANDLE ConsoleInHandle; void *ConIn;
    EFI_HANDLE ConsoleOutHandle; EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut;
    EFI_HANDLE StandardErrorHandle; void *StdErr;
    void *RuntimeServices;
    EFI_BOOT_SERVICES *BootServices;
    UINTN NumberOfTableEntries;
    void *ConfigurationTable;
} EFI_SYSTEM_TABLE;

_Static_assert(offsetof(EFI_SYSTEM_TABLE, BootServices) == 96, "ST layout");

// From UEFI Spec 10.2: EFI Device Path Protocol

typedef struct {
    uint8_t Type;
    uint8_t SubType;
    uint8_t Length[2];
} EFI_DEVICE_PATH_PROTOCOL;

// From UEFI Spec 9.1.1: EFI_LOADED_IMAGE_PROTOCOL
typedef struct {
    uint32_t            Revision;
    EFI_HANDLE          ParentHandle;
    EFI_SYSTEM_TABLE    *SystemTable;

    // Image Source
    EFI_HANDLE                  DeviceHandle;
    EFI_DEVICE_PATH_PROTOCOL    *FilePath;
    void                        *Reserved;
    
    // Load Options
    uint32_t    LoadOptionsSize;
    void        *LoadOptions;

    // Image Location
    void                *ImageBase;
    UINTN               ImageSize;
    EFI_MEMORY_TYPE     ImageCodeType;
    EFI_MEMORY_TYPE     ImageDataType;
    EFI_IMAGE_UNLOAD    Unload;
} EFI_LOADED_IMAGE_PROTOCOL;

#endif // PROTOCOLS_H