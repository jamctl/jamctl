#pragma once
#include "../../ext/alias.h"

namespace jamd::structs::config
{
    using InstanceParams = Vector<String>;

    struct MemoryLimit
    {
        int xms;
        int xmx;
    };

    struct ServerConfig
    {
        String name;
        String executor;
        MemoryLimit memoryLimit;
        InstanceParams params;
    };

    using ConfFilePair = Pair<String, ServerConfig>;
}
