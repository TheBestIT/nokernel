#ifndef X86_64_ARCH_H
#define X86_64_ARCH_H

#include "gdt.h"
#include "idt.h"
#include "irq.h"
#include "isr.h"

namespace x86_64 {
    void ArchInit();
}

#endif // X86_64_ARCH_H