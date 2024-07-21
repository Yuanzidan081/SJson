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

        /* case '\"':
            parse_string();
            return;
        case '[':
            parse_array();
            return;
        case '{':
            parse_object();
            return;*/
        case '\0':
            throw(JsonException("parse expect value"));
        default:
            ParseNumber();
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
    void JsonParser::ParseNumber()
    {
        const char *p = m_cur;
        // 处理负号
        if (*p == '-')
            p++;

        // 处理整数部分，分为两种合法情况：一种是单个 0，另一种是一个 1~9 再加上任意数量的 digit。
        if (*p == '0')
            p++;
        else
        {
            if (!isdigit(*p))
                throw(JsonException("parse invalid value"));
            while (isdigit(*++p))
                ;
        }

        // 处理小数部分：小数点后面第一个数不是数字，则抛出异常，然后再处理连续的数字
        if (*p == '.')
        {
            if (!isdigit(*++p))
                throw(JsonException("parse invalid value"));
            while (isdigit(*++p))
                ;
        }

        // 处理指数部分：需要处理指数的符号，符号之后的第一个字符不是数字，则抛出异常；然后再处理连续的数字
        if (*p == 'e' || *p == 'E')
        {
            ++p;
            if (*p == '+' || *p == '-')
                ++p;
            if (!isdigit(*p))
                throw(JsonException("parse invalid value"));
            while (isdigit(*++p))
                ;
        }

        errno = 0;
        // 将 json 的十进制数字转换为 double 型的二进制数字
        double v = strtod(m_cur, NULL);
        // 如果转换出来的数字过大，则抛出异常
        if (errno == ERANGE && (v == HUGE_VAL || v == -HUGE_VAL))
            throw(JsonException("parse number too big"));

        // 最后设置 Value 为数字，然后更新 m_cur 的位置
        m_val.SetNumber(v);
        m_cur = p;
    }
}