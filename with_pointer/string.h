#ifndef MYSTRING
#define MYSTRING

#include <iostream>
#include <cstring>

class String
{
public:
    String(const char* cstr = 0);
    // Copy constructor function
    String(const String& str);
    // Copy assignment function
    String& operator=(const String& str);
    // Destructor function
    ~String();

    char* get_c_str() const { return my_data; };

private:
    char* my_data;

};

inline String::String(const char* cstr) {
    if (cstr) {
        my_data = new char[strlen(cstr) + 1];
        strcpy(my_data, cstr);
    }
    else {
        my_data = new char[1];
        *my_data = '\0'; 
    }
}

inline String::String(const String& str) {
    my_data = new char[strlen(str.my_data) + 1];
    strcpy(my_data, str.my_data);
}

inline String& String::operator=(const String& str) {
    if (this == &str) {     // check self assignment
        return *this;
    }

    delete[] my_data;
    my_data = new char[strlen(str.my_data) + 1];
    strcpy(my_data, str.my_data);
    return *this;
}

inline String::~String() {
    delete[] my_data;
}

std::ostream& operator<<(std::ostream& os, const String& str) {
    os << str.get_c_str();
    return os;
}


#endif