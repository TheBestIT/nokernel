#ifndef DISK_H
#define DISK_H

#include "efiapi/Base.h"
#include "efiapi/Protocols.h"
#include "efiapi/Errors.h"
#include "efiapi/Media.h"

#include "guids/guids.h"
#include "console/console.h"

EFI_STATUS GetDiskPartUUID(EFI_SYSTEM_TABLE *st, EFI_HANDLE handle, CHAR16 uuid[37]);

#endif // DISK_H