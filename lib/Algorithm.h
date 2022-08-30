// algorithms

#pragma once


// requires copy assignment
template <class T>
inline void Swap(T& a, T& b) {
    T tmp = a;
    a = b;
    b = tmp;
}

// requires operator< overloaded
template <class T>
inline const T& Min(const T& a, const T& b) { return a <= b ? a : b; }

// requires iterator
template<class Iter>
void Reverse(const Iter& first, const Iter& last) {
    Iter left = first, right = last;
    while(left < right) Swap(*(left++), *(--right));
}
