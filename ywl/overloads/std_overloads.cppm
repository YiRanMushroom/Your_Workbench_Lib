module;

#include <format>

export module ywl.overloads.std_overloads;

import ywl.basic.string_literal;

namespace std {
    // now define hash_code in a class can be used in std::hash
    export template<typename T> requires requires(T t) { { t.hash_code() } -> std::same_as<size_t>; }
    struct hash<T> {
        constexpr size_t operator()(const T &t) const {
            return t.hash_code();
        }
    };
}

export template<ywl::basic::string_literal sl>
constexpr decltype(auto) operator""_fmt() {
    return []<typename... Tps>(const Tps &... args) {
        return std::format(sl.data, args...);
    };
}

export template<typename T> requires (requires(T t) { t.to_string(); } && T::ywl_define_formatter == true)
struct std::formatter<T> { // NOLINT
    constexpr static auto parse(std::format_parse_context &ctx) {
        if constexpr (requires { { T::parse_fmt(ctx) } -> std::same_as<std::format_parse_context>; }) {
            return T::parse_fmt(ctx);
        } else {
            return ctx.begin();
        }
    }

    constexpr static auto format(const T &t, std::format_context &ctx) {
        return std::format_to(ctx.out(), "{}", t.to_string());
    }
};

namespace ywl {
    export template<typename T>
    struct format_type_function_ud {
        /*constexpr static auto to_string(const T &t) = delete;

        constexpr static std::format_parse_context parse_fmt(std::format_parse_context &ctx) = delete;*/
    };
}

export template<typename T> requires (requires(T t) { ywl::format_type_function_ud<T>::to_string(t); })
struct std::formatter<T> { // NOLINT
    constexpr static auto parse(std::format_parse_context &ctx) {
        if constexpr (requires {
            { ywl::format_type_function_ud<T>::parse_fmt(ctx) } -> std::same_as<std::format_parse_context>;
        }) {
            return ywl::format_type_function_ud<T>::parse_fmt(ctx);
        } else {
            return ctx.begin();
        }
    }

    constexpr static auto format(const T &t, std::format_context &ctx) {
        return std::format_to(ctx.out(), "{}", ywl::format_type_function_ud<T>::to_string(t));
    }
};
