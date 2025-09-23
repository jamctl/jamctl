#pragma once

#include <variant>
#include <iostream>
#include <type_traits>
#include "alias.h"

template <typename T, typename... Cases> class PatternMatcher {
    const T& value;
    std::tuple<Cases...> cases;

    public:
    explicit PatternMatcher(const T& val, Cases&&... cs) :
        value(val), cases(std::forward<Cases>(cs)...) {}

    template <typename R = void> auto execute() -> Optional<R>
    {
        return execute_impl<R>(std::index_sequence_for<Cases...> {});
    }

    private:
    template <typename R, std::size_t... Is> auto execute_impl(std::index_sequence<Is...>) -> Optional<R>
    {
        Optional<R> result;

        (std::get<Is>(cases)(value, result) || ...);

        return result;
    }
};

template <typename T, typename... Cases> auto match(const T& value, Cases&&... cases)
{
    return PatternMatcher<T, Cases...>(value, std::forward<Cases>(cases)...);
}

template <typename Pattern, typename Action> auto when(Pattern&& pattern, Action&& action)
{
    return [pattern = std::forward<Pattern>(pattern),
            action = std::forward<Action>(action)]<typename V, typename R>(const V& value, R& result)
    {
        if constexpr (std::is_invocable_v<Pattern, V>) {
            if (auto match_result = pattern(value); match_result.has_value()) {
                if constexpr (std::is_same_v<R, std::nullopt_t>) {
                    action(match_result.value());
                } else {
                    result = action(match_result.value());
                }
                return true;
            }
        } else {
            if (auto ptr = std::get_if<Pattern>(&value)) {
                if constexpr (std::is_same_v<R, std::nullopt_t>) {
                    action(*ptr);
                } else {
                    result = action(*ptr);
                }
                return true;
            }
        }
        return false;
    };
}
