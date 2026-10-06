#include "arch/x86_64/idt.h"

static IDTEntry64 idt[IDT_SIZE];
static IDTPtr64 idtp;

extern "C" void idt_load(IDTPtr64 *idt_ptr); // ASM Loader

IDT::IDT() {
    idtp.limit = (sizeof(idt)) - 1;
    idtp.base = (VirtAddress) & idt;

    Mem::memset(&idt, 0, sizeof(IDTEntry64) * IDT_SIZE); // clears up the idt
    
    idt_load(&idtp);
}

IDT::~IDT() {
    return;
}

void SetIDTGate(uint8_t num, void(*handler)(void), uint16_t selector, uint8_t flags) {
    VirtAddress offset = (VirtAddress)handler;

    idt[num].offset_1           = offset & 0xFFFF;
    idt[num].selector           = selector;
    idt[num].ist                = 0;
    idt[num].type_attributes    = flags;
    idt[num].offset_2           = (offset >> 16) & 0xFFFF;
    idt[num].offset_3           = (offset >> 32) & 0xFFFFFFFF;
    idt[num].zero               = 0;
}