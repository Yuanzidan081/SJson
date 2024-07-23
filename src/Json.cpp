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
}
