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

            coroutine_generator_task_impl_t get_return_object() {
                return coroutine_generator_task_impl_t{
                    std::coroutine_handle<coroutine_generator_promise_type>::from_promise(*this)
                };
            }

            std::suspend_always initial_suspend() { // NOLINT
                return {};
            }

            std::suspend_always final_suspend() noexcept { // NOLINT
                return {};
            }

            void unhandled_exception() {
                exception = std::current_exception();
            }

            void return_void() { // NOLINT
            }

            std::suspend_always yield_value(ResultType value) {
                result = std::move(value);
                return {};
            }

        private:
            struct Awaiter {
                Co_Await_Type co_await_value;

                [[nodiscard]] static constexpr bool await_ready() noexcept {
                    return false;
                }

                void await_suspend(std::coroutine_handle<coroutine_generator_promise_type>) {
                    // do nothing
                }

                Co_Await_Type await_resume() {
                    return std::move(co_await_value);
                }

                explicit Awaiter(Co_Await_Type co_await_value) : co_await_value{std::move(co_await_value)} {
                }
            };

            std::optional<Awaiter> awaiter;

        public:
            Awaiter await_transform(nullptr_t) {
                if (!awaiter) {
                    throw ywl::basic::ywl_impl_error{"Implementation error in ywl: coroutine_generator_task_impl_t"};
                }
                return std::exchange(awaiter, std::nullopt).value();
            }

            void set_awaiter(Co_Await_Type co_await_value) {
                awaiter.emplace(std::move(co_await_value));
            }

            ~coroutine_generator_promise_type() {
                if (exception) {
                    auto e = exception;
                    exception = nullptr;
                    std::rethrow_exception(e);
                }
            }

            void rethrow_if_has_exception() {
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
        explicit coroutine_generator_task_impl_t(handle_type handle) : handle{handle} {
        }

        coroutine_generator_task_impl_t(const coroutine_generator_task_impl_t &) = delete;

        coroutine_generator_task_impl_t(coroutine_generator_task_impl_t &&) = delete;

        coroutine_generator_task_impl_t &operator=(const coroutine_generator_task_impl_t &) = delete;

        coroutine_generator_task_impl_t &operator=(coroutine_generator_task_impl_t &&) = delete;

        bool is_done() const {
            return handle.done();
        }

        operator bool() const {
            return !is_done();
        }

        ~coroutine_generator_task_impl_t() {
            if (handle) {
                handle.destroy();
            }
        }

        std::optional<ResultType> yield_optional(Co_Await_Type co_await_value) {
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

        ResultType yield_value(Co_Await_Type co_await_value) {
            return *yield_optional(std::move(co_await_value));
        }

        void rethrow_if_has_exception() {
            handle.promise().rethrow_if_has_exception();
        }
    };

    template<typename ResultType>
    class coroutine_generator_task_impl_t<ResultType, void> {
    private:
        struct coroutine_generator_promise_type {
            std::optional<ResultType> result;

            std::exception_ptr exception;

            coroutine_generator_task_impl_t get_return_object() {
                return coroutine_generator_task_impl_t{
                    std::coroutine_handle<coroutine_generator_promise_type>::from_promise(*this)
                };
            }

            std::suspend_always initial_suspend() noexcept { // NOLINT
                return {};
            }

            std::suspend_always final_suspend() noexcept { // NOLINT
                return {};
            }

            void unhandled_exception() {
                exception = std::current_exception();
            }

            void return_void() { // NOLINT
            }

            std::suspend_always yield_value(ResultType value) {
                result = std::move(value);
                return {};
            }

        private:
            struct Awaiter {
                [[nodiscard]] static constexpr bool await_ready() noexcept {
                    return false;
                }

                void await_suspend(std::coroutine_handle<coroutine_generator_promise_type>) {
                    // do nothing
                }

                static void await_resume() {
                    // do nothing
                }

                Awaiter() = default;
            };

        public:
            Awaiter await_transform(nullptr_t) {
                return {};
            }

            void set_awaiter() {
            }

            ~coroutine_generator_promise_type() {
                if (exception) {
                    auto e = exception;
                    exception = nullptr;
                    std::rethrow_exception(e);
                }
            }

            void rethrow_if_has_exception() {
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
        explicit coroutine_generator_task_impl_t(handle_type handle) : handle{handle} {
        }

        coroutine_generator_task_impl_t(const coroutine_generator_task_impl_t &) = delete;

        coroutine_generator_task_impl_t(coroutine_generator_task_impl_t &&) = delete;

        coroutine_generator_task_impl_t &operator=(const coroutine_generator_task_impl_t &) = delete;

        coroutine_generator_task_impl_t &operator=(coroutine_generator_task_impl_t &&) = delete;

        bool is_done() const {
            return handle.done();
        }

        operator bool() const {
            return !is_done();
        }

        ~coroutine_generator_task_impl_t() {
            if (handle) {
                handle.destroy();
            }
        }

        std::optional<ResultType> yield_optional() {
            while (!handle.done()) {
                handle.promise().rethrow_if_has_exception();
                handle.resume();
                if (handle.promise().result) {
                    return std::exchange(handle.promise().result, std::nullopt);
                }
            }

            return {};
        }

        ResultType yield_value() {
            return *yield_optional();
        }

        void rethrow_if_has_exception() {
            handle.promise().rethrow_if_has_exception();
        }
    };

    export template<typename T>
    struct generator_task {
        static_assert(false, "T is not an acceptable function type");
    };

    export template<typename Ret_T, typename Arg_T>
    struct generator_task<Ret_T(Arg_T)> : coroutine_generator_task_impl_t<Ret_T, Arg_T> {

    };
}
