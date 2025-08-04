#include "User.h"

#include <drogon/orm/Mapper.h>

#include "../ext/extFuns.h"
#include "../structs/api.h"
#include "../ext/marcos/ktstyle.h"
#include "../models/Users.h"
#include "../modules/PasswordHasher.h"


void User::UserRegister(const HttpRequestPtr& req, Function<void(const HttpResponsePtr&)>&& callback)
{
    var info = *req->getJsonObject();
    val ptr = app().getDbClient("jamd");
    orm::Mapper<drogon_model::jamd::Users> mp(ptr);
    info["password"] = PasswordHasher::hashPassword(info["password"].asString());
    var new_user = drogon_model::jamd::Users(info);
    mp.insert(new_user);
    callback(
        jamd::structs::api::instances::LaunchResult{
            .success = true, .code = 200, .message = "Succeeded."
        }.toResponse());
}
