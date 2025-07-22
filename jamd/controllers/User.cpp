#include "User.h"

#include <drogon/orm/Mapper.h>

#include "../ext/extFuns.h"
#include "../structs/api.h"
#include "../ext/marcos/ktstyle.h"
#include "../models/Users.h"


void User::UserRegister(const HttpRequestPtr& req, Function<void(const HttpResponsePtr&)>&& callback)
{
    using jamd::structs::api::user::RegisterInfo;
    val info = RegisterInfo::fromJson(*req->getJsonObject().get());
    val ptr = app().getDbClient("jamd");
    orm::Mapper<drogon_model::jamd::Users> mp(ptr);
    var target = Json::Value(info.toJson());
    target.append(JValue("nickname", info.name));
    // target.append(JValue("extra_emails", Vector<String>()));
    target.append(JValue("group", "user"));
    var new_user = drogon_model::jamd::Users(target);
    mp.insert(new_user);
    callback(jamd::structs::api::instances::LaunchResult{.success = true, .code = 200, .message = "Succeeded."}.toResponse());
}