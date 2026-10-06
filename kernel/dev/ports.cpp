#include "dev/ports.h"

Port::Port::Port() {};
Port::Port::~Port() {};

// 8 bits
Port::Port8Bits::Port8Bits() {}
Port::Port8Bits::~Port8Bits() {}

uint8_t Port::Port8Bits::in(uint16_t portNumber) {
    uint8_t result;
    __asm__ volatile (
        "inb %1, %0"
        : "=a"(result)
        : "Nd"(portNumber)
    );
    return result;
}

void Port::Port8Bits::out(uint8_t data, uint16_t portNumber) {
    __asm__ volatile(
        "outb %0, %1"
        :
        : "a"(data), "Nd"(portNumber)
    );
}

// 16 bits
Port::Port16Bits::Port16Bits() {}
Port::Port16Bits::~Port16Bits() {}

uint16_t Port::Port16Bits::in(uint16_t portNumber) {
    uint16_t result;
    __asm__ volatile (
        "inw %1, %0"
        : "=a"(result)
        : "Nd"(portNumber)
    );
    return result;
}

void Port::Port16Bits::out(uint16_t data, uint16_t portNumber) {
    __asm__ volatile (
        "outw %0, %1"
        :
        : "a"(data), "Nd"(portNumber)
    );
}

// 32 bits
Port::Port32Bits::Port32Bits() {}
Port::Port32Bits::~Port32Bits() {}

uint32_t Port::Port32Bits::in(uint16_t portNumber) {
    uint32_t result;
    __asm__ volatile (
        "inl %1, %0"
        : "=a"(result)
        : "Nd"(portNumber)
    );
    return result;
}

void Port::Port32Bits::out(uint32_t data, uint16_t portNumber) {
    __asm__ volatile (
        "outl %0, %1"
        :
        : "a"(data), "Nd"(portNumber)
    );
}