#pragma once

#include <concepts>
#include <type_traits>
#include <string>
#include <boost/hana.hpp>

template <typename T> concept Integral = std::is_integral_v<T> && !std::same_as<T, bool>;

template <typename T> concept FloatingPoint = std::is_floating_point_v<T>;

template <typename T> concept StringLike = std::convertible_to<T, std::string_view>;

template <typename T> concept Boolean = std::same_as<T, bool>;

template <typename T> concept Container = requires(T t) {
    { t.begin() } -> std::input_iterator;
    { t.end() } -> std::input_iterator;
    typename T::value_type;
};

template <typename T> concept StringContainer = Container<T> && requires(T container) {
    requires std::same_as < typename T::value_type, std::string >
             || std::same_as<typename T::value_type, std::string_view>;
};

template <typename T> concept MapContainer = Container<T> && requires(T t) {
    typename T::key_type;
    typename T::mapped_type;
};

template <typename T> concept HanaStruct = boost::hana::Struct<T>::value;

template <typename T> concept JsonSerializable = requires(const T& obj, const Json::Value& json) {
    { obj.toJson() } -> std::convertible_to<Json::Value>;
    { T::fromJson(json) } -> std::convertible_to<T>;
    requires std::is_same_v<decltype(T::fromJson(json)), T>;
};