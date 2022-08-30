// basic iterator 
// https://sourceforge.net/project/showfiles.php?group_id=146814 

#pragma once

#include <cstddef>


// basic iterator
template <class T, class DifferenceType = const ptrdiff_t&>
class _Iterator {
protected: // in case of inheritance
    T *_cur;

public:
    _Iterator(T* cur = NULL): _cur{cur} { }
    _Iterator(const _Iterator& it): _cur{it._cur} { }

    _Iterator& operator=(const _Iterator& it) {
        _cur = it._cur;
        return *this;
    }

    T* base() const { return _cur; }
    T& operator*() { return *_cur; }

    _Iterator& operator++() {
        ++_cur;
        return *this;
    }
    _Iterator operator++(int) {
        _Iterator it(*this);
        ++_cur;
        return it;
    }
    _Iterator& operator--() {
        --_cur;
        return *this;
    }
    _Iterator operator--(int) {
        _Iterator ret(*this);
        --_cur;
        return ret;
    }
};


// declaration
template <class T, class D> class _RandomIterator;


template <class T, class DifferenceType = const ptrdiff_t&>
class _ReverseIterator: public _Iterator<T, DifferenceType> {
public:
    _ReverseIterator(const _Iterator<T, DifferenceType>& it): _Iterator<T, DifferenceType>(it) { }

    T* base() const { return this->_cur; }
    T& operator*() { return *(this->_cur - 1); }

    _ReverseIterator& operator++() {
        --this->_cur;
        return *this;
    }
    _ReverseIterator operator++(int) {
        _ReverseIterator it(*this);
        --this->_cur;
        return it;
    }
    _ReverseIterator& operator--() {
        ++this->_cur;
        return *this;
    }
    _ReverseIterator operator--(int) {
        _ReverseIterator ret(*this);
        ++this->_cur;
        return ret;
    }
};


// random access iterator, memory must be continuous by default
template <class T, class DifferenceType = const std::ptrdiff_t&>
class _RandomIterator : public _Iterator<T, DifferenceType> {
public:
    _RandomIterator(const _Iterator<T, DifferenceType>& it): _Iterator<T, DifferenceType>(it) { }

    // must specify this->_cur (compiler doesn't know the dependence)
    _RandomIterator operator+(const DifferenceType diff) { return _RandomIterator(this->_cur + diff); }
    _RandomIterator& operator+=(const DifferenceType diff) {
        this->_cur += diff;
        return *this;
    }
    _RandomIterator operator-(const DifferenceType diff) { return _RandomIterator(this->_cur - diff); }
    _RandomIterator& operator-=(const DifferenceType diff) {
        this->_cur -= diff;
        return *this;
    }

    T& operator[](DifferenceType diff) { return *(this->_cur + diff); }
};


template <class T, class DifferenceType = const std::ptrdiff_t&>
class _RandomReverseIterator : public _ReverseIterator<T, DifferenceType> {
public:
    using _ReverseIterator<T, DifferenceType>::_ReverseIterator;
    // can only list initialize direct base class
    _RandomReverseIterator(const _Iterator<T, DifferenceType>& it): _ReverseIterator<T, DifferenceType>(it) { }

    // must specify this->_cur (compiler doesn't know the dependence)
    _RandomReverseIterator operator+(const DifferenceType diff) { return _RandomReverseIterator(this->_cur - diff); }
    _RandomReverseIterator& operator+=(const DifferenceType diff) {
        this->_cur -= diff;
        return *this;
    }
    _RandomReverseIterator operator-(const DifferenceType diff) { return _RandomReverseIterator(this->_cur + diff); }
    _RandomReverseIterator& operator-=(const DifferenceType diff) {
        this->_cur += diff;
        return *this;
    }

    T& operator[](DifferenceType diff) { return *(this->_cur - diff); }
};


template <class T, class D>
bool operator==(const _Iterator<T, D>& lhs, const _Iterator<T, D>& rhs) { return lhs.base() == rhs.base(); }

template <class T, class D>
bool operator!=(const _Iterator<T, D>& lhs, const _Iterator<T, D>& rhs) { return lhs.base() != rhs.base(); }

template <class T, class D>
bool operator<(const _Iterator<T, D>& lhs, const _Iterator<T, D>& rhs) { return lhs.base() < rhs.base(); }

template <class T, class D>
ptrdiff_t operator-(const _RandomIterator<T, D>& lhs, const _RandomIterator<T, D>& rhs) { return lhs.base() - rhs.base(); }

template <class T, class D>
ptrdiff_t operator-(const _RandomReverseIterator<T, D>& lhs, const _RandomReverseIterator<T, D>& rhs) { return rhs.base() - lhs.base(); }
