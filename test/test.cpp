#include <iostream>
#include <string>
#include "../src/Json.h"

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;
static std::string status;
using namespace SJson;
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

#define TEST_LITERAL(expect, content)        \
    do                                       \
    {                                        \
        SJson::Json v;                       \
        v.SetBoolean(false);                 \
        v.Parse(content, status);            \
        EXPECT_EQ_BASE("parse ok", status);  \
        EXPECT_EQ_BASE(expect, v.GetType()); \
    } while (0)

static void test_parse_literal()
{
    TEST_LITERAL(JsonType::Null, "null");
    TEST_LITERAL(JsonType::True, "true");
    TEST_LITERAL(JsonType::False, "false");
}

#define TEST_NUMBER(expect, content)                   \
    do                                                 \
    {                                                  \
        SJson::Json j;                                 \
        j.Parse(content, status);                      \
        EXPECT_EQ_BASE("parse ok", status);            \
        EXPECT_EQ_BASE(JsonType::Number, j.GetType()); \
        EXPECT_EQ_BASE(expect, j.GetNumber());         \
    } while (0)

static void test_parse_number()
{
    TEST_NUMBER(0.0, "0");
    TEST_NUMBER(0.0, "-0");
    TEST_NUMBER(0.0, "-0.0");
    TEST_NUMBER(1.0, "1");
    TEST_NUMBER(-1.0, "-1");
    TEST_NUMBER(1.5, "1.5");
    TEST_NUMBER(-1.5, "-1.5");
    TEST_NUMBER(3.1416, "3.1416");
    TEST_NUMBER(1E10, "1E10");
    TEST_NUMBER(1e10, "1e10");
    TEST_NUMBER(1E+10, "1E+10");
    TEST_NUMBER(1E-10, "1E-10");
    TEST_NUMBER(-1E10, "-1E10");
    TEST_NUMBER(-1e10, "-1e10");
    TEST_NUMBER(-1E+10, "-1E+10");
    TEST_NUMBER(-1E-10, "-1E-10");
    TEST_NUMBER(1.234E+10, "1.234E+10");
    TEST_NUMBER(1.234E-10, "1.234E-10");
    TEST_NUMBER(0.0, "1e-10000"); /* must underflow */
}

#define TEST_STRING(expect, content)                                                    \
    do                                                                                  \
    {                                                                                   \
        SJson::Json v;                                                                  \
        v.Parse(content, status);                                                       \
        EXPECT_EQ_BASE("parse ok", status);                                             \
        EXPECT_EQ_BASE(JsonType::String, v.GetType());                                  \
        EXPECT_EQ_BASE(0, memcmp(expect, v.GetString().c_str(), v.GetString().size())); \
    } while (0)

#define EXPECT_EQ_STRING(expect, actual) EXPECT_EQ_BASE(0, memcmp(expect, actual.c_str(), actual.size()));

static void test_parse_string()
{

    TEST_STRING("", "\"\"");
    TEST_STRING("Hello", "\"Hello\"");

    TEST_STRING("Hello\nWorld", "\"Hello\\nWorld\"");
    TEST_STRING("\" \\ / \b \f \n \r \t", "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");

    TEST_STRING("Hello\0World", "\"Hello\\u0000World\"");
    TEST_STRING("\x24", "\"\\u0024\"");                    /* Dollar sign U+0024 */
    TEST_STRING("\xC2\xA2", "\"\\u00A2\"");                /* Cents sign U+00A2 */
    TEST_STRING("\xE2\x82\xAC", "\"\\u20AC\"");            /* Euro sign U+20AC */
    TEST_STRING("\xF0\x9D\x84\x9E", "\"\\uD834\\uDD1E\""); /* G clef sign U+1D11E */
    TEST_STRING("\xF0\x9D\x84\x9E", "\"\\ud834\\udd1e\""); /* G clef sign U+1D11E */
}

