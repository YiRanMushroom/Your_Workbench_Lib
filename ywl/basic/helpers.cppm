module;

export module ywl.basic.helpers;

import ywl.std.prelude;

namespace ywl::basic {
    template<auto, typename>
    struct function_t_crtp {
        static_assert(false, "Fn_T is not a function or function pointer type");
    };

    template<auto * fn, typename Ret_T, typename... Args_T>
    struct function_t_crtp<fn, Ret_T(*)(Args_T...)> {
        static_assert(std::is_invocable_v<decltype(fn), Args_T...>,
                      "The function pointer type does not match the function signature");
        using function_ptr_type = Ret_T(*)(Args_T...);

        constexpr Ret_T operator()(Args_T... args) const noexcept(noexcept(fn(std::forward<Args_T>(args)...))) {
            return fn(std::forward<Args_T>(args)...);
        }

        constexpr operator function_ptr_type() const noexcept { // NOLINT
            return fn;
        }
    };

    export template<auto *fn>
    struct function_t : public function_t_crtp<fn, decltype(fn)> {
    };


    export struct nothing_fn_t {
        constexpr void operator()(auto &&...) const noexcept {
        }
    };

    template<size_t N, size_t... Is>
    struct make_reverse_index_sequence_impl :
            make_reverse_index_sequence_impl<N - 1, N - 1, Is...> {
    };

    template<size_t... Is>
    struct make_reverse_index_sequence_impl<0, Is...> {
        using type = std::index_sequence<Is...>;
    };

    export template<size_t N>
    using reverse_index_sequence_t = typename make_reverse_index_sequence_impl<N>::type;

    export template<size_t N>
    using index_sequence_t = std::make_index_sequence<N>;

    template<typename Tp, typename T>
    constexpr bool function_like_impl_v = false;

    template<typename Tp, typename Ret_T, typename ...Args_T>
    constexpr bool function_like_impl_v<Tp, Ret_T(Args_T...)> = requires(Tp t, Args_T... args) {
        { t(std::forward<Args_T>(args)...) } -> std::convertible_to<Ret_T>;
    };

    export template<typename Tp, typename T>
    concept function_like = function_like_impl_v<Tp, T>;

    template<typename Tp, typename T>
    constexpr bool function_same_as_impl_v = false;

    template<typename Tp, typename Ret_T, typename ...Args_T>
    constexpr bool function_same_as_impl_v<Tp, Ret_T(Args_T...)> = requires(Tp t, Args_T... args) {
        { t(std::forward<Args_T>(args)...) } -> std::same_as<Ret_T>;
    };

    export template<typename Tp, typename T>
    concept function_same_as = function_same_as_impl_v<Tp, T>;
}
