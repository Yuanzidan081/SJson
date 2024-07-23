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
        case '\"':
            ParseString();
            return;
        case '[':
            ParseArray();
            return;
        /*case '{':
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
    void JsonParser::ParseString()
    {
        std::string s = "";
        // 用临时值 s 来保存解析出来的字符串，然后将 s 赋值为 Value
        ParseStringRaw(s);
        m_val.SetString(s);
    }
    void JsonParser::ParseStringRaw(std::string &tmp)
    {
        Expect(m_cur, '\"'); // 跳过字符串的第一个引号
        const char *p = m_cur;
        unsigned u = 0, u2 = 0;
        while (*p != '\"') // 直到解析到字符串结尾，也就是第二个引号
        {
            // 字符串的结尾不是双引号，说明该字符串缺少引号，抛出异常即可
            if (*p == '\0')
                throw(JsonException("parse miss quotation mark"));
            // 处理 9 种转义字符：当前字符是'\'，然后跳到下一个字符
            if (*p == '\\' && ++p)
            {
                switch (*p++)
                {
                case '\"':
                    tmp += '\"';
                    break;
                case '\\':
                    tmp += '\\';
                    break;
                case '/':
                    tmp += '/';
                    break;
                case 'b':
                    tmp += '\b';
                    break;
                case 'f':
                    tmp += '\f';
                    break;
                case 'n':
                    tmp += '\n';
                    break;
                case 'r':
                    tmp += '\r';
                    break;
                case 't':
                    tmp += '\t';
                    break;
                case 'u':
                    // 遇到\u转义时，调用parse_hex4()来解析4位十六进制数字
                    ParseHex4(p, u);
                    if (u >= 0xD800 && u <= 0xDBFF)
                    {
                        if (*p++ != '\\')
                            throw(JsonException("parse invalid unicode surrogate"));
                        if (*p++ != 'u')
                            throw(JsonException("parse invalid unicode surrogate"));
                        ParseHex4(p, u2);
                        if (u2 < 0xDC00 || u2 > 0xDFFF)
                            throw(JsonException("parse invalid unicode surrogate"));
                        u = (((u - 0xD800) << 10) | (u2 - 0xDC00)) + 0x10000;
                    }
                    // 把码点编码成 utf-8，写进缓冲区
                    ParseUTF8(tmp, u);
                    break;
                default:
                    throw(JsonException("parse invalid string escape"));
                }
            }
            else if ((unsigned char)*p < 0x20)
            {
                throw(JsonException("parse invalid string char"));
            }
            else
                tmp += *p++;
        }
        // 更新当前字符串的位置
        m_cur = ++p;
    }
    void JsonParser::ParseHex4(const char *&p, unsigned &u)
    {
        u = 0;
        for (size_t i = 0; i < 4; ++i)
        {
            char ch = *p++;
            u <<= 4;
            if (isdigit(ch))
                u |= ch - '0';
            else if (ch >= 'A' && ch <= 'F')
                u |= ch - ('A' - 10);
            else if (ch >= 'a' && ch <= 'f')
                u |= ch - ('a' - 10);
            else
                throw(JsonException("parse invalid unicode hex"));
        }
    }
    void JsonParser::ParseUTF8(std::string &str, unsigned u)
    {
        if (u <= 0x7F)
            str += static_cast<char>(u & 0xFF);
        else if (u <= 0x7FF)
        {
            str += static_cast<char>(0xC0 | ((u >> 6) & 0xFF));
            str += static_cast<char>(0x80 | (u & 0x3F));
        }
        else if (u <= 0xFFFF)
        {
            str += static_cast<char>(0xE0 | ((u >> 12) & 0xFF));
            str += static_cast<char>(0x80 | ((u >> 6) & 0x3F));
            str += static_cast<char>(0x80 | (u & 0x3F));
        }
        else
        {
            assert(u <= 0x10FFFF);
            str += static_cast<char>(0xF0 | ((u >> 18) & 0xFF));
            str += static_cast<char>(0x80 | ((u >> 12) & 0x3F));
            str += static_cast<char>(0x80 | ((u >> 6) & 0x3F));
            str += static_cast<char>(0x80 | (u & 0x3F));
        }
    }

    void JsonParser::ParseArray()
    {
        Expect(m_cur, '['); // 处理数字的左括号，然后将当前字符的位置右移一位
        ParseWhitespace();  // 第一个解析空白：在左括号之后解析空白
        std::vector<JsonValue> tmp;
        if (*m_cur == ']')
        { // 遇到数组的右括号，然后将当前字符位置右移一位，并将 Value 设置为数组 tmp
            ++m_cur;
            m_val.SetArray(tmp);
            return;
        }
        for (;;)
        {
            // 先解析 json 值，如解析出现异常，则将 Value 设置为 null，然后抛出异常
            try
            {
                ParseValue();
            }
            catch (JsonException)
            {
                m_val.SetType(JsonType::Null);
                throw;
            }
            // 将解析出来的值加入到 tmp 后面
            tmp.push_back(m_val);
            ParseWhitespace(); // 第二个解析空白：在逗号之后处理空白

            // 值之后若为逗号，将当前字符的位置右移一位，然后处理逗号之后的空白
            if (*m_cur == ',')
            {
                ++m_cur;
                ParseWhitespace(); // 第三个解析空白：在逗号之后处理空白
            }

            // 值之后若为右括号，则将当前字符的位置右移一位，然后将 val_ 设置为数组 tmp
            else if (*m_cur == ']')
            {
                ++m_cur;
                m_val.SetArray(tmp);
                return;
            }

            // 若遇到解析失败，则直接抛出异常
            else
            {
                m_val.SetType(JsonType::Null);
                throw(JsonException("parse miss comma or square bracket"));
            }
        }
    }
}