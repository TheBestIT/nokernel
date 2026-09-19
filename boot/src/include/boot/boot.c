#include "boot.h"

EFI_STATUS bootstrap(EFI_HANDLE image, EFI_SYSTEM_TABLE *st, UINTN physEntryAddress, bootinfo_t *bootInfo) {
    st->BootServices->SetWatchdogTimer(0, 0, 0, 0);

    EFI_MEMORY_DESCRIPTOR *mmap = 0;
    UINTN size = 0, key = 0, dsize = 0;
    uint32_t dver = 0;

    uint64_t stack = 0;
    if (EFI_ERROR(st->BootServices->AllocatePages(AllocateAnyPages, EfiLoaderData, 16, &stack)))
        return EFI_OUT_OF_RESOURCES;
    uint64_t stack_top = (stack + 16 * 4096 - 16) & ~0xFULL;

    for (int attempt = 0; attempt < 3; ++attempt) {
        size = 0;
        if (st->BootServices->GetMemoryMap(&size, 0, &key, &dsize, &dver) != EFI_BUFFER_TOO_SMALL)
            return EFI_LOAD_ERROR;

        size += 4 * dsize;
        if (EFI_ERROR(st->BootServices->AllocatePool(EfiLoaderData, size, (void **)&mmap)))
            return EFI_OUT_OF_RESOURCES;
        
        if (st->BootServices->GetMemoryMap(&size, mmap, &key, &dsize, &dver) == EFI_SUCCESS) {
            bootInfo->mmap      = mmap;
            bootInfo->mmap_size = size;
            bootInfo->desc_size = dsize;
            bootInfo->desc_ver  = dver;

            if (st->BootServices->ExitBootServices(image, key) == EFI_SUCCESS)
                break;
        }

        st->BootServices->FreePool(mmap);
        mmap = 0;
    }
    if (!mmap) return EFI_LOAD_ERROR;

    // "Abandon all hope, ye who enter here"

    __asm__ volatile (
        "cli\n"
        "movq %0, %%rsp\n"
        "xorq %%rbp, %%rbp\n"
        "callq *%1\n"
        :
        : "r"(stack_top), "r"(physEntryAddress), "D"(bootInfo)
        : "memory"
    );
    __builtin_unreachable();
}