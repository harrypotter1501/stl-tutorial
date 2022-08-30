// string

#pragma once

#include <iostream>
#include <stdexcept>

#include "Allocator.h"
#include "Iterator.h"


template <class T, class Alloc>
class _String {
public:
    // iterator (random access, no need to change)
    typedef _RandomIterator<T> Iterator;
    typedef _RandomIterator<const T> ConstIterator;

protected:
    // c str ptr
    T *_data;
    // length
    size_t _len;
    // memory capacity
    size_t _capacity;
    // allocator
    Alloc _alloc;

    void strcpy(T* dst, const T* src) {
        size_t i = 0;
        while(src[i] != (T)'\0') dst[i] = src[i], ++i;
        dst[i] = (T)'\0';
    }

public:
    // default
    _String(): _len{0}, _capacity{_len + 1} {
        _data = _alloc.allocate(_capacity);
        _alloc.construct(_data, '\0');
    }
    // from c str
    _String(const T* s) {
        if(s == NULL) throw std::invalid_argument("cannot initialize with nullptr");
        _len = strlen(s);
        _capacity = _len + 1;
        _data = _alloc.allocate(_capacity);
        strcpy(_data, s);
    }
    // copy
    _String(const _String& s): _len{s._len}, _capacity{s._capacity}, _alloc{s._alloc} {
        _data = _alloc.allocate(_capacity);
        strcpy(_data, s._data);
    }
    // move
    _String(_String&& s): _len{s._len}, _capacity{s._capacity}, _alloc{s._alloc} {
        _data = s._data;
        s._data = NULL;
        s._len = 0;
        s._capacity = 0;
    }
    ~_String() {
        for(size_t i = 0; i < _capacity; ++i) _alloc.destroy(_data + i);
        _alloc.deallocate(_data, _capacity);
        _data = NULL;
        _len = 0;
        _capacity = 0;
    }

    Iterator begin() { return Iterator(_data); }
    ConstIterator begin() const { return ConstIterator(_data); }
    Iterator end() { return Iterator(_data + _len); }
    ConstIterator end() const { return ConstIterator(_data + _len); }

    // search
    Iterator find(const T c) const {
        size_t i;
        for(i = 0; i < _len; ++i) {
            if(_data[i] == c) break;
        }
        return Iterator(_data + i);
    }

    // copy assign
    _String& operator=(const _String& s) {
        if(&s != this) {
            for(size_t i = 0; i < _capacity; ++i) _alloc.destroy(_data + i);
            _alloc.deallocate(_data, _capacity);
            _data = _alloc.allocate(s._capacity);
            _capacity = s._capacity;
            strcpy(_data, s._data);
            _len = s._len;
        }
        return *this;
    }
    // move assign
    _String& operator=(_String&& s) {
        if(&s != this) {
            for(size_t i = 0; i < _capacity; ++i) _alloc.destroy(_data + i);
            _alloc.deallocate(_data, _capacity);
            _data = s._data;
            // set this to NULL to avoid repeated destruction
            s._data = NULL;
            _capacity = s._capacity;
            // clear capacity
            s._capacity = 0;
            _len = s._len;
            s._len = 0;
        }
        return *this;
    }

    // c string
    const T* c_str() const { return _data; }
    // len
    size_t length() const { return _len; }

    // append
    _String& operator+=(const T c) {
        if(_len + 1 >= _capacity) {
            // double capacity if theres no space
            T* new_data = _alloc.allocate(_capacity * 2);
            strcpy(new_data, _data);
            for(size_t i = 0; i < _capacity; ++i)  _alloc.destroy(_data + i);
            _alloc.deallocate(_data, _capacity);
            _data = new_data;
            // make sure allocation is a success before changing the value of capacity
            _capacity *= 2;
        }
        _data[_len++] = c;
        return *this;
    }

    _String& operator+=(const _String& s) {
        if(_len + s._len >= _capacity) {
            T* new_data = _alloc.allocate(_capacity + s._capacity);
            // destroy & copy old
            strcpy(new_data, _data);
            for(size_t i = 0; i < _capacity; ++i) _alloc.destroy(_data + i);
            _alloc.deallocate(_data, _capacity);
            _data = new_data;
            _capacity += s._capacity;
        }
        // copy new
        strcpy(_data + _len, s._data);
        _len += s._len;
        return *this;
    }

    // cast
    explicit operator T*() const { return _data; }

    // access
    T& operator[](const int index) {
        if(index < 0 || index >= _len) throw std::out_of_range("string index out of range");
        return _data[index];
    }
    const T& operator[](const int index) const { return _data[index]; }

    // regex
    bool match(const T* regex) const;
};


// define the basic string
typedef _String<char, _Allocator<char>> String;


String operator+(const String& lhs, const String& rhs) {
    String ret(lhs);
    ret += rhs;
    return ret;
}

std::ostream& operator<<(std::ostream& os, const String& s) {
    return os << s.c_str();
}

bool operator==(const String& lhs, const String& rhs) {
    return strcmp(lhs.c_str(), rhs.c_str()) == 0;
}

// regex: https://www.cs.princeton.edu/courses/archive/spr09/cos333/beautiful.html
bool match_any(const char c, const char* regex, const char* text);

bool match_start(const char* regex, const char* text) {
    // empty regex
    if(regex[0] == '\0') return true;
    // match *
    if(regex[1] == '*') return match_any(regex[0], regex + 2, text);
    // match end
    if(regex[0] == '$' && regex[1] == '\0') return text[0] == '\0';
    // recursive
    if(text[0] != '\0' && (regex[0] == '.' || regex[0] == text[0]))
        return match_start(regex + 1, text + 1);
    return false;
}

bool match_any(const char c, const char* regex, const char* text) {
    do {
        // match zero or more
        if(match_start(regex, text)) return true;
    } while(text[0] != '\0' && (*text++ == c || c == '.'));
    return false;
}

bool match_regex(const char* regex, const char* text) {
    // check ^ specifier
    if(regex[0] == '^') return match_start(regex + 1, text);
    do {
        // match from every position
        if(match_start(regex, text)) return true;
    } while(*text++ != '\0');
    // no match
    return false;
}

template <class T, class Alloc>
bool _String<T, Alloc>::match(const T* regex) const { return match_regex(regex, _data); }
