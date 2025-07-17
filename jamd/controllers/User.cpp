#include "User.h"

#include "Instances.h"

void User::UserRegister(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback)
{
    callback(LaunchResult{.success = true, .code = 200, .message = "Succeeded."}.toResponse());
}
