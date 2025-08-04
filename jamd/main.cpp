#include <fstream>
#include <string>
#include <spdlog/spdlog.h>
#include <drogon/drogon.h>

#include "GrpcServer.h"
#include "modules/manager/ConfigFileManager/ConfigFileManager.h"
#include "modules/manager/InstanceManager/InstanceManager.h"

int main(int argc, char* argv[], char* envp[])
{
    std::locale::global(std::locale("zh_CN.UTF-8"));
    jamd::managers::ConfigFileManager::Instance();
    try
    {
        spdlog::set_level(
            spdlog::level::from_str(jamd::managers::ConfigFileManager::Instance().daemonConfig.log.level));
        spdlog::flush_on(spdlog::level::info); // 在 info 级别时自动刷新日志
        spdlog::info(std::format("\n{:=^{}}", "Welcome to Jamd!", 40));
    }
    catch (const spdlog::spdlog_ex& ex)
    {
        std::cerr << "Log initialization failed: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    jamd::managers::InstanceManager::Instance();
    drogon::app().loadConfigFile("drogon.yaml").registerBeginningAdvice([]
    {
        for (const auto listeners = drogon::app().getListeners(); auto listener : listeners)
            spdlog::info("HTTP Server listening on {}", listener.toIpPort());
    });
    GrpcServer::Instance().Start("127.0.0.1:9001");
    drogon::app().run();
    GrpcServer::Instance().Stop();
    return 0;
}
