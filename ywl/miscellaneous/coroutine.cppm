export module ywl.miscellaneous.coroutine;

import ywl.std.prelude;
import ywl.basic.exceptions;

namespace ywl::miscellaneous::coroutine {
    template<typename ResultType, typename Co_Await_Type>
    class coroutine_generator_task_impl_t {
    private:
        struct coroutine_generator_promise_type {
            std::optional<ResultType> result;

            std::exception_ptr exception;

            constexpr coroutine_generator_task_impl_t get_return_object() {
                return coroutine_generator_task_impl_t{
                    std::coroutine_handle<coroutine_generator_promise_type>::from_promise(*this)
                };
            }

            constexpr std::suspend_always initial_suspend() { // NOLINT
                return {};
            }

            constexpr std::suspend_always final_suspend() noexcept { // NOLINT
                return {};
            }

            constexpr void unhandled_exception() {
                exception = std::current_exception();
            }

            constexpr void return_void() { // NOLINT
            }

            constexpr std::suspend_always yield_value(ResultType value) {
                result = std::move(value);
                return {};
            }

        private:
            struct Awaiter {
                Co_Await_Type co_await_value;

                [[nodiscard]] constexpr static bool await_ready() noexcept {
                    return false;
                }

                constexpr void await_suspend(std::coroutine_handle<coroutine_generator_promise_type>) {
                    // do nothing
                }

                constexpr Co_Await_Type await_resume() {
                    return std::move(co_await_value);
                }

                constexpr explicit Awaiter(Co_Await_Type co_await_value) : co_await_value{std::move(co_await_value)} {
                }
            };

            std::optional<Awaiter> awaiter;

        public:
            constexpr Awaiter await_transform(nullptr_t) {
                if (!awaiter) {
                    throw ywl::basic::ywl_impl_error{"Implementation error in ywl: coroutine_generator_task_impl_t"};
                }
                return std::exchange(awaiter, std::nullopt).value();
            }

            constexpr void set_awaiter(Co_Await_Type co_await_value) {
                awaiter.emplace(std::move(co_await_value));
            }

            ~coroutine_generator_promise_type() {
                if (exception) {
                    auto e = exception;
                    exception = nullptr;
                    std::rethrow_exception(e);
                }
            }

            constexpr void rethrow_if_has_exception() {
                if (exception) {
                    auto e = exception;
                    exception = nullptr;
                    std::rethrow_exception(e);
                }
            }
        };

    public:
        using promise_type = coroutine_generator_promise_type;
        using handle_type = std::coroutine_handle<promise_type>;

    private:
        handle_type handle;

    public:
        constexpr explicit coroutine_generator_task_impl_t(handle_type handle) : handle{handle} {
        }

        constexpr coroutine_generator_task_impl_t(const coroutine_generator_task_impl_t &) = delete;

        constexpr coroutine_generator_task_impl_t(coroutine_generator_task_impl_t &&) = delete;

        constexpr coroutine_generator_task_impl_t &operator=(const coroutine_generator_task_impl_t &) = delete;

        constexpr coroutine_generator_task_impl_t &operator=(coroutine_generator_task_impl_t &&) = delete;

        constexpr bool is_done() const {
            return handle.done();
        }

        constexpr operator bool() const {
            return !is_done();
        }

        constexpr ~coroutine_generator_task_impl_t() {
            if (handle) {
                handle.destroy();
            }
        }

        constexpr std::optional<ResultType> yield_optional(Co_Await_Type co_await_value) {
            handle.promise().set_awaiter(std::move(co_await_value));
            while (!handle.done()) {
                handle.promise().rethrow_if_has_exception();
                handle.resume();
                if (handle.promise().result) {
                    return std::exchange(handle.promise().result, std::nullopt);
                }
            }

            return {};
        }

        constexpr ResultType yield_value(Co_Await_Type co_await_value) {
            return *yield_optional(std::move(co_await_value));
        }

