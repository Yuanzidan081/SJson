#include <gtest/gtest.h>
#include "../src/Json.h"
#include <memory>

using namespace std;

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    int status = RUN_ALL_TESTS();
    return 0;
}