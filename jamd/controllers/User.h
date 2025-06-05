#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class User final : public HttpController<User> {
public:
    METHOD_LIST_BEGIN

    METHOD_LIST_END
};
