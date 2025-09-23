#pragma once

#pragma once

#undef val
#undef var
#include <toml++/toml.h>
#include <boost/hana.hpp>
#include <boost/lexical_cast.hpp>

#include "../ext/concepts.h"

namespace hana = boost::hana;

// 基础类型序列化
template <typename T> auto toToml(const T& value)
{
    if constexpr (Integral<T> || FloatingPoint<T>) {
        return toml::value(value);
    } else if constexpr (Boolean<T>) {
        return toml::value(value);
    } else if constexpr (StringLike<T>) {
        return toml::value(std::string(value));
    } else if constexpr (HanaStruct<T>) {
        return value.toToml();
    } else {
        static_assert(!std::is_same_v<T, T>, "Unsupported type for toToml");
    }
}

// 基础类型反序列化
template <typename T, typename V> void fromToml(const toml::value<V>& tomlValue, T& value)
{
    if constexpr (Integral<T>) {
        value = static_cast<T>(tomlValue.as_integer());
    } else if constexpr (FloatingPoint<T>) {
        value = static_cast<T>(tomlValue.as_floating());
    } else if constexpr (Boolean<T>) {
        value = tomlValue.as_boolean();
    } else if constexpr (StringLike<T>) {
        value = tomlValue.as_string();
    } else if constexpr (HanaStruct<T>) {
        value = T::fromToml(tomlValue);
    } else {
        static_assert(!std::is_same_v<T, T>, "Unsupported type for fromToml");
    }
}

// 容器序列化
template <Container C> auto toToml(const C& container)
{
    auto arr = toml::array {};
    for (const auto& item : container) {
        arr.push_back(toToml(item));
    }
    return arr;
}

// 容器反序列化
template <Container C> void fromToml(const toml::table& tomlValue, C& container)
{
    if (!tomlValue.is_array())
        return;

    const auto& arr = tomlValue.as_array();
    container.clear();

    if constexpr (requires { container.reserve(size_t {}); }) {
        container.reserve(arr->size());
    }

    for (const auto& item : *arr) {
        typename C::value_type val;
        fromToml(item, val);
        if constexpr (requires { container.push_back(val); }) {
            container.push_back(std::move(val));
        } else if constexpr (requires { container.insert(val); }) {
            container.insert(std::move(val));
        }
    }
}

// Map 容器序列化
template <MapContainer M> auto toToml(const M& map)
{
    auto tbl = toml::table {};
    for (const auto& [key, value] : map) {
        if constexpr (StringLike<decltype(key)>) {
            tbl[key] = toToml(value);
        } else {
            tbl[std::to_string(key)] = toToml(value);
        }
    }
    return tbl;
}

// Map 容器反序列化
template <MapContainer M> void fromToml(const toml::table& tomlValue, M& map)
{
    if (!tomlValue.is_table())
        return;

    const auto& tbl = tomlValue.as_table();
    map.clear();

    for (const auto& [key, value] : *tbl) {
        typename M::key_type k;
        if constexpr (StringLike<typename M::key_type>) {
            k = key;
        } else {
            k = boost::lexical_cast<typename M::key_type>(key);
        }

        typename M::mapped_type v;
        fromToml(value, v);
        map.emplace(std::move(k), std::move(v));
    }
}

// Variant 序列化
template <typename... Ts> auto toToml(const std::variant<Ts...>& var)
{
    return std::visit([](const auto& v) { return toToml(v); }, var);
}

// 主序列化宏
#define TOMLStruct(NAME, ...)                                   \
    struct NAME {                                               \
        BOOST_HANA_DEFINE_STRUCT(NAME, __VA_ARGS__);            \
                                                                \
        [[nodiscard]] auto toToml() const {                     \
            auto tbl = toml::table{};                           \
            hana::for_each(hana::keys(*this), [&](auto key) {   \
                constexpr auto name = hana::to<char const*>(key); \
                tbl[name] = ::toToml(hana::at_key(*this, key)); \
            });                                                 \
            return toml::value(tbl);                            \
        }                                                       \
                                                                \
        static NAME fromToml(const toml::value& tomlValue) {    \
            NAME obj;                                           \
            if (!tomlValue.is_table()) return obj;              \
            const auto& tbl = tomlValue.as_table();             \
            hana::for_each(hana::keys(obj), [&](auto key) {     \
                constexpr auto name = hana::to<char const*>(key); \
                if (tbl.contains(name)) {                       \
                    ::fromToml(tbl.at(name), hana::at_key(obj, key)); \
                }                                               \
            });                                                 \
            return obj;                                         \
        }                                                       \
    };