static void test_parse_array()
{
    SJson::Json v;

    v.Parse("[ ]", status);
    EXPECT_EQ_BASE("parse ok", status);
    EXPECT_EQ_BASE(JsonType::Array, v.GetType());
    EXPECT_EQ_BASE(0, v.GetArraySize());

    v.Parse("[ null , false , true , 123 , \"abc\" ]", status);
    EXPECT_EQ_BASE("parse ok", status);
    EXPECT_EQ_BASE(JsonType::Array, v.GetType());
    EXPECT_EQ_BASE(5, v.GetArraySize());
    EXPECT_EQ_BASE(JsonType::Null, v.GetArrayElement(0).GetType());
    EXPECT_EQ_BASE(JsonType::False, v.GetArrayElement(1).GetType());
    EXPECT_EQ_BASE(JsonType::True, v.GetArrayElement(2).GetType());
    EXPECT_EQ_BASE(JsonType::Number, v.GetArrayElement(3).GetType());
    EXPECT_EQ_BASE(JsonType::String, v.GetArrayElement(4).GetType());
    EXPECT_EQ_BASE(123.0, v.GetArrayElement(3).GetNumber());
    EXPECT_EQ_STRING("abc", v.GetArrayElement(4).GetString());

    v.Parse("[ [ ] , [ 0 ] , [ 0 , 1 ] , [ 0 , 1 , 2 ] ]", status);
    EXPECT_EQ_BASE("parse ok", status);
    EXPECT_EQ_BASE(JsonType::Array, v.GetType());
    EXPECT_EQ_BASE(4, v.GetArraySize());
    for (int i = 0; i < 4; ++i)
    {
        SJson::Json a = v.GetArrayElement(i);
        EXPECT_EQ_BASE(JsonType::Array, a.GetType());
        EXPECT_EQ_BASE(i, a.GetArraySize());
        for (int j = 0; j < i; ++j)
        {
            SJson::Json e = a.GetArrayElement(j);
            EXPECT_EQ_BASE(JsonType::Number, e.GetType());
            EXPECT_EQ_BASE((double)j, e.GetNumber());
        }
    }
}

static void test_parse_object()
{
    SJson::Json v;

    v.Parse(" { } ", status);
    EXPECT_EQ_BASE("parse ok", status);
    EXPECT_EQ_BASE(JsonType::Object, v.GetType());
    EXPECT_EQ_BASE(0, v.GetObjectSize());

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
    EXPECT_EQ_BASE("parse ok", status);
    EXPECT_EQ_BASE(7, v.GetObjectSize());
    EXPECT_EQ_BASE("n", v.GetObjectKey(0));
    EXPECT_EQ_BASE(JsonType::Null, v.GetObjectValue(0).GetType());
    EXPECT_EQ_BASE("f", v.GetObjectKey(1));
    EXPECT_EQ_BASE(JsonType::False, v.GetObjectValue(1).GetType());
    EXPECT_EQ_BASE("t", v.GetObjectKey(2));
    EXPECT_EQ_BASE(JsonType::True, v.GetObjectValue(2).GetType());
    EXPECT_EQ_BASE("i", v.GetObjectKey(3));
    EXPECT_EQ_BASE(JsonType::Number, v.GetObjectValue(3).GetType());
    EXPECT_EQ_BASE(123.0, v.GetObjectValue(3).GetNumber());
    EXPECT_EQ_BASE("s", v.GetObjectKey(4));
    EXPECT_EQ_BASE(JsonType::String, v.GetObjectValue(4).GetType());
    EXPECT_EQ_BASE("abc", v.GetObjectValue(4).GetString());
    EXPECT_EQ_BASE("a", v.GetObjectKey(5));
    EXPECT_EQ_BASE(JsonType::Array, v.GetObjectValue(5).GetType());
    EXPECT_EQ_BASE(3, v.GetObjectValue(5).GetArraySize());
    for (int i = 0; i < 3; ++i)
    {
        SJson::Json e = v.GetObjectValue(5).GetArrayElement(i);
        EXPECT_EQ_BASE(JsonType::Number, e.GetType());
        EXPECT_EQ_BASE(i + 1.0, e.GetNumber());
    }
    EXPECT_EQ_BASE("o", v.GetObjectKey(6));
    {
        SJson::Json o = v.GetObjectValue(6);
        EXPECT_EQ_BASE(JsonType::Object, o.GetType());
        for (int i = 0; i < 3; ++i)
        {
            SJson::Json ov = o.GetObjectValue(i);
            EXPECT_EQ_BASE('1' + i, (o.GetObjectKey(i))[0]);
            EXPECT_EQ_BASE(1, o.GetObjectKeyLength(i));
            EXPECT_EQ_BASE(JsonType::Number, ov.GetType());
            EXPECT_EQ_BASE(i + 1.0, ov.GetNumber());
        }
    }
}

#define TEST_ERROR(error, content)                     \
    do                                                 \
    {                                                  \
        SJson::Json v;                                 \
        v.Parse(content, status);                      \
        EXPECT_EQ_BASE(error, status);                 \
        EXPECT_EQ_BASE((JsonType::Null), v.GetType()); \
    } while (0)

