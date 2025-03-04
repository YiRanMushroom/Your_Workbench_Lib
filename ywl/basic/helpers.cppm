module;

#include <utility>

export module ywl.basic.helpers;

namespace ywl::basic {
    export template<auto fn>
    struct function_t {
        static decltype(auto) operator()(auto &&... args) requires requires {
            fn(std::forward<decltype(args)>(args)...);
        } {
            return fn(std::forward<decltype(args)>(args)...);
        }
    };

    export struct nothing_fn_t {
        constexpr void operator()(auto &&...) const noexcept {}
    };
}
