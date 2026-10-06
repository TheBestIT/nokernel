#include "arch/x86_64/gdt.h"

static GDTEntry64 gdt[5];
static GDTPtr64 gp; 

extern "C" void gdt_flush(GDTPtr64* gp_ptr); // ASM Loader

GDT::GDT() {
    // Sets up the first 5 entries of the GDT:
    // Null Descriptor, Kernel Code Segment, Kernel Data Segment, User Data Segment, User Code Segment

    gp.limit = (sizeof(struct GDTEntry64) * 5) - 1;
    gp.base = (VirtAddress)&gdt;

    // 0: Null Descriptor
    this->set_gate(0, 0, 0, 0, 0);

    // 1: Kernel Code Segment
    this->set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xA);

    // 2: Kernel Data Segment
    this->set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xC);

    // 3: User Data Segment
    this->set_gate(3, 0, 0xFFFFFFFF, 0xF2, 0xC);

    // 4: User Code Segment
    this->set_gate(4, 0, 0xFFFFFFFF, 0xFA, 0xA);

    gdt_flush(&gp);
}

GDT::~GDT() {
    return;
} 

uint16_t GDT::UserCodeSegmentSelector() {
    return (uint8_t*)&gdt[4] - (uint8_t*)gdt;
}

uint16_t GDT::UserDataSegmentSelector() {
    return (uint8_t*)&gdt[3] - (uint8_t*)gdt;
}

uint16_t GDT::KernelDataSegmentSelector() {
    return (uint8_t*)&gdt[2] - (uint8_t*)gdt;
}

uint16_t GDT::KernelCodeSegmentSelector() {
    return (uint8_t*)&gdt[1] - (uint8_t*)gdt;
}

void GDT::set_gate(uint8_t index, VirtAddress base, uint64_t limit, uint8_t access, uint8_t granularity) {
    Mem::memset(&gdt[index], 0, sizeof(struct GDTEntry64)); // fills the gdt entry with zeros

    gdt[index].base_low = (base & 0xFFFF); // wipes 4 bits from left to right 0x5555AAAA -> 0xAAAA
    gdt[index].base_middle = (base >> 16) & 0xFF; // wipes 4 bits from right to left then 2 from left to right 0x5544AABB -> 0x00005544 -> 0x44
    gdt[index].base_high = (base >> 24) & 0xFF; // wipes 6 bits from right to left keeps 2 from right to left 0x5544AABB -> 0x00000055 -> 0x55

    gdt[index].limit_low = (limit & 0xFFFF);
    gdt[index].granularity = ((limit >> 16) & 0x0F);

    gdt[index].granularity |= ((granularity << 4) & 0xF0); // the bitshift is to set 64bit mode, or else it defaults to 16bit
    gdt[index].access = access;
}