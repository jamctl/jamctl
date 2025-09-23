#pragma once

#include <spdlog/spdlog.h>

#include "../../../structs/config/JamdConfig.h"
#include "../../../utils/Singleton.h"

namespace jamd::managers
{
    class ConfigFileManager final : public Singleton<ConfigFileManager> {
        friend class Singleton;

        ConfigFileManager()
        {
            try {
                daemonConfig = YAML::LoadFile("jamd.yaml").as<JamdConfig>();
            } catch (YAML::BadFile) {
                daemonConfig = JamdConfig();
                spdlog::info("Using default config");
            }
        }

        public:
            JamdConfig daemonConfig;
    };
}