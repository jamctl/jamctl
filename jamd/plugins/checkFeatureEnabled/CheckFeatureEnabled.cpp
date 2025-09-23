#include "CheckFeatureEnabled.h"

#include "../../ext/marcos/ktstyle.h"
#include "../../ext/extFuns.h"
#include "../../modules/manager/ConfigFileManager/ConfigFileManager.h"

using namespace jamd::plugins;

static std::unordered_map<String, bool JamdFeaturesConfig::*> featureMap = {
    { "/user/register", &JamdFeaturesConfig::enable_register },
};

void CheckFeatureEnabled::invoke(
    const Request& req,
    drogon::MiddlewareNextCallback&& nextCb,
    drogon::MiddlewareCallback&& mcb
)
{
    nextCb([req, callback = std::move(mcb)](const drogon::HttpResponsePtr& resp)
    {
        val path = req->path();
        if (path == "/") {
            callback(resp);
            return;
        }
        try {
            if (!(managers::ConfigFileManager::Instance().daemonConfig.features.*featureMap.at(path))) {
                const auto r = drogon::HttpResponse::newHttpResponse();
                r->setStatusCode(drogon::k418ImATeapot);
                r->setBody("Feature disabled");
                callback(r);
            } else {
                callback(resp);
            }
        } catch (std::out_of_range& _) {
            callback(resp);
        }
    });
}