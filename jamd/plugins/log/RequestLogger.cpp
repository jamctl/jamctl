#include "RequestLogger.h"

using namespace jamd::plugins;

void RequestLogger::invoke(const Request& req,
                           drogon::MiddlewareNextCallback&& nextCb,
                           drogon::MiddlewareCallback&& mcb)
{
    // var start = std::chrono::steady_clock::now();
    var client = req->getPeerAddr().toIpPort();

    // Do something before calling the next middleware
    nextCb([/*start, */ client, req, callback = std::move(mcb)](const drogon::HttpResponsePtr& resp)
    {
        // 计算处理时间
        val end = std::chrono::steady_clock::now();
        // val duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        // 获取响应信息
        val status = resp ? resp->statusCode() : drogon::k500InternalServerError;
        val method = req->methodString();
        var path = req->path();

        // 格式化日志信息
        // spdlog::info("{} {} from {} {} takes {} ms",
        //              std::string(method),
        //              path,
        //              client,
        //              statusCodeToString(status),
        //              std::to_string(duration.count()));
        spdlog::info("{} {} from {} {}",
                     std::string(method),
                     path,
                     client,
                     statusCodeToString(status));

        // 继续处理回调链
        callback(resp);
    });
}
