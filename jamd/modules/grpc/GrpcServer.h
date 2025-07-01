#pragma once

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <thread>
#include <memory>
#include <spdlog/spdlog.h>

#include "LaunchService.h"
#include "../../ext/alias.h"
#include "../../utils/Singleton.h"

class GrpcServer final : public jamd::Singleton<GrpcServer>
{
public:
    // 启动 gRPC 服务器
    void Start(const String& address)
    {
        if (!server_)
        {
            grpc::reflection::InitProtoReflectionServerBuilderPlugin();
            grpc::ServerBuilder builder;
            builder.AddListeningPort(address, grpc::InsecureServerCredentials());
            builder.RegisterService(&service_);
            server_ = builder.BuildAndStart();
            if (server_)
            {
                server_thread_ = std::thread([this]
                {
                    server_->Wait(); // 等待 gRPC 服务器运行
                });
                spdlog::info(std::format("gRPC Server started on {}", address));
            }
            else
            {
                throw std::runtime_error("Failed to start gRPC server");
            }
        }
    }

    // 停止 gRPC 服务器
    void Stop()
    {
        if (server_)
        {
            server_->Shutdown();
            if (server_thread_.joinable())
            {
                server_thread_.join();
            }
            server_.reset();
            spdlog::info("gRPC Server stopped");
        }
    }

private:
    std::unique_ptr<grpc::Server> server_; // gRPC 服务器实例
    std::thread server_thread_; // gRPC 服务器运行线程
    LaunchService service_;
};
