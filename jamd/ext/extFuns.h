#pragma once
#include "alias.h"

template<typename... Args>
auto vectorOf(Args&&... args) -> Vector<std::common_type_t<Args...>> {
    return {std::forward<Args>(args)...};
}

template<typename... Args>
auto listOf(Args&&... args) -> List<std::common_type_t<Args...>> {
    return {std::forward<Args>(args)...};
}

template<typename... Args>
auto dequeOf(Args&&... args) -> Deque<std::common_type_t<Args...>> {
    return {std::forward<Args>(args)...};
}

template<typename... Args>
auto mutableListOf(Args&&... args) -> MutableList<std::common_type_t<Args...>> {
    return {std::forward<Args>(args)...};
}

template<typename... Args>
auto mutableDequeOf(Args&&... args) -> MutableDeque<std::common_type_t<Args...>> {
    return {std::forward<Args>(args)...};
}

template <typename T>
std::optional<T> tryParse(const std::string& str) {
    std::istringstream iss(str);
    T value;
    iss >> value;
    return iss.eof() && !iss.fail() ? std::make_optional(value) : std::nullopt;
}