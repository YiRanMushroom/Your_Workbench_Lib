module;

#include <cassert>

export module ywl.misc.coroutine.framework;

import ywl.std.prelude;
import ywl.utils.logger;
import ywl.basic.exceptions;
import ywl.misc.multithreading;

namespace ywl::misc::coroutine {
    export class co_awaitable_base {
    public:
        constexpr virtual ~co_awaitable_base() = default;

        constexpr virtual std::coroutine_handle<> get_handle() = 0;
    };

    export class co_executor_base {
    public:
        constexpr virtual ~co_executor_base() = default;

        constexpr virtual void initial_schedule_task(std::coroutine_handle<> handle) = 0;

        constexpr virtual void schedule_dependency_all(std::coroutine_handle<>, std::coroutine_handle<>) = 0;

        constexpr virtual void schedule_dependency_any(std::coroutine_handle<>, std::coroutine_handle<>) = 0;

        constexpr virtual void run() = 0;
    };

    export inline thread_local co_executor_base *current_executor = nullptr;

    export class simple_co_executor : public co_executor_base {
    private:
        std::queue<std::coroutine_handle<>> m_queue;
        std::unordered_map<std::coroutine_handle<>, std::coroutine_handle<>> dependency_of;
        std::unordered_map<std::coroutine_handle<>, std::unordered_set<std::coroutine_handle<>>> depend_on;

        std::unordered_set<std::coroutine_handle<>> need_any_rather_than_all;
        std::unordered_set<std::coroutine_handle<>> m_queue_elements;

        std::unordered_set<std::coroutine_handle<>> m_can_be_finished_immediately;

    public:
        constexpr void initial_schedule_task(std::coroutine_handle<> handle) override {
            m_queue.push(handle);
            m_queue_elements.insert(handle);
        }

        constexpr void schedule_dependency_all(std::coroutine_handle<> issuer, std::coroutine_handle<> task) override {
            assert(issuer != nullptr);
            m_queue.push(task);
            m_queue_elements.insert(task);

            m_queue_elements.erase(issuer);

            dependency_of[task] = issuer;
            depend_on[issuer].insert(task);
        }

        constexpr void run() override {
            while (!m_queue.empty()) {
                auto handle = m_queue.front();
                m_queue.pop();

                if (!m_queue_elements.contains(handle) || !handle) {
                    continue;
                }

                m_queue_elements.erase(handle);

                if (depend_on.contains(handle)) {
                    m_queue.push(handle);
                    m_queue_elements.insert(handle);
                    continue;
                }

                if (m_can_be_finished_immediately.contains(handle)) {
                    m_can_be_finished_immediately.erase(handle);
                    assert(!depend_on.contains(handle));
                    handle_finish(handle);
                    continue;
                }

                current_executor = this;

                try {
                    handle.resume();
                    if (handle.done()) {
                        handle_finish(handle);
                    }
                } catch (const std::exception &e) {
                    std::rethrow_exception(std::current_exception());
                }
            }
        }

        constexpr void cancel_all_dependencies_of(std::coroutine_handle<> handle) {
            if (depend_on.contains(handle)) {
                for (const auto &other: depend_on.at(handle)) {
                    cancel_all_dependencies_of(other);
                    m_can_be_finished_immediately.insert(other);
                }
                m_can_be_finished_immediately.insert(handle);
            }
        }

        constexpr void handle_finish(std::coroutine_handle<> handle) {
            if (m_can_be_finished_immediately.contains(handle)) {
                m_can_be_finished_immediately.erase(handle);
                return;
            }

            if (!dependency_of.contains(handle)) {
                return;
            }

            auto issuer = dependency_of.at(handle);

            if (need_any_rather_than_all.contains(issuer)) {
                assert(issuer);
                depend_on.at(issuer).erase(handle);
                dependency_of.erase(handle);

                for (const auto &other: depend_on.at(issuer)) {
                    cancel_all_dependencies_of(other);
                    m_can_be_finished_immediately.insert(other);
                }

                if (depend_on.at(issuer).empty()) {
                    need_any_rather_than_all.erase(issuer);
                    m_queue.push(issuer);
                    m_queue_elements.insert(issuer);
                    depend_on.erase(issuer);
                }
            } else {
                depend_on.at(issuer).erase(handle);

                if (depend_on.at(issuer).empty()) {
                    depend_on.erase(issuer);
                    m_queue.push(issuer);
                    m_queue_elements.insert(issuer);
                }

                dependency_of.erase(handle);
            }
        }

