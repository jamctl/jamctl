#include "LaunchService.h"

grpc::Status LaunchService::Launch(grpc::ServerContext* context, const StartInstanceRequest* request,
                                   LaunchResult* reply)
{
    val target = request->name();
    var& manager = jamd::managers::InstanceManager::Instance();
    val ptr = drogon::app().getDbClient("jamd");
    drogon::orm::Mapper<drogon_model::jamd::Instances> mp(ptr);
    if (val rn = mp.findBy(where{drogon_model::jamd::Instances::Cols::_name eq target}); rn.size() == 1)
    {
        manager.launch(*rn[0].getName());
        reply->set_success(true);
    }
    else
        reply->set_success(false);
    if (val ri = mp.findBy(where{drogon_model::jamd::Instances::Cols::_name eq target}); ri.size() == 1)
    {
        manager.launch(*ri[0].getName());
        reply->set_success(true);
    }
    else
        reply->set_success(false);

    return grpc::Status::OK;
}
