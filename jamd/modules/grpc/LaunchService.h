#pragma once
#include "launch.grpc.pb.h"

#include <drogon/orm/Mapper.h>

#include "../../modules/manager/InstanceManager/InstanceManager.h"
#include "../../ext/marcos/drogon.h"
#include "../../ext/marcos/orm.h"
#include "../../models/Instances.h"

class LaunchService final : public LaunchInstance::Service {
    public:
        grpc::Status Launch(
            grpc::ServerContext* context,
            const StartInstanceRequest* request,
            LaunchResult* reply
        ) override;
};