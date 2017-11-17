#include "time.h"

#include <iostream>
#include <gtest/gtest.h>

TEST(Common, Test) {
    std::cout << utils::time::CurUTCTimeStamp() << std::endl;
}
