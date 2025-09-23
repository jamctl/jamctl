#pragma once

#include <drogon/HttpMiddleware.h>

#include "../../ext/alias.h"
#include "../../ext/marcos/drogon.h"

namespace jamd::plugins
{
    class AutoAddHeaders final : public drogon::HttpMiddleware<AutoAddHeaders> {
        public:
            AutoAddHeaders() = default;

            void invoke(
                const Request& req,
                drogon::MiddlewareNextCallback&& nextCb,
                drogon::MiddlewareCallback&& mcb
            ) override;
    };
}