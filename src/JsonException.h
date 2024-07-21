#ifndef JSONEXCEPTION_H
#define JSONEXCEPTION_H
#include <string>
#include <stdexcept>
namespace SJson
{
    class JsonException : public std::logic_error
    {
    public:
        JsonException(const std::string &errMsg) : logic_error(errMsg) {}
    };
}

#endif // JSONEXCEPTION_H