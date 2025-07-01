#include "Instances.h"
#include "../models/Instances.h"

#include <drogon/orm/Mapper.h>

#include "../ext/styledORM.h"
#define eq ,CompareOperator::EQ,
#define var auto
#define val const auto

void Instances::StartInstance(const Request& req,
                              RouteCallback&& callback,
                              const String& target)
{
    var& manager = InstanceManager::Instance();
    val ptr = app().getDbClient("jamd");
    Mapper<drogon_model::jamd::Instances> mp(ptr);
    if (val rn = mp.findBy(where{drogon_model::jamd::Instances::Cols::_name eq target}); rn.size()
        != 1)
    {
        if (val ri = mp.findBy(where{drogon_model::jamd::Instances::Cols::_id eq std::stoi(target)}); ri.size()
            != 1)
        {
            callback(LaunchResult{
                .success = false, .code = 404, .message = "Error"
            }.toResponse());
            return;
        }
        // manager.launch(*rn.getId());
        callback(LaunchResult{.success = true, .code = 200, .message = "Succeeded."}.toResponse());
        return;
    }
    // manager.launch(*rn.getId());
    callback(LaunchResult{.success = true, .code = 200, .message = "Succeeded."}.toResponse());
}
