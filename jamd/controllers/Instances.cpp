#include "Instances.h"

void Instances::initPathRouting() {
    METHOD_ADD(Instances::StartInstance, "/start", Get);
}

void Instances::StartInstance(const HttpRequestPtr &req,
                              std::function<void(const HttpResponsePtr &)> &&callback) {
    auto resp=HttpResponse::newHttpResponse();
    resp->setStatusCode(static_cast<HttpStatusCode>(200));
    callback(resp);
}
