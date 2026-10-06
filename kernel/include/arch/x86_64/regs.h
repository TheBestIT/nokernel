#ifndef REGS_H
#define REGS_H

#include <stdint.h>

struct Registers {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;
    uint64_t int_no, error_code;
    uint64_t rip, cs, rflags, rsp, ss; // CPU defined
};

#endif // REGS_H