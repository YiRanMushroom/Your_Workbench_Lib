export module ywl.overloads.std_pipe_operation;

import ywl.std.prelude;
import ywl.basic.exceptions;

namespace ywl::overloads {
    struct pipe_flag_t {
        explicit constexpr pipe_flag_t() = default;

        constexpr pipe_flag_t(const pipe_flag_t &) = delete;

        constexpr pipe_flag_t(pipe_flag_t &&) = delete;

        constexpr pipe_flag_t &operator=(const pipe_flag_t &) = delete;

        constexpr pipe_flag_t &operator=(pipe_flag_t &&) = delete;
    };

    template<auto fn, typename... Args>
    struct constexpr_pipe_t {
        std::tuple<Args...> args;

        static_assert((!std::is_rvalue_reference_v <Args> && ...));

        constexpr constexpr_pipe_t() = delete;

        constexpr decltype(auto)
        operator()(auto &&target) requires std::is_invocable_v<decltype(fn), decltype(target), Args...> {
            return std::apply(fn, std::tuple_cat(std::forward_as_tuple(target), args));
        }
    };

    export template<auto fn, typename... Args>
    constexpr auto constexpr_pipe(Args &&...args) {
        return constexpr_pipe_t<fn, Args...>{std::tuple < Args...>{ std::forward<Args>(args)... }};
    }

    template<typename Fn, typename... Args>
    struct pipe_t {
        Fn fn;
        std::tuple<Args...> args;

        static_assert((!std::is_rvalue_reference_v <Args> && ...));

        constexpr pipe_t() = delete;

        constexpr decltype(auto)
        operator()(auto &&target) requires std::is_invocable_v<Fn, decltype(target), Args...> {
            return std::apply(fn, std::tuple_cat(std::forward_as_tuple(target), args));
        }
    };

    export template<typename Fn, typename... Args>
    constexpr auto pipe(Fn &&fn, Args &&...args) {
        return pipe_t<Fn, Args...>{std::forward<Fn>(fn), std::tuple < Args...>{ std::forward<Args>(args)... }};
    }
}

template<typename Target_Type, std::convertible_to<ywl::overloads::pipe_flag_t> Flag_Type>
requires std::invocable<Flag_Type, Target_Type>
constexpr decltype(auto) operator|(Target_Type &&target, Flag_Type && fn) {
    return fn(std::forward<Target_Type>(target));
}