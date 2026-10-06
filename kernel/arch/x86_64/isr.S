
.extern interrupt_exception_handler
.global common_handler

.macro ISR_STUB num
.global isr\num
isr\num:
    cli
    pushq $\num
    jmp common_handler
.endm

.macro NO_ERROR_CODE_ISR_STUB num
.global isr\num
isr\num:
    cli
    pushq $0
    pushq $\num
    jmp common_handler
.endm

common_handler:
    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15

    cld
    movq %rsp, %rdi // Registers *r
    call interrupt_exception_handler

    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rbp
    popq %rdi
    popq %rsi
    popq %rdx
    popq %rcx
    popq %rbx
    popq %rax

    addq $16, %rsp // wipes ERR CODE and INT CODE from stack
    iretq


NO_ERROR_CODE_ISR_STUB 0
NO_ERROR_CODE_ISR_STUB 1
NO_ERROR_CODE_ISR_STUB 2
NO_ERROR_CODE_ISR_STUB 3
NO_ERROR_CODE_ISR_STUB 4
NO_ERROR_CODE_ISR_STUB 5
NO_ERROR_CODE_ISR_STUB 6
NO_ERROR_CODE_ISR_STUB 7
ISR_STUB 8
NO_ERROR_CODE_ISR_STUB 9
ISR_STUB 10
ISR_STUB 11
ISR_STUB 12
ISR_STUB 13
ISR_STUB 14
NO_ERROR_CODE_ISR_STUB 15
NO_ERROR_CODE_ISR_STUB 16
NO_ERROR_CODE_ISR_STUB 17
NO_ERROR_CODE_ISR_STUB 18
NO_ERROR_CODE_ISR_STUB 19
NO_ERROR_CODE_ISR_STUB 20
NO_ERROR_CODE_ISR_STUB 21
NO_ERROR_CODE_ISR_STUB 22
NO_ERROR_CODE_ISR_STUB 23
NO_ERROR_CODE_ISR_STUB 24
NO_ERROR_CODE_ISR_STUB 25
NO_ERROR_CODE_ISR_STUB 26
NO_ERROR_CODE_ISR_STUB 27
NO_ERROR_CODE_ISR_STUB 28
NO_ERROR_CODE_ISR_STUB 29
NO_ERROR_CODE_ISR_STUB 30
NO_ERROR_CODE_ISR_STUB 31