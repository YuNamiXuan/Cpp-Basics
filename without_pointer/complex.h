#ifndef COMPLEX
#define COMPLEX

#include <iostream>
// using namespace std;

template <typename T>
class complex
{
public:
    complex(T r = 0, T i = 0) : re(r), im(i) {}

    complex<T>& operator += (const complex<T>&);

    // 在class body内定义完成的函数自动成为inline函数的候选人
    T real() const { return re; } // 不修改对象的成员函数加上"const"
    T imag() const { return im; }

private:
    T re, im;

    template<typename U>
    friend complex<U>& __doapl(complex<U>* ths, const complex<U>& r);
};

template <typename T>
inline complex<T>& __doapl(complex<T>* ths, const complex<T>& r)
{
    ths->re += r.re; // 友元函数可以直接访问类的private类型成员
    ths->im += r.im;
    return *ths;
}

template <typename T>
inline complex<T>& complex<T>::operator+=(const complex<T>& r) 
{
    return __doapl(this, r);
}

template <typename T>
std::ostream& operator << (std::ostream& os, const complex<T>& x)
{
    return os << '(' << x.real() << ',' << x.imag() << ')';
}

#endif