#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class User final : public drogon::HttpController<User> {
public:
    static void initPathRouting();

    // your declaration of processing function maybe like this:
    // void get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, int p1, std::string p2);
    // void your_method_name(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, double p1, int p2) const;
};
