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

#define test_number(expect, content)              \
    do                                            \
    {                                             \
        SJson::Json j;                            \
        j.Parse(content, status);                 \
        EXPECT_EQ("parse ok", status);            \
        EXPECT_EQ(JsonType::Number, j.GetType()); \
        EXPECT_EQ(expect, j.GetNumber());         \
    } while (0)

TEST(TestNumber, Number)
{
    using namespace SJson;
    test_number(0.0, "0");
    test_number(0.0, "-0");
    test_number(0.0, "-0.0");
    test_number(1.0, "1");
    test_number(-1.0, "-1");
    test_number(1.5, "1.5");
    test_number(-1.5, "-1.5");
    test_number(3.1416, "3.1416");
    test_number(1E10, "1E10");
    test_number(1e10, "1e10");
    test_number(1E+10, "1E+10");
    test_number(1E-10, "1E-10");
    test_number(-1E10, "-1E10");
    test_number(-1e10, "-1e10");
    test_number(-1E+10, "-1E+10");
    test_number(-1E-10, "-1E-10");
    test_number(1.234E+10, "1.234E+10");
    test_number(1.234E-10, "1.234E-10");
    test_number(0.0, "1e-10000"); /* must underflow */
}

#define test_string(expect, content)                 \
    do                                               \
    {                                                \
        SJson::Json j;                               \
        j.Parse(content, status);                    \
        EXPECT_EQ("parse ok", status);               \
        EXPECT_EQ(JsonType::String, j.GetType());    \
        EXPECT_STREQ(expect, j.GetString().c_str()); \
    } while (0)

TEST(TestString, String)
{
    using namespace SJson;

    test_string("", "\"\"");
    test_string("Hello", "\"Hello\"");

    test_string("Hello\nWorld", "\"Hello\\nWorld\"");
    test_string("\" \\ / \b \f \n \r \t", "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");

    test_string("Hello\0World", "\"Hello\\u0000World\"");
    test_string("\x24", "\"\\u0024\"");                    /* Dollar sign U+0024 */
    test_string("\xC2\xA2", "\"\\u00A2\"");                /* Cents sign U+00A2 */
    test_string("\xE2\x82\xAC", "\"\\u20AC\"");            /* Euro sign U+20AC */
    test_string("\xF0\x9D\x84\x9E", "\"\\uD834\\uDD1E\""); /* G clef sign U+1D11E */
    test_string("\xF0\x9D\x84\x9E", "\"\\ud834\\udd1e\""); /* G clef sign U+1D11E */
}