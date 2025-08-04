#include "System.h"

#include "../structs/api.h"
#include "../modules/manager/ConfigFileManager/ConfigFileManager.h"

void System::ReloadConfig(const HttpRequestPtr& req, Function<void(const HttpResponsePtr&)>&& callback)
{
    try
    {
        jamd::managers::ConfigFileManager::Instance().daemonConfig = YAML::LoadFile("jamd.yaml").as<JamdConfig>();
        spdlog::info("Config Reloaded");
    }
    catch (YAML::BadFile)
    {
        spdlog::info("Config file broken, using current config");
    }
    callback(
        jamd::structs::api::instances::LaunchResult{
            .success = true, .code = 200, .message = "Succeeded."
        }.toResponse());
}
