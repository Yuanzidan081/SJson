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
        JsonValue &m_val;
        const char *m_cur;
    };
}
#endif // JSONPARSE_H