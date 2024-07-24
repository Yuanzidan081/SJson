#include "Json.h"
#include "JsonValue.h"
#include "JsonException.h"
namespace SJson
{
    Json::Json() noexcept : m_Value(new JsonValue) {}
    Json::~Json() noexcept {}
    Json::Json(const Json &rhs) noexcept
    {
        m_Value.reset(new JsonValue(*(rhs.m_Value)));
    }
    Json &Json::operator=(const Json &rhs) noexcept
    {
        if (this == &rhs)
            return *this;
        m_Value.reset(new JsonValue(*(rhs.m_Value)));
        return *this;
    }
    Json::Json(Json &&rhs) noexcept
    {
        m_Value.reset(rhs.m_Value.release());
    }

    Json &Json::operator=(Json &&rhs) noexcept
    {
        // rhs.m_Value.release() 返回它管理的指针，转移所有权，所以不对这个指针delete，并且rhs的m_Value定义为nullptr
        // 当前类的m_Value原来管理的指针释放掉（使用delete1），而改为指向rhs.m_Value.release的指针

        m_Value.reset(rhs.m_Value.release());
        return *this;
    }
    void Json::swap(Json &rhs) noexcept
    {
        using std::swap;
        swap(m_Value, rhs.m_Value);
    }

    void Json::Parse(const std::string &content, std::string &status) noexcept
    {
        try
        {
            Parse(content);
            status = "parse ok";
        }
        catch (const JsonException &msg)
        {
            status = msg.what();
        }
        catch (...)
        {
        }
    }

    void Json::Parse(const std::string &content)
    {
        m_Value->Parse(content);
    }

    bool operator==(const Json &lhs, const Json &rhs) noexcept
    {
        return *(lhs.m_Value) == *(rhs.m_Value);
    }

    bool operator!=(const Json &lhs, const Json &rhs) noexcept
    {
        return *(lhs.m_Value) != *(rhs.m_Value);
    }

    void swap(Json &lhs, Json &rhs) noexcept
    {
        lhs.swap(rhs);
    }

    int Json::GetType() const noexcept
    {
        if (m_Value == nullptr)
            return JsonType::Null;
        return m_Value->GetType();
    }
    void Json::SetNull() noexcept
    {
        m_Value->SetType(JsonType::Null);
    }
    void Json::SetBoolean(bool b) noexcept
    {
        if (b)
            m_Value->SetType(JsonType::True);
        else
            m_Value->SetType(JsonType::False);
    }
    double Json::GetNumber() const noexcept
    {
        return m_Value->GetNumber();
    }
    void Json::SetNumber(double d) noexcept
    {
        m_Value->SetNumber(d);
    }
    const std::string Json::GetString() const noexcept
    {
        return m_Value->GetString();
    }
    void Json::SetString(const std::string &str) noexcept
    {
        m_Value->SetString(str);
    }
    size_t Json::GetArraySize() const noexcept
    {
        return m_Value->GetArraySize();
    }
    Json Json::GetArrayElement(size_t index) const noexcept
    {
        Json ret;
        ret.m_Value.reset(new JsonValue(m_Value->GetArrayElement(index)));
        return ret;
    }
    void Json::SetArray() noexcept
    {
        m_Value->SetArray(std::vector<JsonValue>{});
    }
    void Json::PushbackArrayElement(const Json &val) noexcept
    {
        m_Value->PushbackArrayElement(*val.m_Value);
    }
    void Json::PopbackArrayElement() noexcept
    {
        m_Value->PopbackArrayElement();
    }
    void Json::EraseArrayElement(size_t index, size_t count) noexcept
    {
        m_Value->EraseArrayElement(index, count);
    }
    void Json::InsertArrayElement(const Json &val, size_t index) noexcept
    {
        m_Value->InsertArrayElement(*val.m_Value, index);
    }
    void Json::ClearArray() noexcept
    {
        m_Value->ClearArray();
    }
    void Json::SetObject() noexcept
    {
        m_Value->SetObject(std::vector<std::pair<std::string, JsonValue>>{});
    }
    size_t Json::GetObjectSize() const noexcept
    {
        return m_Value->GetObjectSize();
    }
    const std::string &Json::GetObjectKey(size_t index) const noexcept
    {
        return m_Value->GetObjectKey(index);
    }
    Json Json::GetObjectValue(size_t index) const noexcept
    {
        Json ret;
        ret.m_Value.reset(new JsonValue(m_Value->GetObjectValue(index)));
        return ret;
    }
    size_t Json::GetObjectKeyLength(size_t index) const noexcept
    {
        return m_Value->GetObjectKeyLength(index);
    }
    void Json::SetObjectValue(const std::string &key, const Json &val) noexcept
    {
        m_Value->SetObjectValue(key, *val.m_Value);
    }
    long long Json::FindObjectIndex(const std::string &key) const noexcept
    {
        return m_Value->FindObjectIndex(key);
    }
    void Json::RemoveObjectValue(size_t index) noexcept
    {
        m_Value->RemoveObjectValue(index);
    }
    void Json::ClearObject() noexcept
    {
        m_Value->ClearObject();
    }
    void Json::Stringify(std::string &content) const noexcept
    {
        m_Value->Stringify(content);
    }
}
