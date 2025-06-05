#pragma once
#include <boost/lexical_cast.hpp>
#include <json/value.h>
#include <boost/hana.hpp>
#include <type_traits>
#include <optional>
#include <variant>
#include <chrono>
#include <vector>
#include <map>
#include <any>

namespace hana = boost::hana;
using namespace drogon;

// 基础类型序列化适配器
namespace json_serializer
{
    template <typename T>
    Json::Value toJson(const T& value);

    template <typename T>
    void fromJson(const Json::Value& json, T& value);
}

// 主序列化宏系统
#define APIStruct(NAME, ...) \
    struct NAME { \
        BOOST_HANA_DEFINE_STRUCT(NAME, __VA_ARGS__); \
        \
        /* 支持带初始值的构造函数 */ \
        NAME() = default; \
        template <typename... Args> \
        explicit NAME(Args&&... args) { \
            init_fields(std::forward<Args>(args)...); \
        } \
        \
        void serialize_fields(const NAME& obj, Json::Value& json) const { \
        hana::for_each(hana::keys(obj), [&](auto key) { \
        constexpr auto name = hana::to<char const*>(key); \
        json[name] = json_serializer::toJson(hana::at_key(obj, key)); \
        }); \
        } \
        \
        static void deserialize_fields(const Json::Value& json, NAME& obj) { \
        hana::for_each(hana::keys(obj), [&](auto key) { \
        constexpr auto name = hana::to<char const*>(key); \
        if (json.isMember(name)) { \
        json_serializer::fromJson(json[name], hana::at_key(obj, key)); \
        } \
        }); \
        } \
        Json::Value toJson() const { \
            Json::Value json; \
            serialize_fields(*this, json); \
            return json; \
        } \
        \
        static NAME fromJson(const Json::Value& json) { \
            NAME obj; \
            deserialize_fields(json, obj); \
            return obj; \
        } \
        \
        operator HttpResponsePtr() const { \
            return HttpResponse::newHttpJsonResponse(toJson()); \
        } \
        \
    private: \
        template <typename... Args> \
        void init_fields(Args&&... args) { \
            hana::for_each(hana::keys(*this), [&](auto key) { \
                constexpr auto name = hana::to<char const*>(key); \
                init_field_impl<decltype(key)>(name, std::forward<Args>(args)...); \
            }); \
        } \
        \
        template <typename Key, typename... Args> \
        void init_field_impl(const char* field_name, Args&&... args) { \
            if constexpr (sizeof...(Args) > 0) { \
                (init_single_field<Key>(field_name, std::forward<Args>(args)), ...); \
            } \
        } \
        \
        template <typename Key, typename T> \
        void init_single_field(const char* field_name, T&& value) { \
            if constexpr (std::is_convertible_v<T, std::remove_reference_t< \
                decltype(hana::at_key(*this, Key{}))>>) { \
                hana::at_key(*this, Key{}) = std::forward<T>(value); \
            } \
        } \
    };
// 字段初始化辅助宏
#define FIELD(TYPE, NAME) (TYPE, NAME)
#define FIELD_WITH_DEFAULT(TYPE, NAME, DEFAULT) \
    (TYPE, NAME) = DEFAULT

// 类型序列化实现
namespace json_serializer
{
    // 基础类型
    template <typename T>
    auto toJson(const T& value) -> std::enable_if_t<std::is_arithmetic_v<T>, Json::Value>
    {
        return Json::Value(value);
    }

    template <typename T>
    auto fromJson(const Json::Value& json, T& value) -> std::enable_if_t<std::is_arithmetic_v<T>>
    {
        value = json.as<T>();
    }

    // 字符串
    template <>
    inline Json::Value toJson<std::string>(const std::string& value)
    {
        return Json::Value(value);
    }

    template <>
    inline void fromJson<std::string>(const Json::Value& json, std::string& value)
    {
        value = json.asString();
    }

