module;

#include <iostream>

import ywl.std.prelude;

export module ywl.overloads.std_overloads;

import ywl.basic.string_literal;

export struct ywl_overload_flag_t {
    explicit constexpr ywl_overload_flag_t() = default;
};

export constexpr ywl_overload_flag_t ywl_overload_flag;

namespace std {
    // now define hash_code in a class can be used in std::hash
    export template<typename T> requires requires(const T& t) {
        { t.hash_code(ywl_overload_flag) } -> std::same_as<size_t>;
    }
    struct hash<T> {
        constexpr size_t operator()(const T &t) const {
            return t.hash_code(ywl_overload_flag);
        }
    };
}

export template<ywl::basic::string_literal sl>
constexpr decltype(auto) operator""_fmt() {
    return []<typename... Tps>(const Tps &... args) {
        return std::format(sl.data, args...);
    };
}

export template<typename T> requires requires(const T& t) {
    { t.to_string(ywl_overload_flag) } -> std::convertible_to<std::string_view>;
}
struct std::formatter<T> { // NOLINT
    constexpr static auto parse(std::format_parse_context &ctx) {
        if constexpr (requires {
            { T::parse_fmt(ctx) } -> std::same_as<std::format_parse_context>;
        }) {
            return T::parse_fmt(ctx);
        } else {
            return ctx.begin();
        }
    }

    constexpr static auto format(const T &t, std::format_context &ctx) {
        return std::format_to(ctx.out(), "{}", std::move(t.to_string(ywl_overload_flag)));
    }
};

export template<typename T> requires requires(const T& t) {
    { t.to_string(ywl_overload_flag) } -> std::convertible_to<std::string_view>;
}
std::ostream& operator<<(std::ostream& os, const T& t) {
    return os << t.to_string(ywl_overload_flag);
}
