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

TEST(TestArray, Array)
{
    using namespace SJson;
    SJson::Json j;

    j.Parse("[ ]", status);
    EXPECT_EQ("parse ok", status);
    EXPECT_EQ(JsonType::Array, j.GetType());
    EXPECT_EQ(0, j.GetArraySize());

    j.Parse("[ null , false , true , 123 , \"abc\" ]", status);
    EXPECT_EQ("parse ok", status);
    EXPECT_EQ(JsonType::Array, j.GetType());
    EXPECT_EQ(5, j.GetArraySize());
    EXPECT_EQ(JsonType::Null, j.GetArrayElement(0).GetType());
    EXPECT_EQ(JsonType::False, j.GetArrayElement(1).GetType());
    EXPECT_EQ(JsonType::True, j.GetArrayElement(2).GetType());
    EXPECT_EQ(JsonType::Number, j.GetArrayElement(3).GetType());
    EXPECT_EQ(JsonType::String, j.GetArrayElement(4).GetType());
    EXPECT_EQ(123.0, j.GetArrayElement(3).GetNumber());
    EXPECT_STREQ("abc", j.GetArrayElement(4).GetString().c_str());

    j.Parse("[ [ ] , [ 0 ] , [ 0 , 1 ] , [ 0 , 1 , 2 ] ]", status);
    EXPECT_EQ("parse ok", status);
    EXPECT_EQ(JsonType::Array, j.GetType());
    EXPECT_EQ(4, j.GetArraySize());
    for (int i = 0; i < 4; i++)
    {
        SJson::Json a = j.GetArrayElement(i);
        EXPECT_EQ(JsonType::Array, a.GetType());
        for (int k = 0; k < i; k++)
        {
            SJson::Json b = a.GetArrayElement(k);
            EXPECT_EQ(JsonType::Number, b.GetType());
            EXPECT_EQ((double)k, b.GetNumber());
        }
    }
}

TEST(TestObject, Object)
{
    using namespace SJson;
    SJson::Json v;

    v.Parse(" { } ", status);
    EXPECT_EQ("parse ok", status);
    EXPECT_EQ(JsonType::Object, v.GetType());
    EXPECT_EQ(0, v.GetObjectSize());

    v.Parse(" { "
            "\"n\" : null , "
            "\"f\" : false , "
            "\"t\" : true , "
            "\"i\" : 123 , "
            "\"s\" : \"abc\", "
            "\"a\" : [ 1, 2, 3 ],"
            "\"o\" : { \"1\" : 1, \"2\" : 2, \"3\" : 3 }"
            " } ",
            status);
    EXPECT_EQ("parse ok", status);
    EXPECT_EQ(JsonType::Object, v.GetType());
    EXPECT_EQ(7, v.GetObjectSize());
    EXPECT_EQ("n", v.GetObjectKey(0));
    EXPECT_EQ(JsonType::Null, v.GetObjectValue(0).GetType());
    EXPECT_EQ("f", v.GetObjectKey(1));
    EXPECT_EQ(JsonType::False, v.GetObjectValue(1).GetType());
    EXPECT_EQ("t", v.GetObjectKey(2));
    EXPECT_EQ(JsonType::True, v.GetObjectValue(2).GetType());
    EXPECT_EQ("i", v.GetObjectKey(3));
    EXPECT_EQ(JsonType::Number, v.GetObjectValue(3).GetType());
    EXPECT_EQ(123.0, v.GetObjectValue(3).GetNumber());
    EXPECT_EQ("s", v.GetObjectKey(4));
    EXPECT_EQ(JsonType::String, v.GetObjectValue(4).GetType());
    EXPECT_EQ("abc", v.GetObjectValue(4).GetString());
    EXPECT_EQ("a", v.GetObjectKey(5));
    EXPECT_EQ(JsonType::Array, v.GetObjectValue(5).GetType());
    EXPECT_EQ(3, v.GetObjectValue(5).GetArraySize());
    for (int i = 0; i < 3; ++i)
    {
        SJson::Json e = v.GetObjectValue(5).GetArrayElement(i);
        EXPECT_EQ(JsonType::Number, e.GetType());
        EXPECT_EQ(i + 1.0, e.GetNumber());
    }
    EXPECT_EQ("o", v.GetObjectKey(6));
    {
        SJson::Json o = v.GetObjectValue(6);
        EXPECT_EQ(JsonType::Object, o.GetType());
        for (int i = 0; i < 3; i++)
        {
            SJson::Json ov = o.GetObjectValue(i);
            EXPECT_EQ('1' + i, (o.GetObjectKey(i))[0]); // (o.get_object_key(i))[0] 表示获得string的第一个字符
            EXPECT_EQ(1, o.GetObjectKeyLength(i));
            EXPECT_EQ(JsonType::Number, ov.GetType());
            EXPECT_EQ(i + 1.0, ov.GetNumber());
        }
    }
}