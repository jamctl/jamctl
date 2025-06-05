#pragma once
#include <drogon/HttpAppFramework.h>
#include <drogon/orm/Criteria.h>
#include <drogon/orm/Result.h>

#include <drogon/orm/DbClient.h>
#include <drogon/utils/coroutine.h>
#include <functional>
#include <memory>

using namespace drogon;
using namespace drogon::orm;

class TransactionManager {
public:
    template<typename Func>
    static Task<> transaction(const DbClientPtr& client, Func&& func) {
        auto trans = co_await client->newTransaction();
        try {
            co_await func(trans);
            co_await trans->commit();
        } catch (const DrogonDbException& e) {
            co_await trans->rollback();
            throw;
        }
    }

    template<typename Func>
    static Task<> transaction(Func&& func) {
        co_await transaction(app().getDbClient(), std::forward<Func>(func));
    }
};
