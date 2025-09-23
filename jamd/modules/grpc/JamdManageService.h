#pragma once
#include "system.grpc.pb.h"
#include "../../modules/manager/ConfigFileManager/ConfigFileManager.h"

class JamdManageService final : public System::Service {
    public:
        grpc::Status ReloadConfig(
            grpc::ServerContext* context,
            const ReloadConfigRequest* request,
            ReloadConfigResult* reply
        ) override;
};