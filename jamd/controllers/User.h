#pragma once

#include <drogon/HttpController.h>
#include "../ext/alias.h"
#include "../ext/marcos/drogon.h"

using namespace drogon;

class User final : public HttpController<User> {
public:
    METHOD_LIST_BEGIN
        POST("/register", UserRegister)
    METHOD_LIST_END

    static void UserRegister(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr&)>&& callback);
};
