#pragma once

#include <yaml-cpp/yaml.h>

#include "../../ext/yamlSerialization.h"
#include "../../ext/alias.h"

YAMLStruct(JamdInstancesConfig,
           (String, save_path))

YAMLStruct(JamdFeaturesConfig,
           (bool, enable_register))

YAMLStruct(JamdConfig,
           (JamdInstancesConfig, instances),
           (JamdFeaturesConfig, features))
