#include "Instances.h"
#include "../models/Instances.h"

#include <drogon/orm/Mapper.h>

#include "../ext/styledORM.h"

void Instances::StartInstance(const Request& req,
                              RouteCallback&& callback,
                              const String& target)
{
    val resp = HttpResponse::newHttpJsonResponse(
        LaunchResult{.success = true, .code = 200, .message = "Succeeded."}.toJson());
    var& manager = InstanceManager::Instance();
    val ptr = app().getDbClient("jamd");
    Mapper<drogon_model::jamd::Instances> mp(ptr);
    if (val id = mp.findBy(where{drogon_model::jamd::Instances::Cols::_name eq target}); id.size() == 1)
    {
        val r = HttpResponse::newHttpResponse();
        r->setBody(std::to_string(*id[0].getId().get()));
        callback(r);
    }
    callback(resp);
}
