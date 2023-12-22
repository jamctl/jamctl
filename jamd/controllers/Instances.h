#pragma once

#include <drogon/HttpController.h>
#include <drogon/HttpAppFramework.h>
#include <chrono>
#include "../modules/manager/InstanceManager/InstanceManager.h"

using namespace drogon;

namespace jamd::controllers {
    class Instances final : public drogon::HttpController<Instances> {
    public:
        METHOD_LIST_BEGIN
            METHOD_ADD(Instances::StartInstance, "/start", Get);
        METHOD_LIST_END

        // 使用协程来处理请求
        void StartInstance(const HttpRequestPtr &req, std::function<void (const HttpResponsePtr &)> &&callback);
    };
}
