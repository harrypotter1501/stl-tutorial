// list test

#pragma once

#include <gtest/gtest.h>
#include <exception>

#include "../lib/List.h"


#define EXPECT_LSTEQ(l, a, n) EXPECT_TRUE(list_eq(l, a, n))


template <typename T> bool list_eq(const List<T>& l, const T* a, const int& n) {
    if(l.size() != n) return false;
    auto *p = l.head();
    while(p != NULL) {
        if(p->_data != *a++) return false;
        p = p->_next;
    }
    return true;
}


TEST(ListTest, Constructors) {
    // default
    List<int> l0;
    EXPECT_EQ(l0.size(), 0);
    EXPECT_EQ(l0.head(), (List<int>::Node*)NULL);
    // size
    List<char> l1(5, 'c');
    char arr[] = {'c', 'c', 'c', 'c', 'c'};
    EXPECT_LSTEQ(l1, arr, 5);
}
