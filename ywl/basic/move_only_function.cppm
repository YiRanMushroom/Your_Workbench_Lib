module;

#include <stdc++pch.hpp>

export module ywl.basic.move_only_function;

import ywl.basic.helpers;
import ywl.basic.exceptions;

namespace ywl::basic {
    export template<typename>
    class move_only_function;

    template<typename Ret, typename... Args>
    class move_only_function_impl {
    public:
        constexpr move_only_function_impl() = default;

        constexpr move_only_function_impl(const move_only_function_impl &) = delete;

        constexpr move_only_function_impl(move_only_function_impl &&) = default;

        constexpr move_only_function_impl &operator=(const move_only_function_impl &) = delete;

        constexpr move_only_function_impl &operator=(move_only_function_impl &&) = default;

        using return_type = Ret;
        using argument_types = std::tuple<Args...>;

    private:
        struct move_only_function_base_t {
            constexpr virtual ~move_only_function_base_t() = default;

            constexpr virtual Ret operator()(Args...) = 0;
        };

    public:
        using base_type = move_only_function_base_t;

    private:
        std::unique_ptr<base_type> m_base;

    public:
        constexpr operator bool() const {
            return m_base != nullptr;
        }

        constexpr bool has_value() const {
            return m_base != nullptr;
        }

        constexpr Ret operator()(Args... args) const {
            if (!m_base) {
                throw ywl::basic::null_pointer_exception{"move only function does not have a value"};
            }
            return (*m_base)(std::forward<Args>(args)...);
        }

    private:
        template<ywl::basic::function_like<Ret(Args...)> F>
        struct derive_base_t : base_type {
            using function_type = F;

            function_type m_function;

            constexpr derive_base_t(F f) : m_function(std::move(f)) {}

            constexpr Ret operator()(Args... args) override {
                return m_function(std::forward<Args>(args)...);
            }
        };

        friend class move_only_function<Ret(Args...)>;

    public:
        constexpr move_only_function_impl(std::unique_ptr<base_type> base) : m_base(std::move(base)) {}

        constexpr move_only_function_impl(std::nullptr_t) : m_base(nullptr) {}

        constexpr void reset() {
            m_base.reset();
        }

        constexpr Ret invoke(Args... args) const {
            return (*this)(std::forward<Args>(args)...);
        }

        constexpr void swap(move_only_function_impl &other) {
            std::swap(m_base, other.m_base);
        }
    };

    export template<typename>
    class move_only_function {
        static_assert(false, "Template parameter is not a function type");
    };

    export template<typename Ret, typename... Args>
    class move_only_function<Ret(Args...)> : public move_only_function_impl<Ret, Args...> {
    private:
        using impl_type = move_only_function_impl<Ret, Args...>;

    public:
        using typename impl_type::base_type;
        using typename impl_type::return_type;
        using typename impl_type::argument_types;


        using impl_type::has_value;
        using impl_type::operator();
        using impl_type::operator bool;
        using impl_type::invoke;
        using impl_type::reset;
        using impl_type::swap;

        constexpr move_only_function() = default;

        constexpr move_only_function(const move_only_function &) = delete;

        constexpr move_only_function(move_only_function &&) = default;

        constexpr move_only_function &operator=(const move_only_function &) = delete;

        constexpr move_only_function &operator=(move_only_function &&) = default;

        constexpr move_only_function(std::nullptr_t) : impl_type{nullptr} {}

        template<ywl::basic::function_like<Ret(Args...)> F>
        constexpr move_only_function(F &&f) : impl_type{
            std::make_unique<typename impl_type::template derive_base_t<std::remove_reference_t<F>>>(
                std::forward<F>(f))
        } {}
    };

    export template<typename Ret, typename... Args>
    move_only_function(Ret (*)(Args...)) -> move_only_function<Ret(Args...)>;

    export template<typename T>
    move_only_function(T) -> move_only_function<std::remove_pointer_t<decltype(+std::declval<T>())>>;

    export template<typename Ret, typename... Args>
    move_only_function(std::function<Ret(Args...)>) -> move_only_function<Ret(Args...)>;
}
