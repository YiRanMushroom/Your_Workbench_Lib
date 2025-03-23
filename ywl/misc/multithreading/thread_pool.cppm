export module ywl.misc.multithreading.thread_pool;

import ywl.std.prelude;
import ywl.basic.move_only_function;
import ywl.utils.enum_entry;
import ywl.misc.multithreading.channel;
import ywl.misc.multithreading.thread_safe_queue;

using namespace ywl::utils::enum_entry;

namespace ywl::misc::multithreading {
    export class thread_pool {
    private:
        enum class task_enum_type {
            task,
            stop
        };

        using task_type = std::variant<enum_entry<task_enum_type, task_enum_type::task,
                ywl::basic::move_only_function<void()>
        >,
                enum_entry<task_enum_type, task_enum_type::stop, void>>;

        using channel_sender_type = mpmc_sender <thread_safe_queue<std::queue < task_type>>>;
        using channel_receiver_type = mpmc_receiver <thread_safe_queue<std::queue < task_type>>>;

    private:
        std::vector <std::thread> m_threads;
        channel_sender_type m_sender;
        channel_receiver_type m_receiver;

        constexpr thread_pool(std::vector <std::thread> threads, channel_sender_type sender,
                              channel_receiver_type receiver)
                : m_threads(std::move(threads)), m_sender(std::move(sender)), m_receiver(std::move(receiver)) {}

    public:
        constexpr thread_pool() = delete;

        constexpr thread_pool(thread_pool const &) = delete;

        constexpr thread_pool(thread_pool &&) = default;

        constexpr thread_pool &operator=(thread_pool const &) = delete;

        constexpr thread_pool &operator=(thread_pool &&other) noexcept {
            if (this != &other) {
                stop();
                m_threads = std::move(other.m_threads);
                m_sender = std::move(other.m_sender);
                m_receiver = std::move(other.m_receiver);
            }
            return *this;
        }

        constexpr static thread_pool create(size_t thread_count) {
            auto [sender, receiver] = make_simple_mpmc_channel<task_type>();
            std::vector <std::thread> threads;
            threads.reserve(thread_count);

            for (size_t i = 0; i < thread_count; ++i) {
                threads.emplace_back([recv = receiver]() mutable {
                    while (true) {
                        auto task = recv.receive_strong();
                        if (!task) {
                            continue;
                        }

                        if (*task | holds_enum<task_enum_type::stop>) {
                            break;
                        }

                        auto unwrapped = *task | get_if_move<task_enum_type::task>;
                        unwrapped();
                    }
                });
            }

            return {std::move(threads), std::move(sender), std::move(receiver)};
        }

        [[nodiscard]] constexpr auto submit(std::invocable auto &&task) const {
            // return a future
            using result_type = std::invoke_result_t <std::remove_cvref_t<decltype(task)>>;
            std::promise <result_type> promise{};
            auto future = promise.get_future();
            ywl::basic::move_only_function<void()> wrapped_task = [task = std::forward<decltype(task)>(task),
                    ps = std::move(promise)]() mutable {
                try {
                    if constexpr (std::is_same_v < result_type, void >) {
                        task();
                        ps.set_value();
                    } else {
                        ps.set_value(task());
                    }
                } catch (...) {
                    ps.set_exception(std::current_exception());
                }
            };

            m_sender.send(emplace<task_type, task_enum_type::task>(std::move(wrapped_task)));
            return future;
        }

        constexpr void submit_detached(std::invocable auto &&task) const {
            ywl::basic::move_only_function<void()> wrapped_task = [task = std::forward<decltype(task)>(
                    task)]() mutable {
                task();
            };

            m_sender.send(emplace<task_type, task_enum_type::task>(std::move(wrapped_task)));
        }

        constexpr void stop() {
            for (size_t i = 0; i < m_threads.size(); ++i) {
                m_sender.send(emplace<task_type, task_enum_type::stop>());
            }
            for (auto &thread: m_threads) {
                thread.join();
            }
            m_threads.clear();
        }

        constexpr ~thread_pool() {
            stop();
        }

        constexpr explicit thread_pool(size_t thread_count) : thread_pool{create(thread_count)} {}
    };
}
