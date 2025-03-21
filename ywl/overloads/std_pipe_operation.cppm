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
    struct constexpr_pipe_t : pipe_flag_t {
        std::tuple<Args...> args;

        static_assert((!std::is_rvalue_reference_v <Args> && ...));

        constexpr constexpr_pipe_t() = delete;

        constexpr constexpr_pipe_t(std::tuple<Args...> args) : args{std::move(args)} {}

        constexpr decltype(auto)
        operator()(auto &&target) requires std::is_invocable_v<decltype(fn), decltype(target), Args...> {
            return std::apply(fn, std::tuple_cat(std::forward_as_tuple(std::forward<decltype(target)>(target)), args));
        }
    };

    export template<auto fn, typename... Args>
    constexpr auto constexpr_pipe(Args &&...args) {
        return constexpr_pipe_t<fn, Args...>{std::tuple < Args...>{ std::forward<Args>(args)... }};
    }

    template<typename Fn, typename... Args>
    struct pipe_t : pipe_flag_t {
        Fn fn;
        std::tuple<Args...> args;

        static_assert((!std::is_rvalue_reference_v <Args> && ...));

        constexpr pipe_t() = delete;

        constexpr pipe_t(Fn fn, std::tuple<Args...> args) : fn{std::move(fn)}, args{std::move(args)} {}

        constexpr decltype(auto)
        operator()(auto &&target) requires std::is_invocable_v<Fn, decltype(target), Args...> {
            return std::apply(fn, std::tuple_cat(std::forward_as_tuple(std::forward<decltype(target)>(target)), args));
        }
    };

    export template<typename Fn, typename... Args>
    constexpr auto pipe(Fn &&fn, Args &&...args) {
        return pipe_t<Fn, Args...>{std::forward<Fn>(fn), std::tuple < Args...>{ std::forward<Args>(args)... }};
    }

    namespace ops {
        struct unwrap_impl_t {
            constexpr unwrap_impl_t() = default;

            template<typename T>
            constexpr decltype(auto) operator()(T &&x) const {
                return *(std::forward<T>(x));
            }
        };

        export constexpr auto unwrap = constexpr_pipe<unwrap_impl_t{}>();

        struct unwrap_or_impl_t {
            constexpr unwrap_or_impl_t() = default;

            template<typename T, typename U>
            constexpr auto operator()(T &&x, U &&y) const {
                return x ? *x : std::forward<U>(y);
            }
        };

        export constexpr auto unwrap_or(auto &&y) {
            return constexpr_pipe<unwrap_or_impl_t{}>(std::forward<decltype(y)>(y));
        }

        struct unwrap_or_else_impl_t {
            constexpr unwrap_or_else_impl_t() = default;

            template<typename T, typename U>
            constexpr auto operator()(T &&x, U &&y) const {
                return x ? *x : y();
            }
        };

        export constexpr auto unwrap_or_else(auto &&y) {
            return constexpr_pipe<unwrap_or_else_impl_t{}>(std::forward<decltype(y)>(y));
        }

        struct unwrap_or_throw_impl_t {
            constexpr unwrap_or_throw_impl_t() = default;

            template<typename T, typename E>
            constexpr decltype(auto) operator()(T &&x, E &&e) const {
                if (x) {
                    return *(std::forward<T>(x));
                } else {
                    throw std::forward<E>(e);
                }
            }
        };

        export constexpr auto unwrap_or_throw(auto &&e) {
            return constexpr_pipe<unwrap_or_throw_impl_t{}>(std::forward<decltype(e)>(e));
        }

        struct sorted_impl_t {
            constexpr sorted_impl_t() = default;

            template<typename Container_Type, typename Compare>
            constexpr decltype(auto) operator()(Container_Type &&container, Compare &&compare) const {
                std::sort(container.begin(), container.end(), std::forward<Compare>(compare));
                return std::forward<Container_Type>(container);
            }
        };

        export template<typename Compare = std::less<void>>
        constexpr decltype(auto) sorted(Compare &&compare = Compare{}) {
            return constexpr_pipe<sorted_impl_t{}>(std::forward<Compare>(compare));
        }
    }
}

export template<typename Target_Type, std::convertible_to<const ywl::overloads::pipe_flag_t &> Flag_Type>
requires std::invocable<Flag_Type, Target_Type>
constexpr decltype(auto) operator|(Target_Type &&target, Flag_Type &&fn) {
    return fn(std::forward<Target_Type>(target));
}