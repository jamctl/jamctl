#pragma once

#include <ranges>
#include <vector>
#include "alias.h"
#include "file.h"
#include "concepts.h"

template <typename... Args> auto vectorOf(Args&&... args) -> Vector<std::common_type_t<Args...>>
{
    return { std::forward<Args>(args)... };
}

template <typename... Args> auto listOf(Args&&... args) -> List<std::common_type_t<Args...>>
{
    return { std::forward<Args>(args)... };
}

template <typename... Args> auto mutableListOf(Args&&... args) -> MutableList<std::common_type_t<Args...>>
{
    return { std::forward<Args>(args)... };
}

template <typename... Args> auto dequeOf(Args&&... args) -> Deque<std::common_type_t<Args...>>
{
    return { std::forward<Args>(args)... };
}

template <typename... Args> auto mutableDequeOf(Args&&... args) -> MutableDeque<std::common_type_t<Args...>>
{
    return { std::forward<Args>(args)... };
}

template <typename T> Optional<T> tryParse(const String& str)
{
    std::istringstream iss(str);
    T value;
    iss >> value;
    return iss.eof() && !iss.fail() ? std::make_optional(value) : std::nullopt;
}

template <StringContainer R = Vector<String>> R split(
    const StringLike auto& str,
    char delim,
    const bool skip_empty = true
)
{
    StringView sv(str);
    auto splitView = sv | std::views::split(delim);

    R result;
    auto it = splitView.begin();
    const auto end = splitView.end();

    while (it != end) {
        auto token_range = *it;

        if (StringView token_sv(token_range.begin(), token_range.end()); !(skip_empty && token_sv.empty()))
            result.emplace_back(token_sv);
        ++it;
    }
    return result;
}

Json::Value JValue(const String& name, auto value)
{
    auto json = Json::Value();
    json[name] = value;
    return json;
}

Json::Value JArray(const Container auto& values)
{
    auto arr = Json::Value(Json::arrayValue);
    for (const auto& value : values) {
        arr.append(value);
    }
    return arr;
}

template <StringLike R = String> R expand(
    const StringContainer auto& container,
    const String& delim = ", ",
    int pos = 0
)
{
    if (container.empty())
        return "";

    // 预计算所需大小
    size_t total_size = 0;
    auto it = container.begin();
    std::advance(it, pos);
    for (; it != container.end(); ++it) {
        total_size += it->size() + delim.size();
    }
    total_size -= delim.size();

    R result;
    result.reserve(total_size);

    it = container.begin();
    std::advance(it, pos);
    result = *it;
    while (++it != container.end()) {
        result += delim;
        result += *it;
    }
    return result;
}