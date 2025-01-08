#ifndef COMPLEX_H
#define COMPLEX_H

#include <iostream>

// Define complex class
template <typename T>
class Complex {
public:
    Complex(T r = 0, T i = 0) : re(r), im(i) {}

    Complex<T>& operator+=(const Complex<T>& other);
    Complex<T>& operator-=(const Complex<T>& other);

    // Member functions that do not modify the object should be marked as const
    T real() const { return re; }
    T imag() const { return im; }

private:
    T re, im;

    // Friend function to perform addition assignment
    template<typename U>
    friend Complex<U>& __dapl(Complex<U>* lhs, const Complex<U>& rhs);
    template<typename U>
    friend Complex<U>& __dami(Complex<U>* lhs, const Complex<U>& rhs);
};


// Friend function definition
template <typename T>
inline Complex<T>& __dapl(Complex<T>* lhs, const Complex<T>& rhs) {
    lhs->re += rhs.re;  // Friend functions can directly access private members
    lhs->im += rhs.im;
    return *lhs;
}

template <typename T>
inline Complex<T>& __dami(Complex<T>* lhs, const Complex<T>& rhs) {
    lhs->re -= rhs.re;
    lhs->im -= rhs.im;
    return *lhs;
}


// Overloading operator +=
template <typename T>
inline Complex<T>& Complex<T>::operator+=(const Complex<T>& other) {
    return __dapl(this, other);
}


// Overloading operator -=
template <typename T>
inline Complex<T>& Complex<T>::operator-=(const Complex<T>& other) {
    return __dami(this, other);
}


// Overloading operator +
template <typename T>
inline Complex<T> operator+(const Complex<T>& lhs, const Complex<T>& rhs) {
    return Complex<T>(lhs.real() + rhs.real(), lhs.imag() + rhs.imag());
}

template <typename T>
inline Complex<T> operator+(const Complex<T>& lhs, T rhs) {
    return Complex<T>(lhs.real() + rhs, lhs.imag());
}

template <typename T>
inline Complex<T> operator+(T lhs, const Complex<T>& rhs) {
    return Complex<T>(lhs + rhs.real(), rhs.imag());
}


// Overloading operator -
template <typename T>
inline Complex<T> operator-(const Complex<T>& lhs, const Complex<T>& rhs) {
    return Complex<T>(lhs.real() - rhs.real(), lhs.imag() - rhs.imag());
}

template <typename T>
inline Complex<T> operator-(const Complex<T>& lhs, T rhs) {
    return Complex<T>(lhs.real() - rhs, lhs.imag());
}

template <typename T>
inline Complex<T> operator-(T lhs, const Complex<T>& rhs) {
    return Complex<T>(lhs - rhs.real(), -rhs.imag());
}

template <typename T>
inline Complex<T> operator-(const Complex<T>& x) {
    return Complex<T>(-x.real(), -x.imag());
}


// Overloading operator ==
template <typename T>
inline bool operator==(const Complex<T>& lhs, const Complex<T>& rhs) {
    return lhs.real() == rhs.real() && lhs.imag() == rhs.imag();
}

template <typename T>
inline bool operator==(const Complex<T>& lhs, const T& rhs) {
    return lhs.real() == rhs.real() && lhs.imag() == 0;
}

template <typename T>
inline bool operator==(const T& lhs, const Complex<T>& rhs) {
    return lhs == rhs.real() && rhs.imag() == 0;
}


// Overloading operator !=
template <typename T>
inline bool operator!=(const Complex<T>& lhs, const Complex<T>& rhs) {
    return lhs.real() != rhs.real() || lhs.imag() != rhs.imag();
}

template <typename T>
inline bool operator!=(const Complex<T>& lhs, const T& rhs) {
    return lhs.real() != rhs.real() || lhs.imag() != 0;
}

template <typename T>
inline bool operator!=(const T& lhs, const Complex<T>& rhs) {
    return lhs != rhs.real() || rhs.imag() != 0;
}


// Calculating the conjugate
template <typename T>
inline Complex<T> getConjuate(const Complex<T>& x) {
    return Complex<T>(x.real(), -x.imag());
}


// Overloading operator <<
template <typename T>
std::ostream& operator<<(std::ostream& os, const Complex<T>& x) {
    return os << '(' << x.real() << ',' << x.imag() << ')';
}


#endif // COMPLEX_H