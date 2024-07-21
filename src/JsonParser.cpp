#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include "JsonParser.h"
#include "JsonException.h"
namespace SJson
{
    inline void Expect(const char *&c, char ch)
    {
        assert(*c == ch);
        ++c;
    }
    JsonParser::JsonParser(JsonValue &val, const std::string &content)
        : m_val(val), m_cur(content.c_str())
    {
        m_val.SetType(JsonType::Null);
        // 去掉Value前面的空白，若 json 在一个值之后，空白之后还有其他字符的话，说明该 json 值是不合法的。
        ParseWhitespace();
        ParseValue();
        ParseWhitespace();
        if (*m_cur != '\0')
        {
            m_val.SetType(JsonType::Null);
            throw(JsonException("parse root not singular"));
        }
    }
    void JsonParser::ParseWhitespace() noexcept
    {
        /* 过滤掉 json 字符串中的空白，即空格符、制表符、换行符、回车符 */
        while (*m_cur == ' ' || *m_cur == '\t' || *m_cur == '\n' || *m_cur == '\r')
            ++m_cur;
    }
    void JsonParser::ParseValue()
    {
        switch (*m_cur)
        {
        case 'n':
            ParseLiteral("null", JsonType::Null);
            return;
        case 't':
            ParseLiteral("true", JsonType::True);
            return;
        case 'f':
            ParseLiteral("false", JsonType::False);
            return;
        }
    }
    void JsonParser::ParseLiteral(const char *literal, JsonType::type t)
    {
        Expect(m_cur, literal[0]);
        size_t i;
        for (i = 0; literal[i + 1]; i++)
        {                                   // 直到 literal[i+1] 为 '\0'，循环结束
            if (m_cur[i] != literal[i + 1]) // 解析失败，抛出异常
                throw(JsonException("parse invalid value"));
        }
        // 解析成功，将 m_cur 右移 i 位，然后设置 val_ 的类型为 t
        m_cur += i;
        m_val.SetType(t);
    }
}