export module ywl.basic.move_only_function;

import ywl.std.prelude;
import ywl.basic.helpers;
import ywl.basic.exceptions;

namespace ywl::basic {
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
        std::unique_ptr <base_type> m_base;

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

            constexpr derive_base_t(function_type &&f) : m_function(std::move(f)) {}

            constexpr Ret operator()(Args... args) override {
                return m_function(std::forward<Args>(args)...);
            }
        };

    public:
        template<ywl::basic::function_like<Ret(Args...)> F>
        constexpr move_only_function_impl(F &&f) : m_base(std::make_unique<derive_base_t<F>>(std::forward<F>(f))) {}

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

    template<typename>
    struct move_only_function_type {
        static_assert(false, "Template parameter is not a function type");
    };

    template<typename Ret, typename... Args>
    struct move_only_function_type<Ret(Args...)> {
        using type = move_only_function_impl<Ret, Args...>;
    };

    // function pointer type
    template<typename Ret, typename... Args>
    struct move_only_function_type<Ret (*)(Args...)> {
        using type = move_only_function_impl<Ret, Args...>;
    };

    export template<typename F>
    using move_only_function = typename move_only_function_type<F>::type;
}