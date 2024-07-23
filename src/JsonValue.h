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

        /* null true false */
        int GetType() const noexcept;
        void SetType(JsonType::type t);
        void Parse(const std::string &content);

        /* number */
        double GetNumber() const noexcept;
        void SetNumber(double d) noexcept;

        /* string */
        const std::string &GetString() const noexcept;
        void SetString(const std::string &str) noexcept;

        /* array */
        size_t GetArraySize() const noexcept;
        const JsonValue &GetArrayElement(size_t index) const noexcept;
        void SetArray(const std::vector<JsonValue> &arr) noexcept;

        /* object */
        void SetObject(const std::vector<std::pair<std::string, JsonValue>> &obj) noexcept;
        size_t GetObjectSize() const noexcept;
        const std::string &GetObjectKey(size_t index) const noexcept;
        const JsonValue &GetObjectValue(size_t index) const noexcept;
        size_t GetObjectKeyLength(size_t index) const noexcept;

    private:
        /* 初始化 JsonValue 与释放 JsonValue 的内存 */
        void
        Init(const JsonValue &rhs) noexcept;
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