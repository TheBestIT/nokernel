#include "arch/x86_64/isr.h"

ISR::ISR() {}
ISR::~ISR() {}

extern void panic(const char* exception);

// ASM ISR handlers
extern "C" void isr0(void);
extern "C" void isr1(void);
extern "C" void isr2(void);
extern "C" void isr3(void);
extern "C" void isr4(void);
extern "C" void isr5(void);
extern "C" void isr6(void);
extern "C" void isr7(void);
extern "C" void isr8(void);
extern "C" void isr9(void);
extern "C" void isr10(void);
extern "C" void isr11(void);
extern "C" void isr12(void);
extern "C" void isr13(void);
extern "C" void isr14(void);
extern "C" void isr15(void);
extern "C" void isr16(void);
extern "C" void isr17(void);
extern "C" void isr18(void);
extern "C" void isr19(void);
extern "C" void isr20(void);
extern "C" void isr21(void);
extern "C" void isr22(void);
extern "C" void isr23(void);
extern "C" void isr24(void);
extern "C" void isr25(void);
extern "C" void isr26(void);
extern "C" void isr27(void);
extern "C" void isr28(void);
extern "C" void isr29(void);
extern "C" void isr30(void);
extern "C" void isr31(void);

static const char *ExceptionMessages[32] = {
    "Division by zero",
    "Debug",
    "Non-maskable interrupt",
    "Breakpoint",
    "Detected overflow",
    "Out-of-bounds",
    "Invalid opcode",
    "No coprocessor",
    "Double fault",
    "Coprocessor segment overrun",
    "Bad TSS",
    "Segment not present",
    "Stack fault",
    "General protection fault",
    "Page fault",
    "Unknown interrupt",
    "Coprocessor fault",
    "Alignment check",
    "Machine check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void ISR::install() {
    SetIDTGate(0, isr0, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(1, isr1, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(2, isr2, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(3, isr3, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(4, isr4, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(5, isr5, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(6, isr6, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(7, isr7, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(8, isr8, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(9, isr9, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(10, isr10, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(11, isr11, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(12, isr12, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(13, isr13, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(14, isr14, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(15, isr15, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(16, isr16, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(17, isr17, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(18, isr18, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(19, isr19, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(20, isr20, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(21, isr21, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(22, isr22, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(23, isr23, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(24, isr24, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(25, isr25, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(26, isr26, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(27, isr27, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(28, isr28, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(29, isr29, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(30, isr30, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(31, isr31, 0x08, IDT_INTERRUPT_GATE);
}

extern "C" void interrupt_exception_handler(Registers *r) {
    if (r->int_no < 32) kprintf("\nError %s", ExceptionMessages[r->int_no]);
    else kprintf("Got an unknown exception %d", r->int_no);

    for ( ; ; ) __asm__ volatile ("hlt");
}