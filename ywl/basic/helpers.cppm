module;

#include <tuple>
#include <utility>

export module ywl.basic.helpers;

namespace ywl::basic {
    template<auto, typename>
    struct function_t_crtp {
        static_assert(false, "Fn_T is not a function or function pointer type");
    };

    template<auto * fn, typename Ret_T, typename... Args_T>
    struct function_t_crtp<fn, Ret_T(*)(Args_T...)> {
        static_assert(std::is_invocable_v<decltype(fn), Args_T...>,
                      "The function pointer type does not match the function signature");
        using type = Ret_T(Args_T...);

        constexpr Ret_T operator()(Args_T... args) const noexcept(noexcept(fn(std::forward<Args_T>(args)...))) {
            return fn(std::forward<Args_T>(args)...);
        }
    };

    export template<auto *fn>
    struct function_t : public function_t_crtp<fn, decltype(fn)> {
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
