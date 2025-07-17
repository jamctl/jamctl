#pragma once
#include <drogon/HttpMiddleware.h>
#include <spdlog/spdlog.h>

#include "../../ext/alias.h"
#include "../../structs/config.h"

namespace jamd::plugins
{
    class CheckFeatureEnabled final : public drogon::HttpMiddleware<CheckFeatureEnabled>
    {
    public:
        CheckFeatureEnabled() = default;

        void invoke(const Request& req,
                    drogon::MiddlewareNextCallback&& nextCb,
                    drogon::MiddlewareCallback&& mcb) override;
    };
}
