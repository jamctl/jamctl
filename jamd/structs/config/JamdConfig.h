#pragma once

#include <yaml-cpp/yaml.h>

#include "../../ext/yamlSerialization.h"
#include "../../ext/alias.h"

struct JamdInstancesConfig {
    String save_path = "config";

    friend YAML::Emitter& operator<<(YAML::Emitter& out, const JamdInstancesConfig& obj)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "save_path";
        out << YAML::Value << obj.save_path;
        out << YAML::EndMap;
        return out;
    }

    friend bool operator>>(const YAML::Node& node, JamdInstancesConfig& obj)
    {
        // if (!node.IsMap() || node.size() < 1) return false;
        if (const YAML::Node& field = node["save_path"]) {
            obj.save_path = field.as<String>();
        }
        // else { return false; }
        return true;
    }
};

template <> struct YAML::convert<JamdInstancesConfig> {
    static Node encode(const JamdInstancesConfig& rhs)
    {
        Emitter out;
        out << rhs;
        return Load(out.c_str());
    }

    // ReSharper disable once CppDFAConstantFunctionResult
    static bool decode(const Node& node, JamdInstancesConfig& rhs) { return node >> rhs; }
};

struct JamdFeaturesConfig {
    bool enable_register = true;
    bool enable_grpc_server_reflection = true;

    friend YAML::Emitter& operator<<(YAML::Emitter& out, const JamdFeaturesConfig& obj)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "enable_register";
        out << YAML::Value << obj.enable_register;
        out << YAML::Key << "enable_grpc_server_reflection";
        out << YAML::Value << obj.enable_grpc_server_reflection;
        out << YAML::EndMap;
        return out;
    }

    friend bool operator>>(const YAML::Node& node, JamdFeaturesConfig& obj)
    {
        // if (!node.IsMap() || node.size() < 1) return false;
        if (const YAML::Node& field = node["enable_register"]) {
            obj.enable_register = field.as<bool>();
        }
        // else { return false; }
        if (const YAML::Node& field = node["enable_grpc_server_reflection"]) {
            obj.enable_grpc_server_reflection = field.as<bool>();
        }
        return true;
    }
};

template <> struct YAML::convert<JamdFeaturesConfig> {
    static Node encode(const JamdFeaturesConfig& rhs)
    {
        Emitter out;
        out << rhs;
        return Load(out.c_str());
    }

    // ReSharper disable once CppDFAConstantFunctionResult
    static bool decode(const Node& node, JamdFeaturesConfig& rhs) { return node >> rhs; }
};

struct JamdSecurityArgon2Config {
    uint32_t timecost = 2;
    uint32_t memcost = 65536;
    uint32_t parallelism = 1;
    size_t length = 32;

    friend YAML::Emitter& operator<<(YAML::Emitter& out, const JamdSecurityArgon2Config& obj)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "timecost";
        out << YAML::Value << obj.timecost;
        out << YAML::Key << "memcost";
        out << YAML::Value << obj.memcost;
        out << YAML::Key << "parallelism";
        out << YAML::Value << obj.parallelism;
        out << YAML::Key << "length";
        out << YAML::Value << obj.length;
        out << YAML::EndMap;
        return out;
    }

    friend bool operator>>(const YAML::Node& node, JamdSecurityArgon2Config& obj)
    {
        if (!node.IsMap() || node.size() < 4)
            return false;
        if (const YAML::Node& field = node["timecost"]) {
            obj.timecost = field.as<uint32_t>();
        } else { return false; }
        if (const YAML::Node& field = node["memcost"]) {
            obj.memcost = field.as<uint32_t>();
        } else { return false; }
        if (const YAML::Node& field = node["parallelism"]) {
            obj.parallelism = field.as<uint32_t>();
        } else { return false; }
        if (const YAML::Node& field = node["length"]) {
            obj.length = field.as<size_t>();
        } else { return false; }
        return true;
    }
};

