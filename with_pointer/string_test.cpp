#include <iostream>
#include "string.h"

int main() {
    // Test constructor with C-string
    String s1("Hello, World!");
    std::cout << "s1: " << s1 << std::endl;

    // Test copy constructor
    String s2 = s1;
    std::cout << "s2 (copy of s1): " << s2 << std::endl;

    // Test copy assignment operator
    String s3;
    s3 = s2;
    std::cout << "s3 (assigned from s2): " << s3 << std::endl;

    // Test self-assignment
    s3 = s3;
    std::cout << "s3 (after self-assignment): " << s3 << std::endl;

    // Test constructor with null pointer
    String s4(nullptr);
    std::cout << "s4 (constructed with nullptr): " << s4 << std::endl;

    // Test assignment to a string with different content
    String s5("Another string");
    std::cout << "s5 (before assignment): " << s5 << std::endl;
    s5 = s1;
    std::cout << "s5 (after assignment from s1): " << s5 << std::endl;

    // Test destructor (implicitly tested when objects go out of scope)
    return 0;
}