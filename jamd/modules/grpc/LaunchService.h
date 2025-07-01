#pragma once
#include "launch.grpc.pb.h"

class LaunchService final : public LaunchInstance::Service
{
public:
    grpc::Status Launch(grpc::ServerContext* context,
                        const StartInstanceRequest* request,
                        LaunchResult* reply) override;
};
