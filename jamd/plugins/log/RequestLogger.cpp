#include "RequestLogger.h"

using namespace jamd::plugins;

void RequestLogger::invoke(
    const Request& req,
    drogon::MiddlewareNextCallback&& nextCb,
    drogon::MiddlewareCallback&& mcb
)
{
    var client = req->getPeerAddr().toIpPort();
    nextCb([/*start, */ client, req, callback = std::move(mcb)](const drogon::HttpResponsePtr& resp)
    {
        val status = resp ? resp->statusCode() : drogon::k500InternalServerError;
        val method = req->methodString();
        var path = req->path();
        spdlog::info("{} {} from {} {}",
                     std::string(method),
                     path,
                     client,
                     statusCodeToString(status));

        callback(resp);
    });
}