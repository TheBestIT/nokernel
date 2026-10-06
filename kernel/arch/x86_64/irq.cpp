#include "arch/x86_64/irq.h"

#define PIC1 0x20
#define PIC2 0xA0
#define PIC_EOI 0x20

#define PIC1_COMMAND PIC1
#define PIC1_DATA    (PIC1+1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA    (PIC2+1)

typedef void(*RegistersFunction)(struct Registers *r);

// IRQs (16)
extern "C" void irq0(void);  // Programmable Interrupt Timer Interrupt
extern "C" void irq1(void);  // Keyboard Interrupt
extern "C" void irq2(void);  // Cascade (Internal)
extern "C" void irq3(void);  // COM2
extern "C" void irq4(void);  // COM1
extern "C" void irq5(void);  // LPT2
extern "C" void irq6(void);  // Floppy Disk
extern "C" void irq7(void);  // LPT1
extern "C" void irq8(void);  // CMOS RTC
extern "C" void irq8(void);  // CMOS RTC
extern "C" void irq9(void);  // Free
extern "C" void irq10(void); // Free
extern "C" void irq11(void); // Free
extern "C" void irq12(void); // PS1 Mouse
extern "C" void irq13(void); // FPU / Coprocessor / Inter-processor
extern "C" void irq14(void); // Primary ATA HDD
extern "C" void irq15(void); // Secondary ATA HDD 

RegistersFunction irq_routines[16] = { 0 };

static Port::Port8Bits p8b_irq;

IRQ::IRQ() {};
IRQ::~IRQ() {};

void IRQ::remap() {
    // Init ICW1
    p8b_irq.out(0x11, PIC1_COMMAND);
    p8b_irq.out(0x11, PIC2_COMMAND);

    // Remap interrupt
    p8b_irq.out(0x20, PIC1_DATA);
    p8b_irq.out(0x28, PIC2_DATA);

    // Init ICW3
    p8b_irq.out(0x04, PIC1_DATA);
    p8b_irq.out(0x02, PIC2_DATA);

    // Init ICW4
    p8b_irq.out(0x01, PIC1_DATA);
    p8b_irq.out(0x01, PIC2_DATA);

    // Mask Interrupts
    p8b_irq.out(0x00, PIC1_DATA);
    p8b_irq.out(0x00, PIC2_DATA);
}

void IRQ::install() {
    this->remap();
    SetIDTGate(32, irq0, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(33, irq1, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(34, irq2, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(35, irq3, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(36, irq4, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(37, irq5, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(38, irq6, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(39, irq7, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(40, irq8, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(41, irq9, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(42, irq10, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(43, irq11, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(44, irq12, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(45, irq13, 0x08, IDT_INTERRUPT_GATE);
    SetIDTGate(46, irq14, 0x08, IDT_INTERRUPT_GATE);    
    SetIDTGate(47, irq15, 0x08, IDT_INTERRUPT_GATE);
}

void InstallIRQHandler(int irq, void (*handler)(Registers *r)) {
    kprintf("\nInstalling IRQ %d", irq);
    irq_routines[irq] = handler;
}

void UninstallIRQHandler(int irq) {
    irq_routines[irq] = 0;
}

extern "C" void irq_handler(Registers *r) {
    RegistersFunction handler;

    handler = irq_routines[r->int_no];
    if (handler) handler(r);

    if (r->int_no >= 8) p8b_irq.out(PIC_EOI, PIC2_COMMAND);

    p8b_irq.out(PIC_EOI, PIC1_COMMAND);
}