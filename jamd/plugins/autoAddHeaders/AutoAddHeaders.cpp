#include "AutoAddHeaders.h"

using namespace jamd::plugins;

void AutoAddHeaders::invoke(const Request& req, drogon::MiddlewareNextCallback&& nextCb,
                            drogon::MiddlewareCallback&& mcb)
{
    nextCb([mcb = std::move(mcb)](const Response& resp)
    {
        resp->addHeader("Server", "jamd/0.1.0");
        mcb(resp);
    });
}
