#include "JsonValue.h"
#include "JsonParser.h"
namespace SJson
{
    JsonValue &JsonValue::operator=(const JsonValue &rhs) noexcept
    {
        Free();
        Init(rhs);
        return *this;
    }

    JsonValue::~JsonValue() noexcept
    {
        Free();
    }

    int JsonValue::GetType() const noexcept
    {
        return m_type;
    }

    void JsonValue::SetType(JsonType::type t)
    {
        // 先释放内存，然后再重置类型
        Free();
        m_type = t;
    }

    void JsonValue::Parse(const std::string &content)
    {
        JsonParser(*this, content);
    }

    void JsonValue::Init(const JsonValue &rhs) noexcept
    {
        m_type = rhs.m_type;
        m_num = 0;
        switch (m_type)
        {
        case JsonType::Number:
            m_num = rhs.m_num;
            break;
        case JsonType::String:
            new (&m_string) std::string(rhs.m_string);
            break;
        case JsonType::Array:
            new (&m_array) std::vector<JsonValue>(rhs.m_array);
            break;
        case JsonType::Object:
            new (&m_object) std::vector<std::pair<std::string, JsonValue>>(rhs.m_object);
            break;
        }
    }
    void JsonValue::Free() noexcept
    {
        using std::string;
        switch (m_type)
        {
        case JsonType::String:
            m_string.~string(); // 显式调用相应的析构函数
            break;
        case JsonType::Array:
            m_array.~vector<JsonValue>();
            break;
        case JsonType::Object:
            m_object.~vector<std::pair<std::string, JsonValue>>();
        }
    }
    bool operator==(const JsonValue &lhs, const JsonValue &rhs) noexcept
    {
        /* todo */
        return false;
    }
    bool operator!=(const JsonValue &lhs, const JsonValue &rhs) noexcept
    {
        return !(lhs == rhs);
    }
}
