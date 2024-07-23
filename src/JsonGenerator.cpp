#include "JsonGenerator.h"
#include <cassert>
namespace SJson
{
    JsonGenerator::JsonGenerator(const JsonValue &val, std::string &result) : m_res(result)
    {
        m_res.clear();
        StringifyValue(val);
    }

    /* 生成json的值 */
    void JsonGenerator::StringifyValue(const JsonValue &val)
    {
        switch (val.GetType())
        {
        case JsonType::Null:
            m_res += "null";
            break;
        case JsonType::True:
            m_res += "true";
            break;
        case JsonType::False:
            m_res += "false";
            break;
        case JsonType::Number:
        {
            char buffer[32] = {0};
            sprintf(buffer, "%.17g", val.GetNumber());
            m_res += buffer;
        }
        break;
        case JsonType::String:
            StringifyString(val.GetString()); // 生成字符串
            break;
        // 生成数组：只要输出"[]"，中间对逐个子值递归调用 stringify_value()
        case JsonType::Array:
            m_res += '[';
            for (size_t i = 0; i < val.GetArraySize(); i++)
            {
                if (i > 0)
                    m_res += ',';
                StringifyValue(val.GetArrayElement(i));
            }
            m_res += ']';
            break;
        // 生成对象
        case JsonType::Object:
            m_res += '{';
            for (int i = 0; i < val.GetObjectSize(); ++i)
            {
                if (i > 0)
                    m_res += ',';
                // 对象需要多处理一个 key 和冒号
                StringifyString(val.GetObjectKey(i));
                m_res += ':';
                // 递归调用生成 json 值
                StringifyValue(val.GetObjectValue(i));
            }
            m_res += '}';
            break;
        default:
            assert(0 && "invalid type");
        }
    }
    void JsonGenerator::StringifyString(const std::string &str)
    {
        m_res += '\"';
        for (auto it = str.begin(); it != str.end(); it++)
        {
            unsigned char ch = *it;
            switch (ch)
            {
            /* 添加这些转义字符 */
            case '\"':
                m_res += "\\\"";
                break;
            case '\\':
                m_res += "\\\\";
                break;
            case '\b':
                m_res += "\\b";
                break;
            case '\f':
                m_res += "\\f";
                break;
            case '\n':
                m_res += "\\n";
                break;
            case '\r':
                m_res += "\\r";
                break;
            case '\t':
                m_res += "\\t";
                break;
            default:
                // 低于 0x20 的字符需要转义为 \u00xx 的形式
                if (ch < 0x20)
                {
                    char buffer[7] = {0};
                    sprintf(buffer, "\\u%04X", ch);
                    m_res += buffer;
                }
                else
                    m_res += *it;
            }
        }
        m_res += '\"'; // 添加最后一个双引号
    }
}