#ifndef IDT_H
#define IDT_H

#include <stdint.h>

#include "lib/common/include/memory.h"

#define IDT_SIZE 256 // Reference: https://wiki.osdev.org/Interrupt_Descriptor_Table

#define IDT_INTERRUPT_GATE      0x8E // Ring 0
#define IDT_TRAP_GATE           0x8F // Ring 0
#define IDT_USER_INTERRUPT_GATE 0xEE // Ring 3

struct [[gnu::packed]] IDTEntry64 {
    uint16_t offset_1; // offset bits 0..15
    uint16_t selector;
    uint8_t ist; // 0..2 IST offset
    uint8_t type_attributes; // gate type, dpl, p fields
    uint16_t offset_2; // offset bits 16..31
    uint32_t offset_3; // offset bits 32..63
    uint32_t zero; // reseved
};
static_assert(sizeof(IDTEntry64) == 16);

struct [[gnu::packed]] IDTPtr64 {
    uint16_t limit;
    VirtAddress base;
};

class IDT {
    public:
        IDT();
        ~IDT();
};

void SetIDTGate(uint8_t num, void(*handler)(void), uint16_t selector, uint8_t flags);

#endif // IDT_H