#include "arch/x86_64/arch.h"

void x86_64::ArchInit() {
    GDT gdt = GDT(); // Inits Global Descriptor Table
    IDT idt = IDT(); // Clears the Interrupt Table and loads it

    ISR isr = ISR(); 
    isr.install(); // Exceptions (gate 0 to 31)
    
    IRQ irq = IRQ();
    irq.install(); // PIC Remap and gates 32 to 47

    __asm__ volatile ("sti"); // Enables Interrupts
}