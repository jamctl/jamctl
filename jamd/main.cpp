#include <fstream>
#include <string>
#include <spdlog/spdlog.h>
#include <drogon/drogon.h>

#include "GrpcServer.h"

int main(int argc, char* argv[], char* envp[])
{
    spdlog::info(std::format("\n{:=^{}}", "Welcome to Jamd!", 40));
    try
    {
        spdlog::set_level(spdlog::level::debug); // 设置日志级别为 debug
        spdlog::flush_on(spdlog::level::info); // 在 info 级别时自动刷新日志
    }
    catch (const spdlog::spdlog_ex& ex)
    {
        std::cerr << "Log initialization failed: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
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