template <> struct YAML::convert<JamdSecurityArgon2Config> {
    static Node encode(const JamdSecurityArgon2Config& rhs)
    {
        Emitter out;
        out << rhs;
        return Load(out.c_str());
    }

    static bool decode(const Node& node, JamdSecurityArgon2Config& rhs) { return node >> rhs; }
};

struct JamdSecurityConfig {
    JamdSecurityArgon2Config argon2 {};

    friend YAML::Emitter& operator<<(YAML::Emitter& out, const JamdSecurityConfig& obj)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "argon2";
        out << YAML::Value << obj.argon2;
        out << YAML::EndMap;
        return out;
    }

    friend bool operator>>(const YAML::Node& node, JamdSecurityConfig& obj)
    {
        // if (!node.IsMap() || node.size() < 1) return false;
        if (const YAML::Node& field = node["argon2"]) {
            obj.argon2 = field.as<JamdSecurityArgon2Config>();
        }
        // else { return false; }
        return true;
    }
};

template <> struct YAML::convert<JamdSecurityConfig> {
    static Node encode(const JamdSecurityConfig& rhs)
    {
        Emitter out;
        out << rhs;
        return Load(out.c_str());
    }

    // ReSharper disable once CppDFAConstantFunctionResult
    static bool decode(const Node& node, JamdSecurityConfig& rhs) { return node >> rhs; }
};

struct JamdLoggingConfig {
    String level = "debug";

    friend YAML::Emitter& operator<<(YAML::Emitter& out, const JamdLoggingConfig& obj)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "level";
        out << YAML::Value << obj.level;
        out << YAML::EndMap;
        return out;
    }

    friend bool operator>>(const YAML::Node& node, JamdLoggingConfig& obj)
    {
        // if (!node.IsMap() || node.size() < 1) return false;
        if (const YAML::Node& field = node["level"]) {
            obj.level = field.as<String>();
        }
        // else { return false; }
        return true;
    }
};

template <> struct YAML::convert<JamdLoggingConfig> {
    static Node encode(const JamdLoggingConfig& rhs)
    {
        Emitter out;
        out << rhs;
        return Load(out.c_str());
    }

    // ReSharper disable once CppDFAConstantFunctionResult
    static bool decode(const Node& node, JamdLoggingConfig& rhs) { return node >> rhs; }
};

struct JamdConfig {
    JamdInstancesConfig instances {};
    JamdFeaturesConfig features {};
    JamdSecurityConfig security {};
    JamdLoggingConfig log {};

    friend YAML::Emitter& operator<<(YAML::Emitter& out, const JamdConfig& obj)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "instances";
        out << YAML::Value << obj.instances;
        out << YAML::Key << "features";
        out << YAML::Value << obj.features;
        out << YAML::Key << "security";
        out << YAML::Value << obj.security;
        out << YAML::Key << "log";
        out << YAML::Value << obj.log;
        out << YAML::EndMap;
        return out;
    }

    friend bool operator>>(const YAML::Node& node, JamdConfig& obj)
    {
        // if (!node.IsMap() || node.size() < 3) return false;
        if (const YAML::Node& field = node["instances"]) {
            obj.instances = field.as<JamdInstancesConfig>();
        }
        // else { return false; }
        if (const YAML::Node& field = node["features"]) {
            obj.features = field.as<JamdFeaturesConfig>();
        }
        // else { return false; }
        if (const YAML::Node& field = node["security"]) {
            obj.security = field.as<JamdSecurityConfig>();
        }
        // else { return false; }
        if (const YAML::Node& field = node["log"]) {
            obj.log = field.as<JamdLoggingConfig>();
        }
        return true;
    }
};

template <> struct YAML::convert<JamdConfig> {
    static Node encode(const JamdConfig& rhs)
    {
        Emitter out;
        out << rhs;
        return Load(out.c_str());
    }

    // ReSharper disable once CppDFAConstantFunctionResult
    static bool decode(const Node& node, JamdConfig& rhs) { return node >> rhs; }
};