#ifndef PORTS_H
#define PORTS_H

#include <stdint.h>

namespace Port {
    class Port {
        protected:
            Port();
            ~Port();
    };

    class Port8Bits : public Port {
        public:
            Port8Bits();
            ~Port8Bits();
            uint8_t in(uint16_t portNumber);
            void out(uint8_t data, uint16_t portNumber);
    };

    class Port16Bits : public Port {
        public:
            Port16Bits();
            ~Port16Bits();
            uint16_t in(uint16_t portNumber);
            void out(uint16_t data, uint16_t portNumber);
    };

    class Port32Bits : public Port {
        public:
            Port32Bits();
            ~Port32Bits();
            uint32_t in(uint16_t portNumber);
            void out(uint32_t data, uint16_t portNumber);
    };
}

#endif // PORTS_H