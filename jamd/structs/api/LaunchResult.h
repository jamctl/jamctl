/**
 * @file LaunchResult.h
 * @brief LaunchResult结构体定义
 * @author Mojave Hao
 * @date 2025/5/29
 * @version 0.1.0
 * @license AGPLv3
 */

#pragma once
#include "../../ext/alias.h"
#include "../../ext/jsonSerialization.h"

namespace jamd::structs::api::instances
{
    /**
    * @struct LaunchResult
    * @brief 储存启动实例API的结果
    */

    APIStruct(LaunchResult, // NOLINT(*-pro-type-member-init)
              (bool, success),
        (
    int
   ,
    code
    )
   ,
    (String
   ,
    message
    )
    )
}