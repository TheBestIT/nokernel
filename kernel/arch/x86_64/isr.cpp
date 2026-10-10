#include "arch/x86_64/isr.h"
#include "panic.h"

ISR::ISR() {}
ISR::~ISR() {}

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

static inline uint64_t read_cr2() {
    uint64_t v;
    __asm__ volatile (
        "mov %%cr2, %0"
        : "=r"(v)
    );
    return v;
}

static inline uint64_t read_cr3() {
    uint64_t v;
    __asm__ volatile (
        "mov %%cr3, %0"
        : "=r"(v)
    );
    return v;
}

// Oops...
[[noreturn]] void exception_panic(Registers *r) {
    static bool panic = false;
    if (panic) for (; ;) __asm__ volatile ("hlt");
    panic = true;
    
    const char *msg = r->int_no < 32 ? ExceptionMessages[r->int_no] : "Unknown";

    for (int i = 1; i > -1; i--) {
        keprintf((bool)i, "\nKernel panic - Not Syncing: %s (vector %lu, error %#lx)\n", msg, r->int_no, r->error_code);
        if (r->int_no == 14) {
            uint64_t e = r->error_code;
            keprintf((bool)i, "Page Fault: %s, %s, %s mode%s%s\n",
                e & 1  ? "protection" : "not present",
                e & 2  ? "write" : "read",
                e & 4  ? "user" : "kernel",
                e & 8  ? ", reserved bit" : "",
                e & 16 ? ", execute" : ""
            );
        }

        keprintf((bool)i, "RIP  %016lx  CS   %04lx   RFLAGS %016lx\n", r->rip, r->cs, r->rflags);
        keprintf((bool)i, "RSP  %016lx  SS   %04lx\n", r->rsp, r->ss);
        keprintf((bool)i, "RAX  %016lx  RBX  %016lx  RCX  %016lx\n", r->rax, r->rbx, r->rcx);
        keprintf((bool)i, "RDX  %016lx  RSI  %016lx  RDI  %016lx\n", r->rdx, r->rsi, r->rdi);
        keprintf((bool)i, "RBP  %016lx  R8   %016lx  R9   %016lx\n", r->rbp, r->r8, r->r9);
        keprintf((bool)i, "R10  %016lx  R11  %016lx  R12  %016lx\n", r->r10, r->r11, r->r12);
        keprintf((bool)i, "R13  %016lx  R14  %016lx  R15  %016lx\n", r->r13, r->r14, r->r15);
        keprintf((bool)i, "CR2  %016lx  CR3  %016lx\n", read_cr2(), read_cr3());
        keprintf((bool)i, "---[ end Kernel panic - Not Syncing: %s ]---\n", msg);
    }

    

    halt();
}

extern "C" void interrupt_exception_handler(Registers *r) {
    exception_panic(r);
}