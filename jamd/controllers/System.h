#pragma once

#include <drogon/HttpController.h>

#include "../ext/marcos/drogon.h"
#include "../ext/alias.h"

using namespace drogon;

class System final : public HttpController<System>
{
public:
    METHOD_LIST_BEGIN
        POST("/reload_config", ReloadConfig, "drogon::LocalHostFilter")
    METHOD_LIST_END

    static void ReloadConfig(const HttpRequestPtr& req, Function<void (const HttpResponsePtr&)>&& callback);
};
