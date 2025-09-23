#pragma once

#include <drogon/HttpMiddleware.h>
#include <spdlog/spdlog.h>

#include "../../ext/alias.h"
#include "../../ext/marcos/ktstyle.h"

namespace jamd::plugins
{
    class RequestLogger final : public drogon::HttpMiddleware<RequestLogger> {
        public:
            RequestLogger() = default;

            void invoke(
                const Request& req,
                drogon::MiddlewareNextCallback&& nextCb,
                drogon::MiddlewareCallback&& mcb
            ) override;
    };
}