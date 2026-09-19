#include <stdint.h>

#include "efiapi/Base.h"
#include "efiapi/BootServices.h"
#include "efiapi/Protocols.h"
#include "efiapi/Errors.h"
#include "efiapi/Filesystem.h"

#include "guids/guids.h"
#include "console/console.h"
#include "disk/disk.h"
#include "boot/boot.h"

uint8_t ELF_SIGNATURE[6] = {0x7F, 0x45, 0x4C, 0x46, 0x02, 0x01};

typedef EFI_LOADED_IMAGE_PROTOCOL EFI_LOADED_IMAGE;

EFI_STATUS efi_main(EFI_HANDLE image, EFI_SYSTEM_TABLE *st) {
    EFI_BOOT_SERVICES *BS = st->BootServices;

    EFI_LOADED_IMAGE *loadedImage;
    EFI_STATUS err;
    CHAR16 uuid[37];
    void *raw = NULL;

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
        Print(st, u"Boot Disk UUID: ");
        Print(st, uuid);
        Print(st, u"\n\r");
    }

    EFI_FILE_PROTOCOL *root = NULL;
    err = LibOpenRoot(st, loadedImage->DeviceHandle, &root);
    if (EFI_ERROR(err)) {
        Print(st, u"Error while opening device root\r\n");
        goto out;
    }

    EFI_FILE_PROTOCOL *kernel = 0;
    err = root->Open(root, &kernel, u"\\kernel.elf", EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(err)) {
        Print(st, u"Error while reading kernel.elf from device root\r\n");
        goto out;
    }

    UINTN infoSize = 0;
    kernel->GetInfo(kernel, &gEfiFileInfoGuid, &infoSize, 0);

    EFI_FILE_INFO *info = 0;
    if (EFI_ERROR(BS->AllocatePool(EfiLoaderData, infoSize, (void**)&info))) {
        err = EFI_OUT_OF_RESOURCES;
        goto out;
    }
    kernel->GetInfo(kernel, &gEfiFileInfoGuid, &infoSize, info);
    UINTN fileSize = info->FileSize;
    
    BS->FreePool(info);

    if (EFI_ERROR(BS->AllocatePool(EfiLoaderData, fileSize, &raw))) {
        err = EFI_OUT_OF_RESOURCES;
        goto out;
    }

    UINTN read = fileSize;
    kernel->Read(kernel, &read, raw);
    err = EFI_LOAD_ERROR; // preset error
    if (read != fileSize) goto out;

    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)raw;
    if (fileSize < sizeof *ehdr) goto out;

    for (int i = 0; i < 6; ++i) if (ehdr->e_ident[i] != ELF_SIGNATURE[i]) goto out;
    if (ehdr->e_machine != 62) goto out;
    if (ehdr->e_phoff == 0 || ehdr->e_phnum == 0) goto out;

    Elf64_Phdr *phdr = (Elf64_Phdr *)((uint8_t *)raw + ehdr->e_phoff);

    UINTN vmin = ~0ULL, vmax = 0;
    for (UINTN i = 0; i < ehdr->e_phnum; ++i) {
        Elf64_Phdr *p = (Elf64_Phdr *)((uint8_t *)phdr + i * ehdr->e_phentsize);
        if (p->p_type != PT_LOAD || p->p_memsz == 0) continue;
        if (p->p_vaddr < vmin) vmin = p->p_vaddr;
        if (p->p_vaddr + p->p_memsz > vmax) vmax = p->p_vaddr + p->p_memsz;
    }
    if (vmin > vmax) goto out;
    
    vmin = ALIGN_DOWN(vmin, PAGE_SIZE);
    vmax = ALIGN_UP(vmax, PAGE_SIZE);
    UINTN pages = (vmax - vmin) / PAGE_SIZE;

    // The kernel is not position-independent. Load it at the address from the ELF file.
    UINTN physBase = vmin;
    if (EFI_ERROR(BS->AllocatePages(AllocateAddress, EfiLoaderData, pages, &physBase))) {
        Print(st, u"Error: the kernel load address is not free\r\n");
        err = EFI_OUT_OF_RESOURCES;
        goto out;
    }

    __builtin_memset((void *)physBase, 0, pages * PAGE_SIZE);

    for (UINTN i = 0; i < ehdr->e_phnum; ++i) {
        Elf64_Phdr *p = (Elf64_Phdr *)((uint8_t *)phdr + i * ehdr->e_phentsize);
        if (p->p_type != PT_LOAD || p->p_memsz == 0) continue;

        if (p->p_offset + p->p_filesz > fileSize) goto out;
        if (p->p_filesz > p->p_memsz)             goto out;

        uint8_t *destination = (uint8_t *)physBase + (p->p_vaddr - vmin);
        __builtin_memcpy(destination, (uint8_t *)raw + p->p_offset, p->p_filesz);
    }


    UINTN entryVirtual  = ehdr->e_entry;
    UINTN entryPhysical = physBase + (ehdr->e_entry - vmin);

    PrintHex(st, ehdr->e_entry);
    Print(st, u"\n\r");
    PrintHex(st, vmin);
    Print(st, u"\n\r");
    PrintHex(st, vmax - vmin);
    Print(st, u"\n\r");
    PrintHex(st, physBase);
    Print(st, u"\n\r");

    uint64_t bi_addr = 0;
    err = st->BootServices->AllocatePages(AllocateAnyPages, EfiLoaderData, 1, &bi_addr);
    if (EFI_ERROR(err)) goto out;

    bootinfo_t *bi = (bootinfo_t *)bi_addr;
    __builtin_memset(bi, 0, sizeof *bi);
    bi->magic   = BOOTINFO_MAGIC;
    bi->version = BOOTINFO_VERSION;
    bi->size    = sizeof *bi;
    
    bi->kernel_phys = physBase;
    bi->kernel_virt = vmin;
    bi->kernel_size = vmax - vmin;

    err = bootstrap(image, st, entryPhysical, bi);
out:
    BS->CloseProtocol(image, &gEfiLoadedImageProtocolGuid, image, NULL);
    if (raw) BS->FreePool(raw);
    if (err == EFI_LOAD_ERROR) {
        Print(st, u"Load Error while validating kernel ELF\n\r");
    }
    BS->Stall(5 * 1000 * 1000);
    return err;
}