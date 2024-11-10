#pragma once

#include <drogon/HttpController.h>
#include <drogon/HttpAppFramework.h>

#include "../modules/manager/InstanceManager/InstanceManager.h"

using namespace drogon;
using InstanceManager = jamd::managers::InstanceManager;

class Instances final : public HttpController<Instances> {
public:
    static void initPathRouting();

    InstanceManager &manager = InstanceManager::Instance();

    // 使用协程来处理请求
    void StartInstance(const HttpRequestPtr &req, std::function<void (const HttpResponsePtr &)> &&callback);
};
