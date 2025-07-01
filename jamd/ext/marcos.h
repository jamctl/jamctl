#pragma once

#include <drogon/HttpController.h>
#include <drogon/HttpTypes.h>
#include <drogon/utils/Utilities.h>

#undef val
#define var auto
#define val const auto
#define castTo static_cast
#define dynamicCastTo dynamic_cast

#define GET(route, fun, ...) METHOD_ADD(fun, route, Get, "jamd::plugins::RequestLogger", "jamd::plugins::AutoAddHeaders", ##__VA_ARGS__);
#define POST(route, fun, ...) METHOD_ADD(fun, route, Post, "jamd::plugins::RequestLogger", "jamd::plugins::AutoAddHeaders", ##__VA_ARGS__);

// #define eq ,CompareOperator::EQ,

// #define contains ,drogon::orm::CompareOperator::In,
// #define notContains ,drogon::orm::CompareOperator::NotIn,