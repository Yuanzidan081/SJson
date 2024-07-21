#include <iostream>
#include <string>
#include "../src/Json.h"

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;
static std::string status;

#define EXPECT_EQ_BASE(expect, actual)                                                                                \
    do                                                                                                                \
    {                                                                                                                 \
        ++test_count;                                                                                                 \
        if (expect == actual)                                                                                         \
            ++test_pass;                                                                                              \
        else                                                                                                          \
        {                                                                                                             \
            std::cerr << __FILE__ << ":" << __LINE__ << ": expect: " << expect << " actual: " << actual << std::endl; \
            main_ret = 1;                                                                                             \
        }                                                                                                             \
    } while (0)

int main()
{
    /* test_parse();
    test_stringify();
    test_equal();
    test_copy();
    test_move();
    test_swap();
    test_access(); */
    printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
    return main_ret;
}