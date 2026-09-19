#ifndef SHARED_BOOT_INFO_H
#define SHARED_BOOT_INFO_H

#include <stdint.h>

#include "framebuffer.h"
#include "shared/libs/bitmap.h"

typedef struct {
    uint64_t magic;
    uint32_t version;
    uint32_t size;              /* sizeof(bootinfo_t) */

    /* memory */
    uint64_t mmap;              /* EFI_MEMORY_DESCRIPTOR * */
    uint64_t mmap_size;
    uint64_t desc_size;
    uint32_t desc_ver;
    uint32_t _pad0;

    /* framebuffer */
    framebuffer_t fb;
    Bitmap font;

    /* kernel position */
    uint64_t kernel_phys, kernel_virt, kernel_size;

    /* initrd */
    uint64_t initrd_phys, initrd_size;

    /* firmware */
    uint64_t rsdp;              /* ACPI 2.0 RSDP */
} bootinfo_t;

_Static_assert(sizeof(bootinfo_t) % 8 == 0, "bootinfo");

typedef void (*kernel_entry_t)(bootinfo_t *) __attribute__((sysv_abi));

#endif // SHARED_BOOT_INFO_H