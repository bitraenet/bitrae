#pragma once

#include <mw/exceptions/BTREException.h>
#include <mw/util/StringUtil.h>

#define ThrowNotFound(msg) throw NotFoundException(msg, __FUNCTION__)
#define ThrowNotFound_F(msg, ...) throw NotFoundException(StringUtil::Format(msg, __VA_ARGS__), __FUNCTION__)

class NotFoundException : public BTREException
{
public:
    NotFoundException(const std::string& message, const std::string& function)
        : BTREException("NotFoundException", message, function)
    {

    }
};