#ifndef BOOT_H
#define BOOT_H

#include "efiapi/Base.h"
#include "efiapi/Errors.h"
#include "efiapi/Protocols.h"
#include "efiapi/BootServices.h"
#include <stdint.h>

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

#define PT_LOAD 1
#define PAGE_SIZE 4096
#define ALIGN_DOWN(x,a) ((x) & ~((UINTN)(a)-1))
#define ALIGN_UP(x,a)   (((x) + (a)-1) & ~((UINTN)(a)-1))

#define BOOTINFO_MAGIC   0x544F4F42464E4942ULL   /* "BINFBOOT" */
#define BOOTINFO_VERSION 1

typedef struct {
    uint64_t magic;
    uint32_t version;
    uint32_t size;              /* sizeof(bootinfo_t): permette estensioni */

    /* memoria (formato UEFI, il kernel lo converte) */
    uint64_t mmap;              /* EFI_MEMORY_DESCRIPTOR * */
    uint64_t mmap_size;
    uint64_t desc_size;
    uint32_t desc_ver;
    uint32_t _pad0;

    /* framebuffer */
    uint64_t fb_base;
    uint64_t fb_size;
    uint32_t fb_width, fb_height, fb_pitch, fb_format;

    /* dove sta il kernel */
    uint64_t kernel_phys, kernel_virt, kernel_size;

    /* initrd, 0 se assente */
    uint64_t initrd_phys, initrd_size;

    /* firmware */
    uint64_t rsdp;              /* ACPI 2.0 RSDP */
} bootinfo_t;

_Static_assert(sizeof(bootinfo_t) % 8 == 0, "bootinfo");

typedef void (*kernel_entry_t)(bootinfo_t *) __attribute__((sysv_abi));

EFI_STATUS bootstrap(EFI_HANDLE image, EFI_SYSTEM_TABLE *st, UINTN physEntryAddress, bootinfo_t *bootInfo);

#endif // BOOT_H