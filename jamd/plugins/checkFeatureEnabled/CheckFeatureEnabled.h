#pragma once
#include <drogon/HttpMiddleware.h>
#include <spdlog/spdlog.h>

#include "../../ext/alias.h"
#include "../../structs/config.h"


static std::unordered_map<String, bool JamdFeaturesConfig::*> featureMap = {
    {"/user/register", &JamdFeaturesConfig::enable_register},
};

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