static void test_parse_expect_value()
{
    TEST_ERROR("parse expect value", "");
    TEST_ERROR("parse expect value", " ");
}

static void test_parse_invalid_value()
{
    TEST_ERROR("parse invalid value", "nul");
    TEST_ERROR("parse invalid value", "!?");

    TEST_ERROR("parse invalid value", "+0");
    TEST_ERROR("parse invalid value", "+1");
    TEST_ERROR("parse invalid value", ".123"); /* at least one digit before '.' */
    TEST_ERROR("parse invalid value", "1.");   /* at least one digit after '.' */
    TEST_ERROR("parse invalid value", "INF");
    TEST_ERROR("parse invalid value", "inf");
    TEST_ERROR("parse invalid value", "NAN");
    TEST_ERROR("parse invalid value", "nan");

#if 1
    TEST_ERROR("parse invalid value", "[1,]");
    TEST_ERROR("parse invalid value", "[\"a\", nul]");
#endif
}

static void test_parse_root_not_singular()
{
    TEST_ERROR("parse root not singular", "null x");
    TEST_ERROR("parse root not singular", "truead");
    TEST_ERROR("parse root not singular", "\"dsad\"d");

    TEST_ERROR("parse root not singular", "0123");
    TEST_ERROR("parse root not singular", "0x0");
    TEST_ERROR("parse root not singular", "0x123");
}

static void test_parse_number_too_big()
{
    TEST_ERROR("parse number too big", "1e309");
    TEST_ERROR("parse number too big", "-1e309");
}

static void test_parse_missing_quotation_mark()
{
    TEST_ERROR("parse miss quotation mark", "\"");
    TEST_ERROR("parse miss quotation mark", "\"abc");
}

static void test_parse_invalid_string_escape()
{
#if 1
    TEST_ERROR("parse invalid string escape", "\"\\v\"");
    TEST_ERROR("parse invalid string escape", "\"\\'\"");
    TEST_ERROR("parse invalid string escape", "\"\\0\"");
    TEST_ERROR("parse invalid string escape", "\"\\x12\"");
#endif
}

static void test_parse_invalid_string_char()
{
#if 1
    TEST_ERROR("parse invalid string char", "\"\x01\"");
    TEST_ERROR("parse invalid string char", "\"\x1F\"");
#endif
}

static void test_parse_invalid_unicode_hex()
{
    TEST_ERROR("parse invalid unicode hex", "\"\\u\"");
    TEST_ERROR("parse invalid unicode hex", "\"\\u0\"");
    TEST_ERROR("parse invalid unicode hex", "\"\\u01\"");
    TEST_ERROR("parse invalid unicode hex", "\"\\u012\"");
    TEST_ERROR("parse invalid unicode hex", "\"\\u/000\"");
    TEST_ERROR("parse invalid unicode hex", "\"\\uG000\"");
    TEST_ERROR("parse invalid unicode hex", "\"\\u0/00\"");
    TEST_ERROR("parse invalid unicode hex", "\"\\u0G00\"");
    TEST_ERROR("parse invalid unicode hex", "\"\\u0/00\"");
    TEST_ERROR("parse invalid unicode hex", "\"\\u00G0\"");
    TEST_ERROR("parse invalid unicode hex", "\"\\u000/\"");
    TEST_ERROR("parse invalid unicode hex", "\"\\u000G\"");
    TEST_ERROR("parse invalid unicode hex", "\"\\u 123\"");
}

static void test_parse_invalid_unicode_surrogate()
{
    TEST_ERROR("parse invalid unicode surrogate", "\"\\uD800\"");
    TEST_ERROR("parse invalid unicode surrogate", "\"\\uDBFF\"");
    TEST_ERROR("parse invalid unicode surrogate", "\"\\uD800\\\\\"");
    TEST_ERROR("parse invalid unicode surrogate", "\"\\uD800\\uDBFF\"");
    TEST_ERROR("parse invalid unicode surrogate", "\"\\uD800\\uE000\"");
}

static void test_parse_miss_comma_or_square_bracket()
{
#if 1
    TEST_ERROR("parse miss comma or square bracket", "[1");
    TEST_ERROR("parse miss comma or square bracket", "[1}");
    TEST_ERROR("parse miss comma or square bracket", "[1 2");
    TEST_ERROR("parse miss comma or square bracket", "[[]");
#endif
}