        constexpr void schedule_dependency_any(std::coroutine_handle<> issuer, std::coroutine_handle<> task) override {
            need_any_rather_than_all.insert(issuer);

            m_queue_elements.erase(issuer);

            m_queue.push(task);
            m_queue_elements.insert(task);
            dependency_of[task] = issuer;
            depend_on[issuer].insert(task);
        }

        constexpr ~simple_co_executor() override {
            if (!m_queue.empty()) {
                ywl::utils::err_printf_ln("Warning: not all coroutines were finished.").flush();
            }

            if (!depend_on.empty()) {
                ywl::utils::err_printf_ln("Warning: not all dependencies were finished.").flush();
            }

            if (!dependency_of.empty()) {
                ywl::utils::err_printf_ln("Warning: not all dependencies were finished.").flush();
                for (const auto &[task, issuer]: dependency_of) {
                    ywl::utils::err_printf_ln("Coroutine {} is waiting for {}.", issuer.address(), task.address()).
                            flush();
                }
            }

            if (!m_queue_elements.empty()) {
                ywl::utils::err_printf_ln("Warning: not all coroutines were finished.").flush();
                for (const auto &handle: m_queue_elements) {
                    ywl::utils::err_printf_ln("Coroutine {} is not finished.", handle.address()).flush();
                }
            }
        }
    };

    export class multithreaded_co_executor : public co_executor_base {
    private:
        std::queue<std::coroutine_handle<>> m_queue;
        std::unordered_map<std::coroutine_handle<>, std::coroutine_handle<>> dependency_of;
        std::unordered_map<std::coroutine_handle<>, std::unordered_set<std::coroutine_handle<>>> depend_on;

        std::unordered_set<std::coroutine_handle<>> need_any_rather_than_all;
        std::unordered_set<std::coroutine_handle<>> m_queue_elements;

        std::recursive_mutex m_mutex;
        ywl::misc::multithreading::thread_pool m_thread_pool;
        std::list<std::future<std::coroutine_handle<>>> m_unhandled_finished_tasks;
        std::unordered_set<std::coroutine_handle<>> m_can_be_finished_immediately;

    public:
        constexpr multithreaded_co_executor() = delete;

        constexpr explicit multithreaded_co_executor(size_t threads_count) : m_thread_pool{threads_count} {
        }

    public:
        constexpr void initial_schedule_task(std::coroutine_handle<> handle) override {
            std::lock_guard lock(m_mutex);
            m_queue.push(handle);
            m_queue_elements.insert(handle);
        }

        constexpr void schedule_dependency_all(std::coroutine_handle<> issuer, std::coroutine_handle<> task) override {
            std::lock_guard lock(m_mutex);
            assert(issuer != nullptr);
            m_queue.push(task);
            m_queue_elements.insert(task);

            m_queue_elements.erase(issuer);

            dependency_of[task] = issuer;
            depend_on[issuer].insert(task);
        }

        constexpr void run() override {
            while (!m_queue.empty() || !m_unhandled_finished_tasks.empty()) {
                for (auto it = m_unhandled_finished_tasks.begin(); it != m_unhandled_finished_tasks.end();) {
                    if (it->wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
                        auto handle = it->get();

                        if (handle.done()) {
                            handle_finish(handle);
                        }

                        it = m_unhandled_finished_tasks.erase(it);
                    } else {
                        ++it;
                    }
                }

                std::coroutine_handle<> handle;

                // scope
                {
                    std::lock_guard lock(m_mutex);
                    if (m_queue.empty()) {
                        continue;
                    }

                    handle = m_queue.front();
                    m_queue.pop();

                    if (!m_queue_elements.contains(handle) || !handle) {
                        continue;
                    }

                    m_queue_elements.erase(handle);

                    if (depend_on.contains(handle)) {
                        m_queue.push(handle);
                        m_queue_elements.insert(handle);
                        continue;
                    }
                }

                if (m_can_be_finished_immediately.contains(handle)) {
                    m_can_be_finished_immediately.erase(handle);
                    assert(!depend_on.contains(handle));
                    handle_finish(handle);
                    continue;
                }

                try {
                    m_unhandled_finished_tasks.push_back(
                        m_thread_pool.submit([co_handle = handle,
                            this_executor = this] {
                            current_executor = this_executor;
                            try {
                                co_handle.resume();
                            } catch (const std::exception &e) {
                                ywl::utils::err_printf_ln("Exception was thrown in coroutine: {}",
                                                          e.what()).flush();
                            }
                            return co_handle;
                        }));
                } catch (const std::exception &e) {
                    std::rethrow_exception(std::current_exception());
                }
            }
        }

