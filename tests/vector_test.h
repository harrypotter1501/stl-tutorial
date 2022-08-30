// vector test

#include <gtest/gtest.h>
#include <exception>

#include "../lib/Vector.h"


#define EXPECT_ARREQ(a1, a2, n) EXPECT_TRUE(array_eq(a1, a2, n))

template <typename T>
bool array_eq(const Vector<T> v, const T* a, const int& n) {
    if(v.size() != n) return false;
    for(int i = 0; i < n; ++i) {
        if(v[i] != a[i]) return false;
    }
    return true;
}


TEST(VectorTest, Constructors) {
    // default
    Vector<int> v0;
    EXPECT_EQ(v0.data(), (int*)NULL);
    // from size
    Vector<char> v1(5, 'a');
    char arr[] = {'a', 'a', 'a', 'a', 'a'};
    EXPECT_ARREQ(v1, arr, 5);
    // copy
    Vector<char> v2(v1);
    EXPECT_ARREQ(v1, v2.data(), 5);
    // move
    Vector<char> v3(std::forward<Vector<char>&&>(v2));
    EXPECT_EQ(v2.data(), (char*)NULL);
    EXPECT_ARREQ(v3, arr, 5);
    // from list
    Vector<int> v4({1, 3, 5, 7});
    int ia[] = {1, 3, 5, 7};
    EXPECT_ARREQ(v4, ia, 4);
}

TEST(VectorTest, Operators) {
    // copy
    Vector<int> v0(3, 1);
    Vector<int> v1 = v0;
    EXPECT_ARREQ(v0, v1.data(), 3);
    // move
    Vector<int> v2 = std::forward<Vector<int>&&>(v0);
    EXPECT_ARREQ(v1, v2.data(), 3);
    // []
    EXPECT_EQ(v1[0], 1);
    EXPECT_THROW(v1[3], std::out_of_range);
    // ==
    EXPECT_ARREQ(v1, v2.data(), 3);
    EXPECT_ARREQ(v1, v2.data(), 3);
}

TEST(VectorTest, Modify) {
    Vector<int> v0;
    v0.push_back(11);
    EXPECT_EQ(v0.size(), 1);
    EXPECT_EQ(v0.front(), 11);
    for(int i = 0; i < 10; ++i) v0.push_back(i);
    int arr[] = {11, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_EQ(v0.size(), 11);
    EXPECT_ARREQ(v0, arr, 11);
    v0.resize(6);
    for(int i = 5; i >=0; --i) {
        EXPECT_EQ(v0.back(), arr[i]);
        v0.pop_back();
    }
    EXPECT_TRUE(v0.empty());
}

TEST(VectorTest, Iterator) {
    Vector<int> v0({1, 3, 5, 7});
    int arr[] = {1, 3, 5, 7};
    EXPECT_ARREQ(v0, arr, 4);
    EXPECT_EQ(*(v0.begin() + 2), 5);
    EXPECT_EQ(*(v0.end() - 1), 7);
    // insert
    v0.insert(v0.begin() + 2, 3, 9);
    int arr2[] = {1, 3, 9, 9, 9, 5 ,7};
    EXPECT_ARREQ(v0, arr2, 7);
    Vector<int> v1(v0.rbegin(), v0.rend());
    Reverse(v1.begin(), v1.end());
    v0.insert(v0.begin() + 2, v1.begin(), v1.end());
    int arr3[] = {1, 3, 1, 3, 9, 9, 9, 5, 7, 9, 9, 9, 5 ,7};
    EXPECT_ARREQ(v0, arr3, 14);
}

TEST(VectorTest, Template) {
    class Dummy {
    public:
        int *cnt;
        char *c;
        Dummy(int* n): cnt{n} {
            //std::cout << "construct" << std::endl;
            ++*cnt;
            c = new char[10];
            strcpy(c, "what");
        }
        Dummy(const Dummy& d): cnt{d.cnt} {
            //std::cout << "copy" << std::endl;
            ++*cnt;
            c = new char[10];
            strcpy(c, "what");
        }
        Dummy(Dummy&& d): cnt{d.cnt}, c{d.c = NULL} {
            ++*cnt;
            //std::cout << "move" << std::endl;
        }
        // don't allow copy construct
        Dummy& operator=(const Dummy& d) = delete;
        ~Dummy() {
            delete[] c;
            --*cnt;
            //std::cout << "destroy" << std::endl;
        }
    };
    int cnt = 0;
    Vector<Dummy> v0;
    Dummy d(&cnt);
    for(int i = 0; i < 5; ++i) v0.push_back(d);
    EXPECT_EQ(cnt, 6);
    v0.pop_back();
    v0.pop_back();
    EXPECT_EQ(v0.size(), 3);
    EXPECT_EQ(v0.capacity(), 6);
    EXPECT_EQ(cnt, 4);
    v0.shrink();
    EXPECT_EQ(v0.size(), 3);
    EXPECT_EQ(v0.capacity(), 3);
    for(int i = 0; i < 3; ++i) v0.push_back(Dummy(&cnt));
    EXPECT_EQ(cnt, 7);
    EXPECT_EQ(v0.size(), 6);
    EXPECT_EQ(v0.capacity(), 8);
    v0.clear();
    EXPECT_EQ(cnt, 1);
    EXPECT_EQ(v0.size(), 0);
    EXPECT_EQ(v0.capacity(), 0);
}
