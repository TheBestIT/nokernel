#ifndef ISR_H
#define ISR_H

#include <stdint.h>

#include "arch/x86_64/idt.h"
#include "arch/x86_64/regs.h"

#include "dev/console.h"

class ISR {
    public:
        ISR();
        ~ISR();
        void install();
};

#endif // ISR_H