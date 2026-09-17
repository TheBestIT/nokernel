#include <stdint.h>

#include "efiapi/Base.h"
#include "efiapi/BootServices.h"
#include "efiapi/Protocols.h"
#include "efiapi/Errors.h"

#include "guids/guids.h"
#include "console/console.h"
#include "disk/disk.h"

/*
Call LibOpenRoot to get the reference to the boot device root
*/

typedef EFI_LOADED_IMAGE_PROTOCOL EFI_LOADED_IMAGE;

EFI_STATUS efi_main(EFI_HANDLE image, EFI_SYSTEM_TABLE *st) {
    EFI_BOOT_SERVICES *BS = st->BootServices;

    EFI_LOADED_IMAGE *loadedImage;
    EFI_STATUS err;
    CHAR16 uuid[37];

    ClearScreen(st);
    Print(st, u"Hello, EFI!\r\n");
    Print(st, st->FirmwareVendor);
    Print(st, u"\r\n");

    // Load Boot Image
    err = BS->OpenProtocol(image, &gEfiLoadedImageProtocolGuid, (void **)&loadedImage,
                            image, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    if (EFI_ERROR(err)) {
        Print(st, u"Error while opening image protocol\r\n");
        goto out;
    }

    if (GetDiskPartUUID(st, loadedImage->DeviceHandle, uuid) == EFI_SUCCESS) {
        Print(st, u"Disk UUID: ");
        Print(st, uuid);
        Print(st, u"\n\r");
    }

    err = EFI_SUCCESS;
out:
    BS->CloseProtocol(image, &gEfiLoadedImageProtocolGuid, image, NULL);
    for (;;) __asm__ volatile ("hlt");
    return err;
}