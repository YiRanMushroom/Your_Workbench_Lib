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

    template<std::size_t N, std::size_t... Is>
    struct make_reverse_index_sequence_impl :
            make_reverse_index_sequence_impl<N - 1, N - 1, Is...> {
    };

    template<std::size_t... Is>
    struct make_reverse_index_sequence_impl<0, Is...> {
        using type = std::index_sequence<Is...>;
    };

    export template<std::size_t N>
    using reverse_index_sequence_t = typename make_reverse_index_sequence_impl<N>::type;

    export template<size_t N>
    using index_sequence_t = std::make_index_sequence<N>;
}
