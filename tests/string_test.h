// string test

#include <gtest/gtest.h>
#include <exception>

#include "../lib/String.h"


TEST(StringTest, Constructors) {
    // default
    String s0;
    EXPECT_STREQ(s0.c_str(), "");
    // destruct
    s0.~String();
    EXPECT_EQ(s0.c_str(), (const char*)NULL);
    // c str
    String s1("hello");
    EXPECT_STREQ(s1.c_str(), "hello");
    // copy
    String s2(s1);
    EXPECT_STREQ(s1.c_str(), s2.c_str());
    // len
    EXPECT_EQ(strlen("hello"), 5);
    EXPECT_EQ(s2.length(), 5);
}

TEST(StringTest, Assignments) {
    // copy
    String s0("abc");
    String s1 = s0;
    EXPECT_TRUE(s0 == s1);
    // move
    s1 = String("def");
    EXPECT_EQ(strcmp(s1.c_str(), "def"), 0);
}

TEST(StringTest, Operators) {
    // +
    String s0("abc");
    EXPECT_TRUE(s0 == "abc");
    String s1("def");
    String s2 = s0 + s1;
    EXPECT_TRUE(s2 == "abcdef");
    // +=
    for(int i = 0; i < 10; i++) {
        s0 += 'a' + i;
    }
    EXPECT_TRUE(s0 == "abcabcdefghij");
    EXPECT_EQ(s0.length(), 13);
    // []
    EXPECT_EQ(s0[3], 'a');
    // char*
    EXPECT_STREQ((char*)s1, "def");
}

TEST(StringTest, Exceptions) {
    // out of range
    String s0("abc");
    EXPECT_THROW(s0[5], std::out_of_range);
}

TEST(StringTest, Iterator) {
    char c[] = "abc";
    String s0(c);
    int i = 0;
    for(String::Iterator it = s0.begin(); it != s0.end(); it++, i++) {
        EXPECT_EQ(*it, c[i]);
        ASSERT_LT(i, 3);
    }
    EXPECT_EQ(i, 3);
    EXPECT_EQ(*s0.find('c'), 'c');
    EXPECT_TRUE(s0.find('c') == s0.begin() + 2);
}

TEST(StringTest, Regex) {
    String s0("harry1501potter");
    EXPECT_TRUE(s0.match(".*"));
    EXPECT_TRUE(s0.match("^h.*r$"));
    EXPECT_TRUE(s0.match("^har*y.*pot.*er$"));
    EXPECT_FALSE(s0.match("^har*y..pot.*er$"));
}
