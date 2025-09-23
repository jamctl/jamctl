#pragma once

#include <drogon/HttpController.h>
#include <drogon/HttpTypes.h>
#include <drogon/utils/Utilities.h>

#define GET(route, fun, ...) METHOD_ADD(fun, route, Get, \
"jamd::plugins::RequestLogger",\
"jamd::plugins::AutoAddHeaders",\
"jamd::plugins::CheckFeatureEnabled", ##__VA_ARGS__);

#define POST(route, fun, ...) METHOD_ADD(fun, route, Post, \
"jamd::plugins::RequestLogger",\
"jamd::plugins::AutoAddHeaders",\
"jamd::plugins::CheckFeatureEnabled", ##__VA_ARGS__);