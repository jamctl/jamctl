#pragma once

#include "../../ext/alias.h"

class LogParserInterface
{
public:
    virtual ~LogParserInterface() = default;
    virtual void parse(const String& log) = 0;
    virtual void parse_err(const String& err) = 0;
};