        constexpr void rethrow_if_has_exception() {
            handle.promise().rethrow_if_has_exception();
        }
    };

    template<typename ResultType>
    class coroutine_generator_task_impl_t<ResultType, void> {
    private:
        struct coroutine_generator_promise_type {
            std::optional<ResultType> result;

            std::exception_ptr exception;

            constexpr coroutine_generator_task_impl_t get_return_object() {
                return coroutine_generator_task_impl_t{
                    std::coroutine_handle<coroutine_generator_promise_type>::from_promise(*this)
                };
            }

            constexpr std::suspend_always initial_suspend() noexcept { // NOLINT
                return {};
            }

            constexpr std::suspend_always final_suspend() noexcept { // NOLINT
                return {};
            }

            constexpr void unhandled_exception() {
                exception = std::current_exception();
            }

            constexpr void return_void() { // NOLINT
            }

            constexpr std::suspend_always yield_value(ResultType value) {
                result = std::move(value);
                return {};
            }

        private:
            struct Awaiter {
                [[nodiscard]] constexpr static bool await_ready() noexcept {
                    return false;
                }

                constexpr void await_suspend(std::coroutine_handle<coroutine_generator_promise_type>) {
                    // do nothing
                }

                constexpr static void await_resume() {
                    // do nothing
                }

                constexpr Awaiter() = default;
            };

        public:
            constexpr Awaiter await_transform(nullptr_t) {
                return {};
            }

            constexpr void set_awaiter() {
            }

            ~coroutine_generator_promise_type() {
                if (exception) {
                    auto e = exception;
                    exception = nullptr;
                    std::rethrow_exception(e);
                }
            }

            constexpr void rethrow_if_has_exception() {
                if (exception) {
                    auto e = exception;
                    exception = nullptr;
                    std::rethrow_exception(e);
                }
            }
        };

    public:
        using promise_type = coroutine_generator_promise_type;
        using handle_type = std::coroutine_handle<promise_type>;

    private:
        handle_type handle;

    public:
        constexpr explicit coroutine_generator_task_impl_t(handle_type handle) : handle{handle} {
        }

        constexpr coroutine_generator_task_impl_t(const coroutine_generator_task_impl_t &) = delete;

        constexpr coroutine_generator_task_impl_t(coroutine_generator_task_impl_t &&) = delete;

        constexpr coroutine_generator_task_impl_t &operator=(const coroutine_generator_task_impl_t &) = delete;

        constexpr coroutine_generator_task_impl_t &operator=(coroutine_generator_task_impl_t &&) = delete;

        constexpr bool is_done() const {
            return handle.done();
        }

        constexpr operator bool() const {
            return !is_done();
        }

        constexpr ~coroutine_generator_task_impl_t() {
            if (handle) {
                handle.destroy();
            }
        }

        constexpr std::optional<ResultType> yield_optional() {
            while (!handle.done()) {
                handle.promise().rethrow_if_has_exception();
                handle.resume();
                if (handle.promise().result) {
                    return std::exchange(handle.promise().result, std::nullopt);
                }
            }

            return {};
        }

        constexpr ResultType yield_value() {
            return *yield_optional();
        }

        constexpr void rethrow_if_has_exception() {
            handle.promise().rethrow_if_has_exception();
        }
    };

    template<typename Function_Type>
    struct generator_task_impl {
        static_assert(false, "template parameter of generator_task must be a function type");
    };

    template<typename ResultType, typename Arg>
    struct generator_task_impl<ResultType(Arg)> {
        using type = coroutine_generator_task_impl_t<ResultType, Arg>;
    };

    template<typename ResultType>
    struct generator_task_impl<ResultType()> {
        using type = coroutine_generator_task_impl_t<ResultType, void>;
    };

    export template<typename Function_Type>
    using generator_task = typename generator_task_impl<Function_Type>::type;
}