static void test_parse_miss_key()
{
    TEST_ERROR("parse miss key", "{:1,");
    TEST_ERROR("parse miss key", "{1:1,");
    TEST_ERROR("parse miss key", "{true:1,");
    TEST_ERROR("parse miss key", "{false:1,");
    TEST_ERROR("parse miss key", "{null:1,");
    TEST_ERROR("parse miss key", "{[]:1,");
    TEST_ERROR("parse miss key", "{{}:1,");
    TEST_ERROR("parse miss key", "{\"a\":1,");
}

static void test_parse_miss_colon()
{
    TEST_ERROR("parse miss colon", "{\"a\"}");
    TEST_ERROR("parse miss colon", "{\"a\",\"b\"}");
}

static void test_parse_miss_comma_or_curly_bracket()
{
    TEST_ERROR("parse miss comma or curly bracket", "{\"a\":1");
    TEST_ERROR("parse miss comma or curly bracket", "{\"a\":1]");
    TEST_ERROR("parse miss comma or curly bracket", "{\"a\":1 \"b\"");
    TEST_ERROR("parse miss comma or curly bracket", "{\"a\":{}");
}

static void test_parse()
{
    test_parse_literal();
    test_parse_number();
    test_parse_string();
    test_parse_array();
    test_parse_object();

    test_parse_expect_value();
    test_parse_invalid_value();
    test_parse_root_not_singular();
    test_parse_number_too_big();
    test_parse_missing_quotation_mark();
    test_parse_invalid_string_escape();
    test_parse_invalid_string_char();
    test_parse_invalid_unicode_hex();
    test_parse_invalid_unicode_surrogate();
    test_parse_miss_comma_or_square_bracket();
    test_parse_miss_key();
    test_parse_miss_colon();
    test_parse_miss_comma_or_curly_bracket();
}

#define TEST_ROUNDTRIP(content)             \
    do                                      \
    {                                       \
        SJson::Json v;                      \
        v.Parse(content, status);           \
        EXPECT_EQ_BASE("parse ok", status); \
        v.Stringify(status);                \
        EXPECT_EQ_BASE(content, status);    \
    } while (0)

static void test_stringify_number()
{
    TEST_ROUNDTRIP("0");
    TEST_ROUNDTRIP("-0");
    TEST_ROUNDTRIP("1");
    TEST_ROUNDTRIP("-1");
    TEST_ROUNDTRIP("1.5");
    TEST_ROUNDTRIP("-1.5");
    TEST_ROUNDTRIP("3.25");
    TEST_ROUNDTRIP("1e+20");
    TEST_ROUNDTRIP("1.234e+20");
    TEST_ROUNDTRIP("1.234e-20");

    TEST_ROUNDTRIP("1.0000000000000002");      /* the smallest number > 1 */
    TEST_ROUNDTRIP("4.9406564584124654e-324"); /* minimum denormal */
    TEST_ROUNDTRIP("-4.9406564584124654e-324");
    TEST_ROUNDTRIP("2.2250738585072009e-308"); /* Max subnormal double */
    TEST_ROUNDTRIP("-2.2250738585072009e-308");
    TEST_ROUNDTRIP("2.2250738585072014e-308"); /* Min normal positive double */
    TEST_ROUNDTRIP("-2.2250738585072014e-308");
    TEST_ROUNDTRIP("1.7976931348623157e+308"); /* Max double */
    TEST_ROUNDTRIP("-1.7976931348623157e+308");
}

static void test_stringify_string()
{
    TEST_ROUNDTRIP("\"\"");
    TEST_ROUNDTRIP("\"Hello\"");
    TEST_ROUNDTRIP("\"Hello\\nWorld\"");
    TEST_ROUNDTRIP("\"\\\" \\\\ / \\b \\f \\n \\r \\t\"");
    TEST_ROUNDTRIP("\"Hello\\u0000World\"");
}

static void test_stringify_array()
{
    TEST_ROUNDTRIP("[]");
    TEST_ROUNDTRIP("[null,false,true,123,\"abc\",[1,2,3]]");
}

static void test_stringify_object()
{
    TEST_ROUNDTRIP("{}");
    TEST_ROUNDTRIP("{\"n\":null,\"f\":false,\"t\":true,\"i\":123,\"s\":\"abc\",\"a\":[1,2,3],\"o\":{\"1\":1,\"2\":2,\"3\":3}}");
}

static void test_stringify()
{
    TEST_ROUNDTRIP("null");
    TEST_ROUNDTRIP("false");
    TEST_ROUNDTRIP("true");
    test_stringify_number();
    test_stringify_string();
    test_stringify_array();
    test_stringify_object();
}

