#ifndef GDT_H
#define GDT_H

#include <stdint.h>
#include <stddef.h>

#include "lib/common/include/memory.h"

// inspired by https://github.com/amanuel2/OS_Mirror/blob/master/gdt.c%2B%2B

#define KERNEL_CS 0x0008
#define KERNEL_DS 0x0010
#define USER_DS   0x0018
#define USER_CS   0x0020

struct [[gnu::packed]] GDTEntry64 {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
};
static_assert(sizeof(GDTEntry64) == 8);

struct [[gnu::packed]] GDTPtr64 {
    uint16_t limit;
    VirtAddress base;
};

class GDT {
    public:
        GDT();
        ~GDT();

        // RING 0
        uint16_t KernelDataSegmentSelector();
        uint16_t KernelCodeSegmentSelector();
        
        // RING 3
        uint16_t UserDataSegmentSelector();
        uint16_t UserCodeSegmentSelector();
    private:
        void set_gate(uint8_t index, VirtAddress base, uint64_t limit, uint8_t access, uint8_t granularity);
};

#endif // GDT_H