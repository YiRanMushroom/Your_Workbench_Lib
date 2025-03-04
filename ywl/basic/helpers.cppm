module;

#include <tuple>
#include <utility>

export module ywl.basic.helpers;

namespace ywl::basic {
    export template<auto * fn>
    struct function_t {
        template<typename... Tps> requires requires
        {
            fn(std::declval<Tps>()...);
        }
        static auto operator()(Tps &&... args)
            noexcept(noexcept(fn(std::forward<decltype(args)>(args)...)))
            -> decltype(fn(std::forward<Tps>(args)...)) {
            return fn(std::forward<decltype(args)>(args)...);
        }
    };

    export struct nothing_fn_t {
        constexpr void operator()(auto &&...) const noexcept {
        }
    };
}