#define TEST_EQUAL(json1, json2, equality)       \
    do                                           \
    {                                            \
        SJson::Json v1, v2;                      \
        v1.Parse(json1, status);                 \
        EXPECT_EQ_BASE("parse ok", status);      \
        v2.Parse(json2, status);                 \
        EXPECT_EQ_BASE("parse ok", status);      \
        EXPECT_EQ_BASE(equality, int(v1 == v2)); \
    } while (0)

static void test_equal()
{
    TEST_EQUAL("true", "true", 1);
    TEST_EQUAL("true", "false", 0);
    TEST_EQUAL("false", "false", 1);
    TEST_EQUAL("null", "null", 1);
    TEST_EQUAL("null", "0", 0);
    TEST_EQUAL("123", "123", 1);
    TEST_EQUAL("123", "456", 0);
    TEST_EQUAL("\"abc\"", "\"abc\"", 1);
    TEST_EQUAL("\"abc\"", "\"abcd\"", 0);
    TEST_EQUAL("[]", "[]", 1);
    TEST_EQUAL("[]", "null", 0);
    TEST_EQUAL("[1,2,3]", "[1,2,3]", 1);
    TEST_EQUAL("[1,2,3]", "[1,2,3,4]", 0);
    TEST_EQUAL("[[]]", "[[]]", 1);
    TEST_EQUAL("{}", "{}", 1);
    TEST_EQUAL("{}", "null", 0);
    TEST_EQUAL("{}", "[]", 0);
    TEST_EQUAL("{\"a\":1,\"b\":2}", "{\"a\":1,\"b\":2}", 1);
    TEST_EQUAL("{\"a\":1,\"b\":2}", "{\"b\":2,\"a\":1}", 1);
    TEST_EQUAL("{\"a\":1,\"b\":2}", "{\"a\":1,\"b\":3}", 0);
    TEST_EQUAL("{\"a\":1,\"b\":2}", "{\"a\":1,\"b\":2,\"c\":3}", 0);
    TEST_EQUAL("{\"a\":{\"b\":{\"c\":{}}}}", "{\"a\":{\"b\":{\"c\":{}}}}", 1);
    TEST_EQUAL("{\"a\":{\"b\":{\"c\":{}}}}", "{\"a\":{\"b\":{\"c\":[]}}}", 0);
}

static void test_copy()
{
    SJson::Json v1, v2;
    v1.Parse("{\"t\":true,\"f\":false,\"n\":null,\"d\":1.5,\"a\":[1,2,3]}");
    v2 = v1;
    EXPECT_EQ_BASE(1, int(v2 == v1));
}

static void test_move()
{
    SJson::Json v1, v2, v3;
    v1.Parse("{\"t\":true,\"f\":false,\"n\":null,\"d\":1.5,\"a\":[1,2,3]}");
    v2 = v1;
    v3 = std::move(v2);
    EXPECT_EQ_BASE(JsonType::Null, v2.GetType());
    EXPECT_EQ_BASE(1, int(v3 == v1));
}

static void test_swap()
{
    SJson::Json v1, v2;
    v1.SetString("Hello");
    v2.SetString("World!");
    SJson::swap(v1, v2);
    EXPECT_EQ_BASE("World!", v1.GetString());
    EXPECT_EQ_BASE("Hello", v2.GetString());
}

static void test_access_null()
{
    SJson::Json v;
    v.SetString("a");
    v.SetNull();
    EXPECT_EQ_BASE(JsonType::Null, v.GetType());
}

static void test_access_boolean()
{
    SJson::Json v;
    v.SetString("a");
    v.SetBoolean(false);
    EXPECT_EQ_BASE(JsonType::False, v.GetType());
}

static void test_access_number()
{
    SJson::Json v;
    v.SetString("a");
    v.SetNumber(1234.5);
    EXPECT_EQ_BASE(1234.5, v.GetNumber());
}

static void test_access_string()
{
    SJson::Json v;
    v.SetString("");
    EXPECT_EQ_STRING("", v.GetString());
    v.SetString("Hello");
    EXPECT_EQ_STRING("Hello", v.GetString());
}

