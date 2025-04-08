module;

#include <stdc++pch.hpp>

export module ywl.basic.string_literal;

// import ywl.std.prelude;

namespace ywl::basic {
    export template<size_t N>
    struct string_literal {
        char data[N];

        constexpr size_t length() const noexcept {
            return N - 1;
        }

        constexpr string_literal(const char (&str)[N]) : data{} { // NOLINT
            std::copy_n(str, N, data);
        }
    };
}
