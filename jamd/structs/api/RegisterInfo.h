/**
 * @file RegisterInfo.h
 * @brief RegisterInfo结构体定义
 * @author Mojave Hao
 * @date 2025/7/17
 * @version 0.1.0
 * @license AGPLv3
 */

#pragma once
#include <drogon/HttpResponse.h>
#include "../../ext/alias.h"
#include "../../ext/jsonSerialization.h"

namespace jamd::structs::api::user
{
    APIStruct(RegisterInfo,
        (String, name),
        (String, password),
        (String, email))
}
