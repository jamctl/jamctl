#pragma once

#include <yaml-cpp/yaml.h>
#include <boost/preprocessor.hpp>

// 宏展开辅助工具
#define YAML_STRUCT_EMPTY()
#define YAML_STRUCT_DEFER(...) __VA_ARGS__ YAML_STRUCT_EMPTY()
#define YAML_STRUCT_EXPAND(...) __VA_ARGS__

// 成员变量声明
#define YAML_STRUCT_MEMBER_DECL(r, data, elem) \
    BOOST_PP_TUPLE_ELEM(0, elem) BOOST_PP_TUPLE_ELEM(1, elem);

// 序列化实现
#define YAML_STRUCT_SERIALIZE_IMPL(r, data, elem) \
    out << ::YAML::Key << BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(1, elem)); \
    out << ::YAML::Value << obj.BOOST_PP_TUPLE_ELEM(1, elem);

// 反序列化实现
#define YAML_STRUCT_DESERIALIZE_IMPL(r, data, elem) \
    if (const ::YAML::Node& field = node[BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(1, elem))]) { \
        obj.BOOST_PP_TUPLE_ELEM(1, elem) = field.as<std::remove_reference_t<decltype(obj.BOOST_PP_TUPLE_ELEM(1, elem))>>(); \
    } else { \
        return false; \
    }

// 主宏定义
#define YAMLStruct(struct_name, ...) \
struct struct_name { \
    BOOST_PP_SEQ_FOR_EACH(YAML_STRUCT_MEMBER_DECL, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)) \
    \
    friend ::YAML::Emitter& operator<<(::YAML::Emitter& out, const struct_name& obj) { \
        out << ::YAML::BeginMap; \
        BOOST_PP_SEQ_FOR_EACH(YAML_STRUCT_SERIALIZE_IMPL, obj, \
            BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)) \
        out << ::YAML::EndMap; \
        return out; \
    } \
    \
    friend bool operator>>(const ::YAML::Node& node, struct_name& obj) { \
        if (!node.IsMap() || node.size() < BOOST_PP_VARIADIC_SIZE(__VA_ARGS__)) \
            return false; \
        BOOST_PP_SEQ_FOR_EACH(YAML_STRUCT_DESERIALIZE_IMPL, obj, \
            BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)) \
        return true; \
    } \
}; \
/* 确保convert特化在全局YAML命名空间中定义 */ \
namespace YAML { \
    template<> \
    struct convert<struct_name> { \
        static ::YAML::Node encode(const struct_name& rhs) { \
            ::YAML::Emitter out; \
            out << rhs; \
            return ::YAML::Load(out.c_str()); \
        } \
        static bool decode(const::YAML:: Node& node, struct_name& rhs) { \
            return node >> rhs; \
        } \
    }; \
}

// 枚举支持
#define YAMLEnum(enum_name, ...) \
enum class enum_name { __VA_ARGS__ }; \
namespace YAML { \
    template<> \
    struct convert<enum_name> { \
        static ::YAML::Node encode(const enum_name& rhs) { \
            return ::YAML::Node(static_cast<int>(rhs)); \
        } \
        static bool decode(const ::YAML::Node& node, enum_name& rhs) { \
            if (!node.IsScalar()) return false; \
            rhs = static_cast<enum_name>(node.as<int>()); \
            return true; \
        } \
    }; \
}