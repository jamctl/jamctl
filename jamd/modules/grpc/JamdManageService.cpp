#include "JamdManageService.h"

#include "../../ext/marcos/ktstyle.h"

grpc::Status JamdManageService::ReloadConfig(grpc::ServerContext* context, const ReloadConfigRequest* request,
                                             ReloadConfigResult* reply)
{
    var config_name = request->name();
    if (config_name.empty())
        config_name = "jamd.yaml";
    try
    {
        jamd::managers::ConfigFileManager::Instance().daemonConfig = YAML::LoadFile(config_name).as<JamdConfig>();
        spdlog::info("Config Reloaded");
        reply->set_success(true);
    }
    catch (YAML::BadFile&)
    {
        spdlog::info("Config file broken, using current config");
        reply->set_success(false);
    }
    return grpc::Status::OK;
}
