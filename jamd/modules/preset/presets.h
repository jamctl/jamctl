#pragma once

#include <yaml-cpp/yaml.h>

typedef std::uint64_t hash_t;

constexpr hash_t prime = 0x100000001B3ull;
constexpr hash_t basis = 0xCBF29CE484222325ull;

inline hash_t hash_(char const* str)
{
    hash_t ret { basis };

    while (*str) {
        ret ^= *str;
        ret *= prime;
        str++;
    }

    return ret;
}

constexpr hash_t hash_compile_time(char const* str, hash_t last_value = basis)
{
    return *str ? hash_compile_time(str + 1, (*str ^ last_value) * prime) : last_value;
}

constexpr unsigned long long operator ""_hash(char const* p, size_t)
{
    return hash_compile_time(p);
}

enum class Preset {
    Vanilla,
    Paper,
    Spigot,
    BukkitLegacy,
    Bukkit, // Bukkit & Spigot 1.14+
    Fabric,
    Forge,
    CatServer,
    Arclight,
    Bungeecord,
    Waterfall,
    Velocity
};

inline String getPresetName(const Preset p)
{
    using enum Preset;
    switch (p) {
        case Vanilla:
            return "Vanilla";
        case Paper:
            return "Paper";
        case Spigot:
            return "Spigot";
        case BukkitLegacy:
            return "BukkitLegacy";
        case Bukkit:
            return "Bukkit";
        case Fabric:
            return "Fabric";
        case Forge:
            return "Forge";
        case CatServer:
            return "CatServer";
        case Arclight:
            return "Arclight";
        case Bungeecord:
            return "Bungeecord";
        case Waterfall:
            return "Waterfall";
        case Velocity:
            return "Velocity";
        default:
            return "Vanilla";
    }
}

inline Preset getPreset(const String& name)
{
    switch (hash_(name.c_str())) {
        case "Vanilla"_hash:
        return Preset::Vanilla;
        case "Paper"_hash:
        return Preset::Paper;
        case "Spigot"_hash:
        return Preset::Spigot;
        case "BukkitLegacy"_hash:
        return Preset::BukkitLegacy;
        case "Bukkit"_hash:
        return Preset::Bukkit;
        case "Fabric"_hash:
        return Preset::Fabric;
        case "Forge"_hash:
        return Preset::Forge;
        case "CatServer"_hash:
        return Preset::CatServer;
        case "Arclight"_hash:
        return Preset::Arclight;
        case "Bungeecord"_hash:
        return Preset::Bungeecord;
        case "Waterfall"_hash:
        return Preset::Waterfall;
        case "Velocity"_hash:
        return Preset::Velocity;
        default:
            return Preset::Vanilla;
    }
}

template <> struct YAML::convert<Preset> {
    static Node encode(const Preset& rhs) { return Node(getPresetName(rhs)); }

    static bool decode(const Node& node, Preset& rhs)
    {
        if (!node.IsScalar())
            return false;
        rhs = getPreset(node.as<String>());
        return true;
    }
};