#pragma once

#include <yaml-cpp/yaml.h>

#include "../../ext/yamlSerialization.h"
#include "../../ext/alias.h"

YAMLStruct(JamdInstancesConfig,
           (String, save_path))

YAMLStruct(JamdFeaturesConfig,
           (bool, enable_register))

YAMLStruct(JamdSecurityArgon2Config,
           (uint32_t, timecost),
           (uint32_t, memcost),
           (uint32_t, parallelism),
           (size_t, length))

YAMLStruct(JamdSecurityConfig,
           (JamdSecurityArgon2Config, argon2))

YAMLStruct(JamdConfig,
           (JamdInstancesConfig, instances),
           (JamdFeaturesConfig, features),
           (JamdSecurityConfig, security))
