#include <assert.h>
#include <string>
#include "JsonValue.h"
#include "JsonParser.h"
#include "JsonGenerator.h"
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

    double JsonValue::GetNumber() const noexcept
    {
        assert(m_type == JsonType::Number);
        return m_num;
    }

    void JsonValue::SetNumber(double d) noexcept
    {
        Free();
        m_type = JsonType::Number;
        m_num = d;
    }

    const std::string &JsonValue::GetString() const noexcept
    {
        assert(m_type == JsonType::String);
        return m_string;
    }

    void JsonValue::SetString(const std::string &str) noexcept
    {
        if (m_type == JsonType::String)
            m_string = str;
        else
        {
            // 释放内存，然后重新设置字符串
            Free();
            m_type = JsonType::String;
            new (&m_string) std::string(str);
        }
    }

    size_t JsonValue::GetArraySize() const noexcept
    {
        assert(m_type == JsonType::Array);
        return m_array.size();
    }

    const JsonValue &JsonValue::GetArrayElement(size_t index) const noexcept
    {
        assert(m_type == JsonType::Array);
        assert(index >= 0 && index < m_array.size());
        return m_array[index];
    }

    void JsonValue::SetArray(const std::vector<JsonValue> &arr) noexcept
    {
        if (m_type == JsonType::Array)
            m_array = arr;
        else
        {
            Free();
            m_type = JsonType::Array;
            new (&m_array) std::vector<JsonValue>(arr);
        }
    }

    void JsonValue::PushbackArrayElement(const JsonValue &val) noexcept
    {
        assert(m_type == JsonType::Array);
        m_array.push_back(val);
    }

    void JsonValue::PopbackArrayElement() noexcept
    {
        assert(m_type == JsonType::Array);
        m_array.pop_back();
    }

    void JsonValue::EraseArrayElement(size_t index, size_t count) noexcept
    {
        assert(m_type == JsonType::Array);
        m_array.erase(m_array.begin() + index, m_array.begin() + index + count);
    }

    void JsonValue::InsertArrayElement(const JsonValue &val, size_t index) noexcept
    {
        assert(m_type == JsonType::Array);
        m_array.insert(m_array.begin() + index, val);
    }

    void JsonValue::ClearArray() noexcept
    {
        assert(m_type == JsonType::Array);
        m_array.clear();
    }

    void JsonValue::SetObject(const std::vector<std::pair<std::string, JsonValue>> &obj) noexcept
    {
        if (m_type == JsonType::Object)
            m_object = obj;
        else
        {
            Free();
            m_type = JsonType::Object;
            new (&m_object) std::vector<std::pair<std::string, JsonValue>>(obj);
        }
    }

    size_t JsonValue::GetObjectSize() const noexcept
    {
        assert(m_type == JsonType::Object);
        return m_object.size();
    }

    const std::string &JsonValue::GetObjectKey(size_t index) const noexcept
    {
        assert(m_type == JsonType::Object);
        assert(index >= 0 && index < m_object.size());
        return m_object[index].first;
    }

    const JsonValue &JsonValue::GetObjectValue(size_t index) const noexcept
    {
        assert(m_type == JsonType::Object);
        assert(index >= 0 && index < m_object.size());
        return m_object[index].second;
    }

    size_t JsonValue::GetObjectKeyLength(size_t index) const noexcept
    {
        assert(m_type == JsonType::Object);
        return m_object[index].first.size();
    }

    long long JsonValue::FindObjectIndex(const std::string &key) const noexcept
    {
        assert(m_type == JsonType::Object);
        for (size_t i = 0, n = m_object.size(); i < n; ++i)
        {
            if (m_object[i].first == key)
                return i;
        }
        return -1;
    }

    void JsonValue::SetObjectValue(const std::string &key, const JsonValue &val) noexcept
    {
        assert(m_type == JsonType::Object);
        auto index = FindObjectIndex(key);
        if (index >= 0)
            m_object[index].second = val;
        else
            m_object.push_back(std::make_pair(key, val));
    }

    void JsonValue::RemoveObjectValue(size_t index) noexcept
    {
        assert(m_type == JsonType::Object);
        m_object.erase(m_object.begin() + index, m_object.begin() + index + 1);
    }

    void JsonValue::ClearObject() noexcept
    {
        assert(m_type == JsonType::Object);
        m_object.clear();
    }

    void JsonValue::Stringify(std::string &content) const noexcept
    {
        JsonGenerator(*this, content);
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
        if (lhs.m_type != rhs.m_type)
            return false;
        // 对于 true、false、null 这三种类型，比较类型后便完成比较。而对于数组、对象、数字、字符串，需要进一步检查是否相等
        switch (lhs.m_type)
        {
        case JsonType::Number:
            return lhs.m_num == rhs.m_num;
        case JsonType::String:
            return lhs.m_string == rhs.m_string;
        case JsonType::Array:
            return lhs.m_array == rhs.m_array;
        case JsonType::Object:
            // 对于对象，先比较键值对的个数是否相等
            if (lhs.GetObjectSize() != rhs.GetObjectSize())
                return false;
            // 相等的话，对左边的键值对，依次在右边进行寻找
            for (size_t i = 0, n = lhs.GetObjectSize(); i < n; i++)
            {
                // 根据左边对象的键值对，在右边对象中寻找相对应的索引
                auto index = rhs.FindObjectIndex(lhs.GetObjectKey(i));
                // key 不存在或者左右对象的 value 值不相等，直接返回 0
                if (index < 0 || lhs.GetObjectValue(i) != rhs.GetObjectValue(index))
                    return false;
            }
            return true;
        default:
            return true;
        }
    }
    bool operator!=(const JsonValue &lhs, const JsonValue &rhs) noexcept
    {
        return !(lhs == rhs);
    }
}
