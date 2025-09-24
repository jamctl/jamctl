/**
 * @file GeneralAPIResult.h
 * @brief GeneralAPIResult结构体定义
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
    * @struct GeneralAPIResult
    * @brief 储存启动实例API的结果
    */

    APIStruct(GeneralAPIResult, // NOLINT(*-pro-type-member-init)
              (bool, success),
              (int, code),
              (String, message))
}
