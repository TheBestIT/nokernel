#ifndef DISK_H
#define DISK_H

#include "efiapi/Base.h"
#include "efiapi/Protocols.h"
#include "efiapi/Errors.h"
#include "efiapi/Media.h"
#include "efiapi/Filesystem.h"

#include "guids/guids.h"
#include "console/console.h"

#include "shared/libs/bitmap.h"

EFI_STATUS GetDiskPartUUID(EFI_SYSTEM_TABLE *st, EFI_HANDLE handle, CHAR16 uuid[37]);
EFI_STATUS LibOpenRoot(EFI_SYSTEM_TABLE *st, EFI_HANDLE DeviceHandle, EFI_FILE_PROTOCOL **file);

EFI_STATUS LoadBMPFile(EFI_SYSTEM_TABLE *st, EFI_FILE_PROTOCOL *root, CHAR16 *filepath, Bitmap *bitmap);

#endif // DISK_H