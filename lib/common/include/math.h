#ifndef COMMON_MATH_H
#define COMMON_MATH_H

#include <stdint.h>
#include <stddef.h>

// masks the value (x) to the previous bound of the mask (a) eg. mask=8: 17 -> 16 
#define ALIGN_DOWN(x,a) ((x) & ~((uint64_t)(a)-1)) 
// masks the value (x) to the next bound of the mask (a) eg. mask=8: 17 -> 24
#define ALIGN_UP(x,a)   (((x) + (a)-1) & ~((uint64_t)(a)-1))

namespace math {
    template<typename T> constexpr T abs(T x) { return (x < 0) ? -x : x; }

// Define COMMON_NO_FLOAT to remove the floating point functions. The kernel has no floating point support.
#ifndef COMMON_NO_FLOAT
    const int MAX_TAYLOR_ITERATIONS = 1000;
    const double PRECISION = 1e-15;
    const double EULER = 2.71828182845904523536;

    int round(double x);

    // natural log of base
    double ln(double base);

    // calculates e^power
    double exp(double power);

    // uses ln and exp to calculate b^p
    // -> b^p = e^(p * ln(b)) -> b^p = exp( power * ln(base) )
    inline double pow(double base, double power) { return math::exp( power * math::ln(base) ); }
    inline float pow(float base, float power) { return (float)math::pow((double)base, (double)power); }
#endif // COMMON_NO_FLOAT

    // Integer b^p with exponentiation by squaring in O(log p)
    template<typename T> constexpr T pow(T base, T power) {
        if (power < 0) {
            if (base == 1) return 1;
            if (base == static_cast<T>(-1)) return (power % 2 == 0) ? 1 : -1;
            return 0;
        }

        T result = 1;
        while (power > 0) {
            if (power % 2 == 1) result *= base;
            power /= 2;
            if (power > 0) base *= base;
        }
        return result;
    }
}

#endif // COMMON_MATH_H