    // 时间点 (C++20 chrono)
    template <>
    inline Json::Value toJson<std::chrono::system_clock::time_point>(
        const std::chrono::system_clock::time_point& value)
    {
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            value.time_since_epoch()).count();
        return Json::Value(static_cast<Json::Int64>(ms));
    }

    template <>
    inline void fromJson<std::chrono::system_clock::time_point>(
        const Json::Value& json, std::chrono::system_clock::time_point& value)
    {
        const auto ms = std::chrono::milliseconds(json.asInt64());
        value = std::chrono::system_clock::time_point(ms);
    }

    // 可选值 (C++17)
    template <typename T>
    Json::Value toJson(const std::optional<T>& value)
    {
        return value ? toJson(*value) : Json::Value::null;
    }

    template <typename T>
    void fromJson(const Json::Value& json, std::optional<T>& value)
    {
        if (json.isNull())
        {
            value = std::nullopt;
        }
        else
        {
            value = fromJson<T>(json);
        }
    }

    // 变体 (C++17)
    template <typename... Ts>
    Json::Value toJson(const std::variant<Ts...>& value)
    {
        Json::Value result;
        std::visit([&](auto&& arg)
        {
            result["type"] = hana::type_c<decltype(arg)>.name();
            result["value"] = toJson(arg);
        }, value);
        return result;
    }

    template <typename... Ts>
    void fromJson(const Json::Value& json, std::variant<Ts...>& value)
    {
        const std::string typeName = json["type"].asString();
        if (const bool found = (try_set_variant<Ts>(json["value"], typeName, value) || ...); !found)
            throw
                std::runtime_error("Unknown variant type: " + typeName);
    }

    template <typename T, typename Variant>
    bool try_set_variant(const Json::Value& json, const std::string& typeName, Variant& value)
    {
        if (typeName == hana::type_c<T>.name())
        {
            value = fromJson<T>(json);
            return true;
        }
        return false;
    }

    // 任意类型 (C++17)
    template <>
    inline Json::Value toJson<std::any>(const std::any& value)
    {
        // 需要类型注册机制，实际使用应更完善
        Json::Value result;
        if (auto intVal = std::any_cast<int>(&value))
        {
            result = *intVal;
        }
        else if (auto strVal = std::any_cast<std::string>(&value))
        {
            result = *strVal;
        } // 其他类型...
        return result;
    }

    // 容器类型
    template <typename T>
    Json::Value toJson(const std::vector<T>& vec)
    {
        Json::Value arr(Json::arrayValue);
        for (const auto& item : vec)
        {
            arr.append(toJson(item));
        }
        return arr;
    }

    template <typename T>
    void fromJson(const Json::Value& json, std::vector<T>& vec)
    {
        vec.clear();
        for (const auto& item : json)
        {
            vec.push_back(fromJson<T>(item));
        }
    }

    // 映射类型
    template <typename K, typename V>
    Json::Value toJson(const std::map<K, V>& map)
    {
        Json::Value obj(Json::objectValue);
        for (const auto& [key, value] : map)
        {
            obj[std::to_string(key)] = toJson(value);
        }
        return obj;
    }

    template <typename K, typename V>
    void fromJson(const Json::Value& json, std::map<K, V>& map)
    {
        map.clear();
        for (auto it = json.begin(); it != json.end(); ++it)
        {
            K key = boost::lexical_cast<K>(it.name());
            map[key] = fromJson<V>(*it);
        }
    }

    // 递归序列化 (C++17 if constexpr)
    template <typename T>
    auto toJson(const T& value) -> decltype(value.toJson(), Json::Value())
    {
        return value.toJson();
    }

    template <typename T>
    auto fromJson(const Json::Value& json, T& value) -> decltype(T::fromJson(json), void())
    {
        value = T::fromJson(json);
    }

    // 通用反序列化器
    template <typename T>
    T fromJson(const Json::Value& json)
    {
        T value;
        fromJson(json, value);
        return value;
    }

    template <>
    inline Json::Value toJson<short>(const short& value) {
        return value;
    }
}

// 自定义序列化扩展点
#define DEFINE_CUSTOM_SERIALIZER(TYPE, TOJSON_EXPR, FROMJSON_EXPR) \
namespace json_serializer { \
    template <> \
    Json::Value toJson<TYPE>(const TYPE& value) { \
        return TOJSON_EXPR; \
    } \
    \
    template <> \
    void fromJson<TYPE>(const Json::Value& json, TYPE& value) { \
        FROMJSON_EXPR; \
    } \
}

// 条件序列化宏
#define SERIALIZE_IF(COND, FIELD) \
    if (COND) { \
        constexpr auto key = decltype(hana::make_map(hana::make_pair(BOOST_HANA_STRING(#FIELD), FIELD))::key); \
        constexpr auto name = hana::to<char const*>(key); \
        json[name] = json_serializer::toJson(hana::at_key(obj, key)); \
    }
