// Vector

#pragma once

#include <exception>
#include <initializer_list>

#include "Algorithm.h"
#include "Allocator.h"
#include "Iterator.h"


template <class T, class _Alloc = _Allocator<T>>
class Vector {
public:
    typedef _RandomIterator<T> Iterator;
    typedef _RandomReverseIterator<T> ReverseIterator;
    typedef _RandomIterator<const T> ConstIterator;
    typedef _RandomReverseIterator<const T> ConstReverseIterator;

protected:
    T* _data;
    size_t _capacity;
    size_t _size;
    _Alloc _alloc;

    // re-allocation
    void _resize(const size_t& old_size, const size_t& old_capacity, const size_t& new_size) {
        T *ret = _alloc.allocate(new_size);
        // enforce copying instead of moving
        for(int i = 0; i < Min(old_size, new_size); ++i) _alloc.construct(ret + i, std::move(*(_data + i)));
        for(int i = old_size - 1; i >= 0; --i) _alloc.destroy(_data + i);
        _alloc.deallocate(_data, old_capacity);
        _data = ret;
    }

public:
    // default
    Vector(): _data{NULL}, _capacity{0}, _size{0} { }
    // from size
    Vector(const size_t& size, const T& value): _capacity{size}, _size{0} {
        _data = _alloc.allocate(size);
        for(int i = 0; i < size; ++i) _alloc.construct(_data + _size++, value);
    }
    // from list
    Vector(std::initializer_list<T> l): _capacity{l.size()}, _size{0} {
        _data = _alloc.allocate(l.size());
        for(typename std::initializer_list<T>::const_iterator it = l.begin(); it != l.end(); ++it)
            _alloc.construct(_data + _size++, *it);
    }
    // copy (_capacity is not copied)
    Vector(const Vector& v): _capacity{v._size}, _size{0} {
        _data = _alloc.allocate(v._size);
        for(ConstIterator it = v.begin(); it != v.end(); ++it) _alloc.construct(_data + _size++, *it);
    }
    template <
        class InputIter, 
        // use only if template derives from _Iterator<T>
        typename = std::enable_if_t<std::is_base_of<_Iterator<T>, InputIter>::value>
    >
    Vector(const InputIter& first, const InputIter& last): _data{NULL}, _capacity{0}, _size{0} {
        for(InputIter it = first; it != last; ++it) push_back(*it);
    }
    // move
    Vector(Vector&& v): _data{v._data}, _capacity{v._capacity}, _size{v._size} {
        v._data = NULL;
        v._capacity = v._size = 0;
    }

    ~Vector() {
        for(int i = _size - 1; i >= 0; --i) _alloc.destroy(_data + i);
        _alloc.deallocate(_data, _capacity);
        _data = NULL;
        _capacity = _size = 0;
    }

    // iterator
    Iterator begin() { return Iterator(_data); }
    ConstIterator begin() const { return ConstIterator(_data); }
    Iterator end() { return Iterator(_data + _size); }
    ConstIterator end() const { return ConstIterator(_data + _size); }

    ReverseIterator rbegin() { return ReverseIterator(end()); }
    ConstReverseIterator rbegin() const { return ConstReverseIterator(end()); }
    ReverseIterator rend() { return ReverseIterator(begin()); }
    ConstReverseIterator rend() const { return ConstReverseIterator(begin()); }

    // copy assign
    Vector& operator=(const Vector& v) {
        if(&v != this) {
            for(int i = _size - 1; i >= 0; --i) _alloc.destroy(_data + i);
            _alloc.deallocate(_data, _capacity);
            _data = _alloc.allocate(v._data, v._size);
            for(int i = 0; i < v._size; ++i) _alloc.construct(_data + i, v._data[i]);
            _capacity = v._size;
            _size = v._size;
        }
        return *this;
    }

    // move assign
    Vector& operator=(Vector&& v) {
        if(&v != this) {
            for(int i = _size - 1; i >= 0; --i) _alloc.destroy(_data + i);
            _alloc.deallocate(_data, _capacity);
            _data = v._data;
            v._data = NULL;
            _size = v._size;
            v._size = 0;
            _capacity = v._capacity;
            v._capacity = 0;
        }
        return *this;
    }

    // size
    size_t size() const { return _size; }
    size_t capacity() const { return _capacity; }
    bool empty() const { return _size == 0; }

    // data
    const T* data() const { return _data; }

    // access
    T& operator[](const int& index) {
        if(index < 0 || index >= _size) throw std::out_of_range("vector index out of range");
        return _data[index];
    }
    const T& operator[](const int& index) const {
        if(index < 0 || index >= _size) throw std::out_of_range("vector index out of range");
        return _data[index];
    }
    // use operator[] to enforce range check
    T& front() { return operator[](0); }
    T& back() { return operator[](_size - 1); }

    // append with copy (rvalues are also allowed to bind to const lvalues)
    void push_back(const T& item) {
        if(_size + 1 > _capacity) {
            size_t new_capacity = 2 * (_size + 1);
            _resize(_size, _capacity, new_capacity);
            _capacity = new_capacity;
        }
        // fill the last element (enforce copy constructor, not assignment)
        _alloc.construct(_data + _size++, item);
    }
    // append with move
    void push_back(T&& item) {
        if(_size + 1 > _alloc.max_size()) throw std::overflow_error("vector push_back overflow");
        if(_size + 1 > _capacity) {
            size_t new_capacity = 2 * (_size + 1);
            _resize(_size, _capacity, new_capacity);
            _capacity = new_capacity;
        }
        // fill the last element (enforce move constructor)
        _alloc.construct(_data + _size++, std::move(item));
    }
    // pop_back will destroy objects
    void pop_back() {
        if(_size == 0) throw std::underflow_error("vector pop_back underflow");
        _alloc.destroy(_data + --_size);
    }

    // resize
    // this is a different version from STL as it do not re-initialize all elements
    // old elements are kept as much as possible
    // the whole vector will be reallocated
    void resize(const size_t& size) {
        _resize(_size, _capacity, size);
        _capacity = size;
        _size = size;
    }
    // shrink capacity to size
    void shrink() { resize(_size); }
    void clear() { resize(0); }

    // insert
    void insert(const Iterator& pos, const size_t& n, const T& item) {
        Vector tail;
        for(ReverseIterator it = rbegin(); it != pos; ++it) {
            tail.push_back(std::move(*it));
            pop_back();
        }
        // insert
        for(int i = 0; i < n; ++i) push_back(item);
        // append with move
        for(ReverseIterator it = tail.rbegin(); it != tail.rend(); ++it) {
            push_back(std::move(*it));
            tail.pop_back();
        }
    }
    void insert(const Iterator& pos, const Iterator& first, const Iterator& last) {
        Vector tail;
        for(ReverseIterator it = rbegin(); it != pos; ++it) {
            tail.push_back(std::move(*it));
            pop_back();
        }
        // insert
        for(auto it = first; it != last; ++it) push_back(*it);
        // append with move
        for(ReverseIterator it = tail.rbegin(); it != tail.rend(); ++it) {
            push_back(std::move(*it));
            tail.pop_back();
        }
    }
};
