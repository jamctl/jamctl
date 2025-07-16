#include "CheckFeatureEnabled.h"

#include "../../ext/marcos/ktstyle.h"
#include "../../ext/extFuns.h"

using namespace jamd::plugins;

void CheckFeatureEnabled::invoke(const Request& req,
                                 drogon::MiddlewareNextCallback&& nextCb,
                                 drogon::MiddlewareCallback&& mcb)
{
    nextCb([req, callback = std::move(mcb)](const drogon::HttpResponsePtr& resp)
    {
        val path = req->path();
        if (path == "/")
            callback(resp);
        try
        {
            if (!featureMap.at(path))
                callback(drogon::HttpResponse::newHttpResponse(drogon::k403Forbidden, drogon::ContentType::CT_NONE));
        }
        catch (std::out_of_range& _)
        {
            callback(resp);
        }
    });
}
