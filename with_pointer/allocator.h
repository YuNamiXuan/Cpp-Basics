#ifndef MYALLOCATOR
#define MYALLOCATOR

template <typename T>
class Allocator 
{
public:
    T* allocate(size_t size) {
        return (T*)malloc(sizeof(T) * size);
    }

    void deallocate(void *p) {
        free(p);
    }

    void construct(T *p, const T& val) {
        new (p) T(val);
    }

    void destroy(T *p) {
        p->~T();
    } 

};

#endif