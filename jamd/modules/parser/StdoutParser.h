#pragma once
#include <spdlog/spdlog.h>

#include "interfaces/ILogParser.h"
#include "../../ext/marcos/ktstyle.h"
#include "../../ext/extFuns.h"

class StdoutParser final : public LogParserInterface {
    public:
        ~StdoutParser() override = default;

        void parse(const String& log) override
        {
            val res = split(log, ':');
            if (val exp = expand(res, "", 3); exp != "")
                spdlog::info("[Instance] {}", exp.substr(1, exp.size() - 1));
        }

        void parse_err(const String& err) override
        {
            val res = split(err, ':');
            if (val exp = expand(res, "", 3); exp != "")
                spdlog::info("[Instance] {}", exp.substr(1, exp.size() - 1));
        }
};