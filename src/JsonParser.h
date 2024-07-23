#ifndef JSONPARSER_H
#define JSONPARSER_H
#include "JsonValue.h"
#include "Json.h"

namespace SJson
{
    class JsonParser
    {
    public:
        JsonParser(JsonValue &val, const std::string &content);

    private:
        /* 处理空白 */
        void ParseWhitespace() noexcept;
        /* 解析 json 值 */
        void ParseValue();
        /* 合并 false、true、null 的解析函数 */
        void ParseLiteral(const char *literal, JsonType::type t);
        /* 解析数字 */
        void ParseNumber();
        /* 解析字符串的函数拆分为两部分，是为了在解析 json 对象的 key 值时，不使用 lept_value 存储键，因为这样会浪费其中的 type 这个无用字段 */
        void ParseString();
        /* 解析 字符串 */
        void ParseStringRaw(std::string &tmp);
        /* 解析Hex */
        void ParseHex4(const char *&p, unsigned &u);
        /* 解析utf-8 */
        void ParseUTF8(std::string &str, unsigned u);
        JsonValue &m_val;
        const char *m_cur;
    };
}
#endif // JSONPARSE_H