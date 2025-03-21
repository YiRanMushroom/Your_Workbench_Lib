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

        export constexpr decltype(auto) unwrap() {
            return constexpr_pipe<unwrap_impl_t{}>();
        }

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

        struct sort_impl_t {
            constexpr sort_impl_t() = default;

            template<typename Container_Type, typename Compare>
            constexpr decltype(auto) operator()(Container_Type &&container, Compare &&compare) const {
                std::sort(std::begin(container), std::end(container), std::forward<Compare>(compare));
                return std::forward<Container_Type>(container);
            }
        };

        export template<typename Compare = std::less<void>>
        constexpr decltype(auto) sort(Compare &&compare = Compare{}) {
            return constexpr_pipe<sort_impl_t{}>(std::forward<Compare>(compare));
        }

        struct clone_impl_t {
            constexpr clone_impl_t() = default;

            template<typename T>
            constexpr auto operator()(T &&x) const {
                return std::forward<T>(x);
            }
        };

        export constexpr auto clone() {
            return constexpr_pipe<clone_impl_t{}>();
        }

        struct move_impl_t {
            constexpr move_impl_t() = default;

            template<typename T>
            constexpr auto operator()(T &&x) const {
                return std::move(x);
            }
        };

        export constexpr auto move() {
            return constexpr_pipe<move_impl_t{}>();
        }

        struct map_impl_t {
            constexpr map_impl_t() = default;

            template<typename Container_Type, typename Fn>
            constexpr auto operator()(Container_Type &&container, Fn &&fn) const {
                std::transform(std::begin(container), std::end(container), std::begin(container), std::forward<Fn>(fn));
                return std::forward<Container_Type>(container);
            }
        };

        export template<typename Fn>
        constexpr auto map(Fn &&fn) {
            return constexpr_pipe<map_impl_t{}>(std::forward<Fn>(fn));
        }

        struct mapped_impl_t {
            constexpr mapped_impl_t() = default;

            template<typename Target_Value_Type, typename Template_Type>
            struct fill_template {
                static_assert(false, "Cannot find the mapped value type.");
            };

            template<typename Target_Value_Type, template<typename...> typename Template_Type, typename First_Type, typename... Rest_Types>
            struct fill_template<Target_Value_Type, Template_Type<First_Type, Rest_Types...>> {
                using type = Template_Type<Target_Value_Type>;
            };

            template<typename Container_Type, typename Fn> requires (
            std::is_rvalue_reference_v < Container_Type &&> && !std::is_same_v<
                    typename std::remove_cvref_t<Container_Type>::value_type,
                    std::invoke_result_t<Fn, typename std::remove_cvref_t<Container_Type>::value_type>>)

            constexpr auto operator()(Container_Type &&container, Fn &&fn) const {
                using target_container_type = typename
                fill_template<std::invoke_result_t < Fn, typename std::remove_cvref_t<Container_Type>::value_type>,
                std::remove_cvref_t < Container_Type >> ::type;

                target_container_type target_container;

                std::transform(std::begin(container), std::end(container), std::back_inserter(target_container),
                               std::forward<Fn>(fn));

                return target_container;
            }
        };

        export template<typename Fn>
        constexpr auto mapped(Fn &&fn) {
            return constexpr_pipe<mapped_impl_t{}>(std::forward<Fn>(fn));
        }
    }
}

export template<typename Target_Type, std::convertible_to<const ywl::overloads::pipe_flag_t &> Flag_Type>
requires std::invocable<Flag_Type, Target_Type>
constexpr decltype(auto) operator|(Target_Type &&target, Flag_Type &&fn) {
    return fn(std::forward<Target_Type>(target));
}