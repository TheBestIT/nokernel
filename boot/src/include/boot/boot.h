#ifndef BOOT_H
#define BOOT_H

#include "efiapi/Base.h"
#include "efiapi/Errors.h"
#include "efiapi/Protocols.h"
#include "efiapi/BootServices.h"
#include "shared/boot/info.h"
#include <stdint.h>

#define PT_LOAD 1

#define BOOTINFO_MAGIC   0x544F4F42464E4942ULL   /* "BINFBOOT" */
#define BOOTINFO_VERSION 1

typedef struct {
    uint8_t  e_ident[16];
    uint16_t e_type, e_machine;
    uint32_t e_version;
    uint64_t e_entry, e_phoff, e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize, e_phentsize, e_phnum, e_shentsize, e_shnum, e_shstrndx;
} Elf64_Ehdr;

typedef struct {
    uint32_t p_type, p_flags;
    uint64_t p_offset, p_vaddr, p_paddr, p_filesz, p_memsz, p_align;
} Elf64_Phdr;

EFI_STATUS handover(EFI_HANDLE image, EFI_SYSTEM_TABLE *st, UINTN physEntryAddress, bootinfo_t *bootInfo);

#endif // BOOT_H