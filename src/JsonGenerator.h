#ifndef JSONGENERATOR_H
#define JSONGENERATOR_H
#include "JsonValue.h"
namespace SJson
{
    class JsonGenerator
    {
    public:
        JsonGenerator(const JsonValue &val, std::string &result);

    private:
        void StringifyValue(const JsonValue &val);
        void StringifyString(const std::string &str);
        std::string &m_res;
    };
}
#endif // JSONGENERATOR_H