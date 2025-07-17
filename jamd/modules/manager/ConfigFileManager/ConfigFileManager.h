#pragma once

#include "../../../structs/config/JamdConfig.h"
#include "../../../utils/Singleton.h"

namespace jamd::managers
{
    class ConfigFileManager final : public Singleton<ConfigFileManager>
    {
        friend class Singleton;

        ConfigFileManager()
        {
            daemonConfig = YAML::LoadFile("jamd.yaml").as<JamdConfig>();
        }

    public:
        JamdConfig daemonConfig;
    };
}
