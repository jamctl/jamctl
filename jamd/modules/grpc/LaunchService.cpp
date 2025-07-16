#include "LaunchService.h"
#include "../../modules/manager/InstanceManager/InstanceManager.h"
#include "../../ext/marcos/drogon.h"

grpc::Status LaunchService::Launch(grpc::ServerContext* context, const StartInstanceRequest* request,
                                   LaunchResult* reply)
{
    var& manager = jamd::managers::InstanceManager::Instance();

    return grpc::Status::OK;
}
