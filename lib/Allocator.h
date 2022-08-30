// allocator: http://www.josuttis.com/cppcode/allocator.html

#include <limits>
#include <stdexcept>
#include <new>


template <class T>
class _Allocator {
// this class is state-less
public:
    _Allocator() { }
    _Allocator(const _Allocator&) { }
    template <class U>
    _Allocator(const _Allocator<U>&) { }
    ~_Allocator() { }

    // type rebind, usage (resulting in an allocator of rebinded type): 
    // Allocator<T>::rebind<U> a;
    template <class U>
    using rebind = _Allocator<U>;

    // max size
    size_t max_size() const { return std::numeric_limits<size_t>::max() / sizeof(T); }

    // address
    T* address(T& x) const { return &x; }
    const T* address(const T& x) const { return &x; }

    // allocate (& construct)
    T* allocate(const size_t& n, const void* = 0) {
        // return a nullptr if passed 0
        if(n == 0) return NULL;
        // std::bad_alloc is a class, here it is constructed using initializer list
        if(n > max_size()) throw std::bad_alloc{};
        // use ::operator new(size_t)
        T *ret = (T*)(::operator new(n * sizeof(T)));
        return ret;
    }
    // deallocate (& destroy)
    void deallocate(T* p, const size_t& n) {
        if((p == NULL) != (n == 0)) throw std::invalid_argument("cannot deallocate");
        // ::operator new (instead of operator new[]) was used, so we don't use ::operator delete[]
        // destructors will NOT be called automatically
        ::operator delete((void*)p);
    }

    // construct (construct objects on an allocated pointer)
    // use placement new (T must support a copy constructor)
    void construct(T* p, const T& val) { new((void*)p) T(val); }
    // move construct when provided with a rvalue
    void construct(T* p, T&& val) { new((void*)p) T(std::move(val)); }
    void destroy(T* p) { p->~T(); }
};

// all allocators of any type are interchangeable
template <class T1, class T2>
bool operator==(const _Allocator<T1>&, const _Allocator<T2>&) { return true; }

template <class T1, class T2>
bool operator!=(const _Allocator<T1>&, const _Allocator<T2>&) { return false; }
