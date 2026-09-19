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

EFI_STATUS LibOpenRoot(EFI_SYSTEM_TABLE *st, EFI_HANDLE DeviceHandle, EFI_FILE_PROTOCOL **file) {
    EFI_STATUS status;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *volume;

    status = st->BootServices->HandleProtocol(
        DeviceHandle,
        &gEfiSimpleFileSystemProtocolGuid,
        (void **)&volume
    );

    if (!EFI_ERROR(status)) {
        status = volume->OpenVolume(volume, file);
        return status;
    }

    return EFI_LOAD_ERROR;
}

EFI_STATUS LoadBMPFile(EFI_SYSTEM_TABLE *st, EFI_FILE_PROTOCOL *root, CHAR16 *filepath, Bitmap *bitmap) {
    EFI_BOOT_SERVICES *BS = st->BootServices;
    EFI_FILE_PROTOCOL *file = NULL;
    EFI_FILE_INFO *fileInfo = NULL;
    UINTN fileInfoSize = 0;
    UINTN fileSize = 0;
    UINTN read = 0;
    UINTN pages = 0;
    uint64_t physBase = 0;
    BitmapHeader *bmpHdr = NULL;
    EFI_STATUS err;

    err = root->Open(root, &file, filepath, EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(err)) {
        file = NULL;
        goto out;
    }

    err = file->GetInfo(file, &gEfiFileInfoGuid, &fileInfoSize, NULL);
    if (err != EFI_BUFFER_TOO_SMALL) {
        err = EFI_LOAD_ERROR;
        goto out;
    }

    err = BS->AllocatePool(EfiLoaderData, fileInfoSize, (void **)&fileInfo);
    if (EFI_ERROR(err)) {
        fileInfo = NULL;
        goto out;
    }

    err = file->GetInfo(file, &gEfiFileInfoGuid, &fileInfoSize, fileInfo);
    if (EFI_ERROR(err)) goto out;

    fileSize = fileInfo->FileSize;
    if (fileSize < sizeof(BitmapHeader)) {
        err = EFI_LOAD_ERROR;
        goto out;
    }

    pages = ALIGN_UP(fileSize, PAGE_SIZE) / PAGE_SIZE;
    err = BS->AllocatePages(AllocateAnyPages, EfiLoaderData, pages, &physBase);
    if (EFI_ERROR(err)) goto out;

    __builtin_memset((void *)physBase, 0, pages * PAGE_SIZE);

    read = fileSize;
    err = file->Read(file, &read, (void *)physBase);
    if (EFI_ERROR(err)) goto error;
    if (read != fileSize) {
        err = EFI_LOAD_ERROR;
        goto error;
    }

    bmpHdr = (BitmapHeader *)physBase;
    if (bmpHdr->Signature[0] != 'B' || bmpHdr->Signature[1] != 'M') {
        err = EFI_LOAD_ERROR; // Not a BMP file
        goto error;
    }

    bitmap->Header      = bmpHdr;
    bitmap->BaseAddress = (uint64_t *)physBase;
    err = EFI_SUCCESS;
    goto out;

error:
    BS->FreePages(physBase, pages);
out:
    if (fileInfo) BS->FreePool(fileInfo);
    if (file) file->Close(file);
    return err;
}
