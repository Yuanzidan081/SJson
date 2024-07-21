#include <gtest/gtest.h>
#include "../src/Json.h"
#include <string>

static std::string status;

#define test_literal(expect, content)   \
    do                                  \
    {                                   \
        SJson::Json j;                  \
        j.SetBoolean(false);            \
        j.Parse(content, status);       \
        EXPECT_EQ("parse ok", status);  \
        EXPECT_EQ(expect, j.GetType()); \
    } while (0)

TEST(TestLiteral, NullTrueFalse)
{
    using namespace SJson;
    test_literal(JsonType::Null, "null");
    test_literal(JsonType::True, "true");
    test_literal(JsonType::False, "false");
}
