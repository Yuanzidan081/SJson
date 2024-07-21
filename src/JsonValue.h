#ifndef JSONVALUE_H
#define JSONVALUE_H
#include "Json.h"
#include <vector>
#include <utility>
#include <string>
namespace SJson
{
    class JsonValue
    {
    public:
        /* 构造函数 */
        JsonValue() noexcept { m_num = 0; }
        JsonValue(const JsonValue &rhs) noexcept { Init(rhs); }
        JsonValue &operator=(const JsonValue &rhs) noexcept;
        ~JsonValue() noexcept;

        int GetType() const noexcept;
        void SetType(JsonType::type t);
        void Parse(const std::string &content);

    private:
        /* 初始化 JsonValue 与释放 JsonValue 的内存 */
        void Init(const JsonValue &rhs) noexcept;
        void Free() noexcept;
        JsonType::type m_type = JsonType::Null;

        union
        {
            double m_num;
            std::string m_string;
            std::vector<JsonValue> m_array;
            std::vector<std::pair<std::string, JsonValue>> m_object;
        };
        friend bool operator==(const JsonValue &lhs, const JsonValue &rhs) noexcept;
    };
    /* 比较两个 json 值 */
    bool operator==(const JsonValue &lhs, const JsonValue &rhs) noexcept;
    bool operator!=(const JsonValue &lhs, const JsonValue &rhs) noexcept;
}
#endif // JSONVALUE_H