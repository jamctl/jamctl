#pragma once
#include "alias.h"
#include "file.h"
#include <ranges>
#include <vector>

template<typename... Args>
auto vectorOf(Args&&... args) -> Vector<std::common_type_t<Args...>> {
    return {std::forward<Args>(args)...};
}

template<typename... Args>
auto listOf(Args&&... args) -> List<std::common_type_t<Args...>> {
    return {std::forward<Args>(args)...};
}

template<typename... Args>
auto mutableListOf(Args&&... args) -> MutableList<std::common_type_t<Args...>> {
    return {std::forward<Args>(args)...};
}

template<typename... Args>
auto dequeOf(Args&&... args) -> Deque<std::common_type_t<Args...>> {
    return {std::forward<Args>(args)...};
}

template<typename... Args>
auto mutableDequeOf(Args&&... args) -> MutableDeque<std::common_type_t<Args...>> {
    return {std::forward<Args>(args)...};
}

template <typename T>
Optional<T> tryParse(const String& str) {
    std::istringstream iss(str);
    T value;
    iss >> value;
    return iss.eof() && !iss.fail() ? std::make_optional(value) : std::nullopt;
}

inline Vector<String> split(const String& str, char delim, bool skip_empty = true) {
    std::string_view sv(str);
    auto split_view = sv | std::views::split(delim);

    Vector<String> result;
    auto it = split_view.begin();
    auto end = split_view.end();

    while (it != end) {
        auto token_range = *it;
        std::string_view token_sv(token_range.begin(), token_range.end());

        if (!(skip_empty && token_sv.empty())) {
            result.emplace_back(token_sv);
        }
        ++it;
    }
    return result;
}

template <typename T> Json::Value JValue(const String& name, T value)
{
    auto json = Json::Value();
    json[name] = value;
    return json;
}