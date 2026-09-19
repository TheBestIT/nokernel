#ifndef GUIDS_H
#define GUIDS_H
#include "efiapi/Base.h"
#include "efiapi/Protocols.h"

extern EFI_GUID gEfiLoadedImageProtocolGuid;
extern EFI_GUID gEfiSimpleFileSystemProtocolGuid;
extern EFI_GUID gEfiGraphicsOutputProtocolGuid;
extern EFI_GUID gEfiDevicePathProtocolGuid;
extern EFI_GUID gEfiFileInfoGuid;

EFI_DEVICE_PATH_PROTOCOL *DevicePathFromHandle(EFI_BOOT_SERVICES *bs, EFI_HANDLE handle);

#endif // GUIDS_H