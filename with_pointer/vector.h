#ifndef MYVECTOR
#define MYVECTOR

#include "allocator.h"

template <typename T, typename Alloc = Allocator<T>>
class vector
{
public:
    vector(int size, const Alloc &alloc = Allocator<T>()) : _allocator(alloc) {
        // _first = new T[size];
        _first = _allocator.allocate(size);
        _last = _first;
        _end = _first + size;
    }

    vector(const vector<T>& rhs) : _allocator(rhs._allocator) {
        int size = rhs._end - rhs._first;
        // _first = new T[size];
        _first = _allocator.allocate(size);
        int len = rhs._last - rhs._first;
        for (int i = 0; i < len; i++) {
            // _first[i] = rhs._first[i];
            _allocator.construct(_first + i, rhs._first[i]);
        }
        _last = _first + len;
        _end = _first + size;
    }

    vector<T>& operator=(const vector<T>& other) {
        if (this == &other) {
            return *this;
        }
        // delete[] _first;
        _allocator = other._allocator;
        for (T *p = _first; p != _last; p++) {
            _allocator.destroy(p);  // 析构有效元素
        }
        _allocator.deallocate(_first);  // 释放堆上的数组内存

        int size = other._end - other._first;
        // _first = new T[size];
        _first = _allocator.allocate(size);
        int len = other._last - other._first;
        for (int i = 0; i < len; i++) {
            _allocator.construct(_first + i, other._first[i]);
        }
        _last = _first + len;
        _end = _first + size;
        return *this;
    }

    ~vector() {
        // delete[] _first;
        for (T *p = _first; p != _last; p++) {
            _allocator.destroy(p);  // 析构有效元素
        }
        _allocator.deallocate(_first);  // 释放堆上的数组内存
        _first = _last = _end = nullptr;
    }

    void push_back(const T& val) {
        if (full()) {
            expand();
        }
        _allocator.construct(_last++, val);
    }

    void pop_back() {
        if (empty()) {
            return ;
        }
        _last--;
        _allocator.destroy(_last);
    }

    T back() const { return *(_last - 1); }

    bool full() const { return _last == _end; }
    bool empty() const { return _first == _last; }

    int size() const { return _last - _first; }

private:
    T *_first;  // 指向数组的起始位置
    T *_last;  // 指向数组中有效元素的后继位置
    T *_end;  // 指向数组空间的后继位置
    Alloc _allocator;  // 容器的空间配置器

    void expand() {
        int size = _end - _first;
        // T *temp = new T[2 * size];
        T* temp = _allocator.allocate(2 * size);
        for (int i = 0; i < size; i++) {
            _allocator.construct(temp + i, _first[i]);
        }
        // delete[] _first;
        for (T* p = _first; p != _last; p++) {
            _allocator.destroy(p);
        }
        _allocator.deallocate(_first);
        _first = temp;
        _last = _first + size;
        _end = _first + 2 * size;
    }
};


#endif