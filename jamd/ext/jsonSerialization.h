#pragma once

#undef val
#undef var
#include <json/value.h>
#include <boost/hana.hpp>
#include <boost/lexical_cast.hpp>
#include <type_traits>
#include <optional>
#include <vector>
#include <map>
#include <variant>

namespace hana = boost::hana;

// 序列化基础类型和标准容器
template <typename T> Json::Value toJson(const T& value);

template <typename T> void fromJson(const Json::Value& json, T& value);

// 基础类型特化
template <> inline Json::Value toJson<int>(const int& value)
{
    return { value };
}

template <> inline void fromJson<int>(const Json::Value& json, int& value)
{
    value = json.asInt();
}

template <> inline Json::Value toJson<bool>(const bool& value)
{
    return { value };
}

template <> inline void fromJson<bool>(const Json::Value& json, bool& value)
{
    value = json.asBool();
}

// 字符串特化
template <> inline Json::Value toJson<String>(const String& value)
{
    return { value };
}

template <> inline void fromJson<String>(const Json::Value& json, String& value)
{
    value = json.asString();
}

// 序列化适配器：vector
template <typename T> Json::Value toJson(const std::vector<T>& vec)
{
    Json::Value arr(Json::arrayValue);
    for (const auto& item : vec) {
        arr.append(toJson(item));
    }
    return arr;
}

template <typename T> void fromJson(const Json::Value& json, std::vector<T>& vec)
{
    if (!json.isArray())
        return;
    vec.reserve(json.size());
    for (const auto& item : json) {
        T va;
        fromJson(item, va);
        vec.emplace_back(std::move(va));
    }
}

// 序列化适配器：map
template <typename K, typename V> Json::Value toJson(const std::map<K, V>& map)
{
    Json::Value obj(Json::objectValue);
    for (const auto& [key, value] : map) {
        obj[std::to_string(key)] = toJson(value);
    }
    return obj;
}

template <typename K, typename V> void fromJson(const Json::Value& json, std::map<K, V>& map)
{
    if (!json.isObject())
        return;
    for (auto it = json.begin(); it != json.end(); ++it) {
        K key = boost::lexical_cast<K>(it.name());
        V va;
        fromJson(*it, va);
        map.emplace(key, std::move(va));
    }
}

// 序列化适配器：optional
template <typename T> Json::Value toJson(const std::optional<T>& opt)
{
    return opt ? toJson(*opt) : Json::Value::null;
}

template <typename T> void fromJson(const Json::Value& json, std::optional<T>& opt)
{
    if (json.isNull()) {
        opt.reset();
    } else {
        T va;
        fromJson(json, va);
        opt = std::move(va);
    }
}

// 序列化适配器：variant (C++17)
template <typename... Ts> Json::Value toJson(const std::variant<Ts...>& va)
{
    return std::visit([](const auto& v) { return toJson(v); }, va);
}

// 需要为每个variant类型特化fromJson（简化版）

// 主序列化宏
#define APIStruct(NAME, ...)                                   \
    struct NAME {                                                             \
        BOOST_HANA_DEFINE_STRUCT(NAME, __VA_ARGS__);                          \
                                                                              \
        [[nodiscard]] Json::Value toJson() const {                                          \
            Json::Value json(Json::objectValue);                                                 \
            hana::for_each(hana::keys(*this), [&](auto key) {                 \
                constexpr auto name = hana::to<char const*>(key);             \
                json[name] = ::toJson(hana::at_key(*this, key));             \
            });                                                               \
            return json;                                                      \
        }                                                                      \
                                                                              \
        static NAME fromJson(const Json::Value& json) {                       \
            NAME obj;                                                         \
            hana::for_each(hana::keys(obj), [&](auto key) {                   \
                constexpr auto name = hana::to<char const*>(key);             \
                if (json.isMember(name)) {                                    \
                    ::fromJson(json[name], hana::at_key(obj, key));          \
                }                                                             \
            });                                                               \
            return obj;                                                       \
        }                                                                     \
                                                                              \
        /* 支持Drogon的HttpResponse */                                      \
        drogon::HttpResponsePtr toResponse() {                            \
            return drogon::HttpResponse::newHttpJsonResponse(toJson());       \
        }                                                                    \
    };

// 嵌套结构体支持
template <typename T> Json::Value toJson(const T& value)
{
    if constexpr (hana::Struct<T>::value) {
        return value.toJson();
    } else {
        static_assert(!std::is_same_v<T, T>,
                      "Type not supported. Please provide toJson specialization");
    }
    return {};
}

template <typename T> void fromJson(const Json::Value& json, T& value)
{
    if constexpr (hana::Struct<T>::value) {
        value = T::fromJson(json);
    } else {
        static_assert(!std::is_same_v<T, T>,
                      "Type not supported. Please provide fromJson specialization");
    }
}