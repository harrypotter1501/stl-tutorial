// all tests

#include <iostream>
#include "gtest/gtest.h"

#include "tests/all_tests.h"


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    //::testing::GTEST_FLAG(filter) = "StringTest*";
    //::testing::GTEST_FLAG(filter) = "VectorTest*";
    return RUN_ALL_TESTS();
}
