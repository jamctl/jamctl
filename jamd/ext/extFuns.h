#pragma once
#include "alias.h"
#include "file.h"
#include <ranges>
#include <vector>

template <typename... Args>
auto vectorOf(Args&&... args) -> Vector<std::common_type_t<Args...>>
{
    return {std::forward<Args>(args)...};
}

template <typename... Args>
auto listOf(Args&&... args) -> List<std::common_type_t<Args...>>
{
    return {std::forward<Args>(args)...};
}

template <typename... Args>
auto mutableListOf(Args&&... args) -> MutableList<std::common_type_t<Args...>>
{
    return {std::forward<Args>(args)...};
}

template <typename... Args>
auto dequeOf(Args&&... args) -> Deque<std::common_type_t<Args...>>
{
    return {std::forward<Args>(args)...};
}

template <typename... Args>
auto mutableDequeOf(Args&&... args) -> MutableDeque<std::common_type_t<Args...>>
{
    return {std::forward<Args>(args)...};
}

template <typename T>
Optional<T> tryParse(const String& str)
{
    std::istringstream iss(str);
    T value;
    iss >> value;
    return iss.eof() && !iss.fail() ? std::make_optional(value) : std::nullopt;
}

inline Vector<String> split(const String& str, char delim, const bool skip_empty = true)
{
    StringView sv(str);
    auto splitView = sv | std::views::split(delim);

    Vector<String> result;
    auto it = splitView.begin();
    const auto end = splitView.end();

    while (it != end)
    {
        auto token_range = *it;

        if (StringView token_sv(token_range.begin(), token_range.end()); !(skip_empty && token_sv.empty()))
            result.emplace_back(token_sv);
        ++it;
    }
    return result;
}

template <typename T>
Json::Value JValue(const String& name, T value)
{
    auto json = Json::Value();
    json[name] = value;
    return json;
}

template <typename T>
Json::Value JArray(const Vector<T>& values)
{
    auto arr = Json::Value(Json::arrayValue);
    for (const auto& value : values)
    {
        arr.append(value);
    }
    return arr;
}

template <typename Container>
String expand(const Container& container, const String& delim = ", ", int pos = 0)
{
    using T = typename Container::value_type;
    static_assert(
        std::is_same_v<T, String> || std::is_same_v<T, StringView>,
        "Only supports String or StringView elements"
    );

    if (container.empty()) return "";

    // 计算实际起始索引
    const auto size = container.size();
    if (pos < 0)
    {
        pos += size; // 将负索引转换为正索引
        if (pos < 0) pos = 0; // 处理转换后仍为负的情况..?
    }
    if (static_cast<size_t>(pos) >= size) return ""; // 索引超出范围

    // 定位起始迭代器
    auto it = container.begin();
    std::advance(it, pos); // 移动到指定位置

    // 构建结果字符串
    String result = *it;
    while (++it != container.end())
    {
        result += delim;
        result += *it;
    }

    return result;
}