        constexpr void cancel_all_dependencies_of(std::coroutine_handle<> handle) {
            std::lock_guard lock(m_mutex);
            if (depend_on.contains(handle)) {
                for (const auto &other: depend_on.at(handle)) {
                    cancel_all_dependencies_of(other);
                    m_can_be_finished_immediately.insert(other);
                }
                m_can_be_finished_immediately.insert(handle);
            }
        }

        constexpr void handle_finish(std::coroutine_handle<> handle) {
            std::lock_guard lock(m_mutex);
            if (m_can_be_finished_immediately.contains(handle)) {
                m_can_be_finished_immediately.erase(handle);
                return;
            }

            if (!dependency_of.contains(handle)) {
                return;
            }

            auto issuer = dependency_of.at(handle);

            if (need_any_rather_than_all.contains(issuer)) {
                assert(issuer);
                depend_on.at(issuer).erase(handle);
                dependency_of.erase(handle);

                for (const auto &other: depend_on.at(issuer)) {
                    cancel_all_dependencies_of(other);
                    m_can_be_finished_immediately.insert(other);
                }

                if (depend_on.at(issuer).empty()) {
                    need_any_rather_than_all.erase(issuer);
                    m_queue.push(issuer);
                    m_queue_elements.insert(issuer);
                    depend_on.erase(issuer);
                }
            } else {
                depend_on.at(issuer).erase(handle);

                if (depend_on.at(issuer).empty()) {
                    depend_on.erase(issuer);
                    m_queue.push(issuer);
                    m_queue_elements.insert(issuer);
                }

                dependency_of.erase(handle);
            }
        }

        constexpr void schedule_dependency_any(std::coroutine_handle<> issuer, std::coroutine_handle<> task) override {
            std::lock_guard lock(m_mutex);
            need_any_rather_than_all.insert(issuer);

            m_queue_elements.erase(issuer);

            m_queue.push(task);
            m_queue_elements.insert(task);
            dependency_of[task] = issuer;
            depend_on[issuer].insert(task);
        }

        constexpr ~multithreaded_co_executor() override {
            if (!m_queue.empty()) {
                ywl::utils::err_printf_ln("Warning: not all coroutines were finished.").flush();
            }

            if (!depend_on.empty()) {
                ywl::utils::err_printf_ln("Warning: not all dependencies were finished.").flush();
            }

            if (!dependency_of.empty()) {
                ywl::utils::err_printf_ln("Warning: not all dependencies were finished.").flush();
                for (const auto &[task, issuer]: dependency_of) {
                    ywl::utils::err_printf_ln("Coroutine {} is waiting for {}.", issuer.address(), task.address()).
                            flush();
                }
            }

            if (!m_queue_elements.empty()) {
                ywl::utils::err_printf_ln("Warning: not all coroutines were finished.").flush();
                for (const auto &handle: m_queue_elements) {
                    ywl::utils::err_printf_ln("Coroutine {} is not finished.", handle.address()).flush();
                }
            }
        }
    };

    export template<typename T>
    class co_awaitable : public co_awaitable_base {
    public:
        using value_type = T;

        constexpr co_awaitable() = default;

        struct promise_type {
        private:
            std::optional<T> m_value;

        public:
            constexpr co_awaitable get_return_object() {
                return co_awaitable{
                    std::coroutine_handle<promise_type>::from_promise(*this)
                };
            }

            constexpr std::suspend_always initial_suspend() {
                return {};
            }

            constexpr std::suspend_always final_suspend() noexcept {
                return {};
            }

            constexpr void unhandled_exception() {
                std::rethrow_exception(std::current_exception());
            }

            constexpr void return_value(T value) {
                m_value = std::move(value);
            }

            constexpr T get_value() {
                if (!m_value.has_value()) {
                    throw ywl::basic::logic_error("value is not set");
                }
                return std::exchange(m_value, std::nullopt).value();
            }

            constexpr std::optional<T> get_value_optional() {
                return std::exchange(m_value, std::nullopt);
            }

            constexpr ~promise_type() = default;
        };

        constexpr explicit co_awaitable(std::coroutine_handle<promise_type> handle) : m_handle(handle) {
        }

