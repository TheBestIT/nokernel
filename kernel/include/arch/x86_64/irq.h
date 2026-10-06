#ifndef IRQ_H
#define IRQ_H

#include "idt.h"
#include "dev/ports.h"
#include "regs.h"
#include "console.h"

class IRQ {
    public:
        IRQ();
        ~IRQ();
        void install();
    private:
        void remap();
        void set_gate(uint8_t num, void(*handler)(void), uint16_t selector, uint8_t flags);
};

void InstallIRQHandler(int irq, void (*handler)(Registers *r));
void UninstallIRQHandler(int irq);

#endif // IRQ_H