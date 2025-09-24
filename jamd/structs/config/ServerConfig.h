#pragma once
#include <yaml-cpp/yaml.h>
#include "../../ext/alias.h"
#include "../../modules/manager/InstanceManager/SingleServer.hpp"
#include "../../modules/preset/presets.h"

struct ServerConfig {
    int id {};
    Optional<unsigned short> xms;
    Optional<unsigned short> xmx;
    Optional<Preset> preset;
    String name;
    String executor;
    String workdir;
    String target;
    Vector<String> params;

    Optional<SingleServer> instance() const
    {
        auto p = Vector<StringView> {};
        for (auto param : params)
            p.emplace_back(StringView(param));
        return std::make_optional<SingleServer>(
            executor.contains("java"),
            executor,
            xms.value_or(1),
            xmx.value_or(4),
            workdir,
            target,
            std::move(p)
            );
    }

    friend YAML::Emitter& operator<<(YAML::Emitter& out, const ServerConfig& obj)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "id";
        out << YAML::Value << obj.id;
        out << YAML::Key << "name";
        out << YAML::Value << obj.name;
        out << YAML::Key << "executor";
        out << YAML::Value << obj.executor;
        out << YAML::Key << "params";
        out << YAML::Value << obj.params;
        if (obj.xms.has_value()) {
            out << YAML::Key << "xms";
            out << YAML::Value << obj.xms.value();
        }
        if (obj.xmx.has_value()) {
            out << YAML::Key << "xmx";
            out << YAML::Value << obj.xmx.value();
        }
        if (obj.preset.has_value()) {
            out << YAML::Key << "preset";
            out << YAML::Value << getPresetName(obj.preset.value());
        }
        out << YAML::EndMap;
        return out;
    }

    friend bool operator>>(const YAML::Node& node, ServerConfig& obj)
    {
        if (!node.IsMap() || node.size() < 4)
            return false;
        if (const YAML::Node& field = node["id"])
            obj.id = field.as<int>();
        else
            return false;
        if (const YAML::Node& field = node["name"])
            obj.name = field.as<String>();
        else
            return false;
        if (const YAML::Node& field = node["executor"])
            obj.executor = field.as<String>();
        else
            return false;
        if (const YAML::Node& field = node["params"])
            obj.params = field.as<Vector<String>>();
        else
            return false;
        if (const YAML::Node& field = node["xms"])
            obj.xms = field.as<unsigned short>();
        else
            obj.xms = std::nullopt;
        if (const YAML::Node& field = node["xmx"])
            obj.xmx = field.as<unsigned short>();
        else
            obj.xmx = std::nullopt;
        if (const YAML::Node& field = node["preset"])
            obj.preset = field.as<Preset>();
        else
            obj.preset = std::nullopt;
        return true;
    }
};

template <> struct YAML::convert<ServerConfig> {
    static Node encode(const ServerConfig& rhs)
    {
        Emitter out;
        out << rhs;
        return Load(out.c_str());
    }

    static bool decode(const Node& node, ServerConfig& rhs) { return node >> rhs; }
};

using ConfFilePair = Pair<String, ServerConfig>;
