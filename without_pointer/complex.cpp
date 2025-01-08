#include <iostream>
#include "complex.h"

using namespace std;

int main()
{
    complex<double> c1(2, 1);
    complex<double> c2(4, 0);
    c1 += c2;
    cout << c1.real() << endl;
    cout << c1.imag() << endl;
    cout << c1 << endl;
    return 0;
}