static void test_access_array()
{
    SJson::Json a, e;

    for (size_t j = 0; j < 5; j += 5)
    {
        a.SetArray();
        EXPECT_EQ_BASE(0, a.GetArraySize());
        for (int i = 0; i < 10; ++i)
        {
            e.SetNumber(i);
            a.PushbackArrayElement(e);
        }

        EXPECT_EQ_BASE(10, a.GetArraySize());
        for (int i = 0; i < 10; ++i)
            EXPECT_EQ_BASE(static_cast<double>(i), a.GetArrayElement(i).GetNumber());
    }

    a.PopbackArrayElement();
    EXPECT_EQ_BASE(9, a.GetArraySize());
    for (int i = 0; i < 9; ++i)
        EXPECT_EQ_BASE(static_cast<double>(i), a.GetArrayElement(i).GetNumber());

    a.EraseArrayElement(4, 0);
    EXPECT_EQ_BASE(9, a.GetArraySize());
    for (int i = 0; i < 9; ++i)
        EXPECT_EQ_BASE(static_cast<double>(i), a.GetArrayElement(i).GetNumber());

    a.EraseArrayElement(8, 1);
    EXPECT_EQ_BASE(8, a.GetArraySize());
    for (int i = 0; i < 8; ++i)
        EXPECT_EQ_BASE(static_cast<double>(i), a.GetArrayElement(i).GetNumber());

    a.EraseArrayElement(0, 2);
    EXPECT_EQ_BASE(6, a.GetArraySize());
    for (int i = 0; i < 6; ++i)
        EXPECT_EQ_BASE(static_cast<double>(i) + 2, a.GetArrayElement(i).GetNumber());

    for (int i = 0; i < 2; ++i)
    {
        e.SetNumber(i);
        a.InsertArrayElement(e, i);
    }

    EXPECT_EQ_BASE(8, a.GetArraySize());
    for (int i = 0; i < 8; ++i)
    {
        EXPECT_EQ_BASE(static_cast<double>(i), a.GetArrayElement(i).GetNumber());
    }

    e.SetString("Hello");
    a.PushbackArrayElement(e);

    a.ClearArray();
    EXPECT_EQ_BASE(0, a.GetArraySize());
}

static void test_access_object()
{
    SJson::Json o, v;

    for (int j = 0; j <= 5; j += 5)
    {
        o.SetObject();
        EXPECT_EQ_BASE(0, o.GetObjectSize());
        for (int i = 0; i < 10; ++i)
        {
            std::string key = "a";
            key[0] += i;
            v.SetNumber(i);
            o.SetObjectValue(key, v);
        }
        EXPECT_EQ_BASE(10, o.GetObjectSize());
        for (int i = 0; i < 10; ++i)
        {
            std::string key = "a";
            key[0] += i;
            auto index = o.FindObjectIndex(key);
            EXPECT_EQ_BASE(1, static_cast<int>(index >= 0));
            v = o.GetObjectValue(index);
            EXPECT_EQ_BASE(static_cast<double>(i), v.GetNumber());
        }
    }

    auto index = o.FindObjectIndex("j");
    EXPECT_EQ_BASE(1, static_cast<int>(index >= 0));
    o.RemoveObjectValue(index);
    index = o.FindObjectIndex("j");
    EXPECT_EQ_BASE(1, static_cast<int>(index < 0));
    EXPECT_EQ_BASE(9, o.GetObjectSize());

    index = o.FindObjectIndex("a");
    EXPECT_EQ_BASE(1, static_cast<int>(index >= 0));
    o.RemoveObjectValue(index);
    index = o.FindObjectIndex("a");
    EXPECT_EQ_BASE(1, static_cast<int>(index < 0));
    EXPECT_EQ_BASE(8, o.GetObjectSize());

    for (int i = 0; i < 8; i++)
    {
        std::string key = "a";
        key[0] += i + 1;
        EXPECT_EQ_BASE((double)i + 1, o.GetObjectValue(o.FindObjectIndex(key)).GetNumber());
    }

    v.SetString("Hello");
    o.SetObjectValue("World", v);

    index = o.FindObjectIndex("World");
    EXPECT_EQ_BASE(1, static_cast<int>(index >= 0));
    v = o.GetObjectValue(index);
    EXPECT_EQ_STRING("Hello", v.GetString());

    o.ClearObject();
    EXPECT_EQ_BASE(0, o.GetObjectSize());
}

static void test_access()
{
    test_access_null();
    test_access_boolean();
    test_access_number();
    test_access_string();
    test_access_array();
    test_access_object();
}

int main()
{

    test_parse();
    test_stringify();
    test_equal();
    test_copy();
    test_move();
    test_swap();
    test_access();
    printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
    return main_ret;
}