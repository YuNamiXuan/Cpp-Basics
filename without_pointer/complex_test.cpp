#include <iostream>
#include <cassert>
#include "Complex.h"

int main() {
    // Test constructor and accessors
    Complex<double> a(3.0, 4.0);
    Complex<double> b(1.0, 2.0);
    assert(a.real() == 3.0 && a.imag() == 4.0);
    assert(b.real() == 1.0 && b.imag() == 2.0);

    // Test operator <<
    std::cout << "The values of a and b: " << a << ' ' << b << std::endl; // Output: (3,4) (1,2)

    // Test operator +=
    a += b;
    assert(a.real() == 4.0 && a.imag() == 6.0);
    std::cout << "a += b: " << a << std::endl; // Output: (4,6)

    // Test operator -=
    a -= b;
    assert(a.real() == 3.0 && a.imag() == 4.0);
    std::cout << "a -= b: " << a << std::endl; // Output: (3,4)

    // Test operator + (complex + complex)
    Complex<double> c = a + b;
    assert(c.real() == 4.0 && c.imag() == 6.0);
    std::cout << "a + b: " << c << std::endl; // Output: (4,6)

    // Test operator + (complex + scalar)
    Complex<double> d = a + 2.0;
    assert(d.real() == 5.0 && d.imag() == 4.0);
    std::cout << "a + 2.0: " << d << std::endl; // Output: (5,4)

    // Test operator + (scalar + complex)
    Complex<double> e = 2.0 + a;
    assert(e.real() == 5.0 && e.imag() == 4.0);
    std::cout << "2.0 + a: " << e << std::endl; // Output: (5,4)

    // Test operator - (complex - complex)
    Complex<double> f = a - b;
    assert(f.real() == 2.0 && f.imag() == 2.0);
    std::cout << "a - b: " << f << std::endl; // Output: (2,2)

    // Test operator - (complex - scalar)
    Complex<double> g = a - 1.0;
    assert(g.real() == 2.0 && g.imag() == 4.0);
    std::cout << "a - 1.0: " << g << std::endl; // Output: (2,4)

    // Test operator - (scalar - complex)
    Complex<double> h = 5.0 - a;
    assert(h.real() == 2.0 && h.imag() == -4.0);
    std::cout << "5.0 - a: " << h << std::endl; // Output: (2,-4)

    // Test unary operator - (negation)
    Complex<double> i = -a;
    assert(i.real() == -3.0 && i.imag() == -4.0);
    std::cout << "-a: " << i << std::endl; // Output: (-3,-4)

    // Test operator ==
    assert(a == Complex<double>(3.0, 4.0));
    assert(a != b);
    std::cout << "a == Complex(3.0, 4.0): " << (a == Complex<double>(3.0, 4.0)) << std::endl; // Output: 1 (true)

    // Test operator !=
    assert(a != b);
    std::cout << "a != b: " << (a != b) << std::endl; // Output: 1 (true)

    // Test getConjugate
    Complex<double> j = getConjuate(a);
    assert(j.real() == 3.0 && j.imag() == -4.0);
    std::cout << "Conjugate of a: " << j << std::endl; // Output: (3,-4)

    return 0;
}