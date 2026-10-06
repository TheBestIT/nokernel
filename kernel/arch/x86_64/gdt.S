.global gdt_flush
gdt_flush:
    lgdt (%rdi)

    movw $0x10, %ax // Moves the index of the KernelDataSegment table in the GDT to the %ax register
    
    // Inits the Data, Extra, Stack Segments registers with the KernelDataSegment index
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    
    // Clears out %ax register
    xorw %ax, %ax

    // Inits the General and Extra Segments registers with 0
    movw %ax, %fs
    movw %ax, %gs

    // Far return to reload the CS
    popq %rax // Return Address
    pushq $0x08 // KernelCodeSegment index
    pushq %rax
    lretq // sets %CS = 0x08