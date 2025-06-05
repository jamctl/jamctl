#pragma once

#include <yaml-cpp/yaml.h>

namespace jamd::structs::config
{
    struct JamdConfig
    {
    };
}

template <>
struct YAML::convert<jamd::structs::config::JamdConfig>
{
    static Node encode(const jamd::structs::config::JamdConfig& config)
    {
        Node node;
        return node;
    }

    static jamd::structs::config::JamdConfig decode(const Node& node)
    {
        jamd::structs::config::JamdConfig config;
        return config;
    }
};
