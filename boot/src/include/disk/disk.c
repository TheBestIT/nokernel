#include "disk.h"

typedef EFI_DEVICE_PATH_PROTOCOL EFI_DEVICE_PATH;

#define DP_END_TYPE 0x7F
static inline UINTN dp_len(const EFI_DEVICE_PATH_PROTOCOL *n) {
    return (UINTN)n->Length[0] | ((UINTN)n->Length[1] << 8);
}

static inline EFI_DEVICE_PATH_PROTOCOL *dp_next(EFI_DEVICE_PATH_PROTOCOL *n) {
    return (EFI_DEVICE_PATH_PROTOCOL *)((uint8_t *)n + dp_len(n));
}

EFI_STATUS GetDiskPartUUID(EFI_SYSTEM_TABLE *st, EFI_HANDLE handle, CHAR16 uuid[37]) {
    EFI_DEVICE_PATH *devicePath;

    devicePath = DevicePathFromHandle(st->BootServices, handle);
    if (devicePath) {
        for (EFI_DEVICE_PATH *n = devicePath; n->Type != DP_END_TYPE; n = dp_next(n)) {
            UINTN len = dp_len(n);
            if (len < 4) return EFI_INVALID_PARAMETER;

            if (n->Type != MEDIA_DEVICE_PATH) continue;
            if (n->SubType != MEDIA_HARDDRIVE_DP) continue;
            if (len < sizeof(HARDDRIVE_DEVICE_PATH)) continue;

            HARDDRIVE_DEVICE_PATH hardDrive;
            __builtin_memcpy(&hardDrive, n, sizeof hardDrive);

            if (hardDrive.SignatureType != SIGNATURE_TYPE_GUID) continue;

            EFI_GUID guid;
            __builtin_memcpy(&guid, hardDrive.Signature, sizeof guid);
            GuidToString(uuid, &guid);
            return EFI_SUCCESS;
        }
    }
    return EFI_NOT_FOUND;
}