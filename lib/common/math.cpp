#include "include/math.h"

#ifndef COMMON_NO_FLOAT

int math::round(double x) {
    if (x >= 0.0) return static_cast<int>(x + 0.5);
    else return static_cast<int>(x - 0.5);
}

// Estimates ln(base) using the Symmetric Taylor-Logarithm in O(n)
double math::ln(double base) {
    if (base <= 0.0) return -1.0;

    const double y = (base - 1.0) / (base + 1.0);
    const double ySquared = y * y;

    double sum = 0.0;
    double currentPow = y; // y^1

    for (int n = 0; n < MAX_TAYLOR_ITERATIONS; n++) {
        double term = currentPow / (2.0 * n + 1.0);
        sum += term;

        if (term < PRECISION && term > -PRECISION) break;

        currentPow *= ySquared; // y^1 * y^2n -> y^2n+1
    }
    
    return 2.0 * sum;
}

// Taylor approximation for e^r where |r| <= 0.5
double _expTaylor(double r) {
    double sum = 1.0;
    double term = 1.0;

    for (int n = 1; n <= 20; n++) {
        term *= r / n;
        sum += term;

        if (math::abs(term) < math::PRECISION) break;
    }

    return sum;
}

double math::exp(double power) {
    // Split power into r and k
    // power=k+r where k=round(power) and |r| <= 0.5 
    int k = math::round(power);
    double r = power - k;

    // e^r
    double er = _expTaylor(r);

    // e^k
    double ek = 1.0;
    double base = math::EULER;

    int abs_k = math::abs(k);

    while (abs_k > 0) {
        if (abs_k % 2 == 1) ek *= base;
        base *= base;
        abs_k /= 2;
    }

    if (k < 0) ek = 1.0 / ek;
    
    // e^(k+r) => e^power
    return ek * er;
}
#endif // COMMON_NO_FLOAT