    private:
        std::coroutine_handle<promise_type> m_handle;

    public:
        constexpr co_awaitable(const co_awaitable &) = delete;

        constexpr co_awaitable &operator=(const co_awaitable &) = delete;

        constexpr co_awaitable(co_awaitable &&other) noexcept: m_handle(std::exchange(other.m_handle, nullptr)) {
        }

        constexpr co_awaitable &operator=(co_awaitable &&other) noexcept {
            if (this != &other) {
                m_handle = std::exchange(other.m_handle, nullptr);
            }
            return *this;
        }

        [[nodiscard]] constexpr T get_value() {
            return m_handle.promise().get_value();
        }

        [[nodiscard]] constexpr std::optional<T> get_value_optional() {
            return m_handle.promise().get_value_optional();
        }

        [[nodiscard]] constexpr bool await_ready() const {
            return false;
        }

        constexpr void await_suspend(std::coroutine_handle<> issuer) {
            current_executor->schedule_dependency_all(issuer, m_handle);
        }

        constexpr T await_resume() {
            return get_value();
        }

        constexpr std::coroutine_handle<> get_handle() override {
            return m_handle;
        }

        constexpr ~co_awaitable() override {
            if (m_handle) {
                m_handle.destroy();
            }
        }
    };

    export class co_context {
    private:
        std::unique_ptr<co_executor_base> m_executor;

        constexpr explicit co_context(std::unique_ptr<co_executor_base> executor) : m_executor(std::move(executor)) {
        }

    public:
        template<std::derived_from<co_executor_base> T, typename... Args>
        constexpr static co_context from_executor(Args &&... args) {
            return co_context(std::make_unique<T>(std::forward<Args>(args)...));
        }

        constexpr co_executor_base *get_executor() {
            return m_executor.get();
        }

        constexpr void run() {
            m_executor->run();
        }

        template<typename T>
        constexpr T block_on(co_awaitable<T> &&co_awaitable) {
            m_executor->initial_schedule_task(co_awaitable.get_handle());
            m_executor->run();
            return co_awaitable.get_value();
        }
    };

    template<typename... Args>
    class wait_all_of_t {
    private:
        std::tuple<co_awaitable<Args>...> m_co_awaitables;

    public:
        constexpr explicit wait_all_of_t(co_awaitable<Args> &&... co_awaitables) : m_co_awaitables(
            std::move(co_awaitables)...) {
        }

        // define awaiter functions
        [[nodiscard]] constexpr bool await_ready() const {
            return false;
        }

        constexpr void await_suspend(std::coroutine_handle<> issuer) {
            std::apply([&issuer](auto &... co_awaitables) {
                ((current_executor->schedule_dependency_all(issuer, co_awaitables.get_handle())), ...);
            }, m_co_awaitables);
        }

        constexpr std::tuple<Args...> await_resume() {
            return std::apply([](auto &&... co_awaitables) {
                return std::make_tuple(co_awaitables.get_value()...);
            }, m_co_awaitables);
        }
    };

    export template<typename... Args>
    constexpr wait_all_of_t<Args...> wait_all_of(co_awaitable<Args> &&... co_awaitables) {
        return wait_all_of_t<Args...>(std::move(co_awaitables)...);
    }

    template<typename... Args>
    class wait_any_of_t {
    private:
        std::tuple<co_awaitable<Args>...> m_co_awaitables;

    public:
        constexpr explicit wait_any_of_t(co_awaitable<Args> &&... co_awaitables) : m_co_awaitables(
            std::move(co_awaitables)...) {
        }

        // define awaiter functions
        [[nodiscard]] constexpr bool await_ready() const {
            return false;
        }

        constexpr void await_suspend(std::coroutine_handle<> issuer) {
            std::apply([&issuer](auto &... co_awaitables) {
                ((current_executor->schedule_dependency_any(issuer, co_awaitables.get_handle())), ...);
            }, m_co_awaitables);
        }

        std::tuple<std::optional<Args>...>

        constexpr await_resume() {
            return std::apply([](auto &&... co_awaitables) {
                return std::make_tuple(co_awaitables.get_value_optional()...);
            }, m_co_awaitables);
        }
    };

    export template<typename... Args>
    constexpr wait_any_of_t<Args...> wait_any_of(co_awaitable<Args> &&... co_awaitables) {
        return wait_any_of_t<Args...>(std::move(co_awaitables)...);
    }
}
