#ifndef JSON_H
#define JSON_H
#include <memory>
#include <string>

namespace SJson
{
    namespace JsonType
    {
        enum type : int
        {
            Null,
            True,
            False,
            Number,
            String,
            Array,
            Object
        };
    }
    class JsonValue;
    class Json final
    {
    public:
        /* 构造函数 */
        Json() noexcept;
        ~Json() noexcept;
        Json(const Json &rhs) noexcept;
        Json &operator=(const Json &rhs) noexcept;
        Json(Json &&rhs) noexcept;
        Json &operator=(Json &&rhs) noexcept;
        void swap(Json &rhs) noexcept;

        /* 解析 json 字符串 */
        void Parse(const std::string &content, std::string &status) noexcept;
        void Parse(const std::string &content);

        /* null true false */
        int GetType() const noexcept;
        void SetNull() noexcept;
        void SetBoolean(bool b) noexcept;
        Json &operator=(bool b) noexcept
        {
            SetBoolean(b);
            return *this;
        }
        /* number */
        double GetNumber() const noexcept;
        void SetNumber(double d) noexcept;
        Json &operator=(double d) noexcept
        {
            SetNumber(d);
            return *this;
        }

    private:
        /* 使用桥接模式，Json暴露给用户，JsonValue来获取具体的值 */
        std::unique_ptr<JsonValue> m_Value;
        friend bool operator==(const Json &lhs, const Json &rhs) noexcept;
        friend bool operator!=(const Json &lhs, const Json &rhs) noexcept;
    };
    bool operator==(const Json &lhs, const Json &rhs) noexcept;
    bool operator!=(const Json &lhs, const Json &rhs) noexcept;
    void swap(Json &lhs, Json &rhs) noexcept;
}
#endif // JSON_H
