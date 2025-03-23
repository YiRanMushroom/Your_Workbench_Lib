export module ywl.misc.multithreading.channel;

import ywl.std.prelude;
import ywl.misc.multithreading.thread_safe_queue;
import ywl.basic.exceptions;

namespace ywl::misc::multithreading {
    export class channel_closed_exception : public basic::ywl_exception_base {
    public:
        channel_closed_exception() noexcept = delete;

        channel_closed_exception(std::string message, std::source_location location = std::source_location::current()
                // , std::stacktrace stacktrace = std::stacktrace::current()
        )
        noexcept: ywl_exception_base{std::move(message), std::move(location)
                // , std::move(stacktrace)
        } {}

        channel_closed_exception(const channel_closed_exception &) = default;

        channel_closed_exception(channel_closed_exception &&) noexcept = default;

        channel_closed_exception &operator=(const channel_closed_exception &) = default;

        channel_closed_exception &operator=(channel_closed_exception &&) noexcept = default;

        [[nodiscard]] const char *exception_reminder() const noexcept override {
            return "channel_closed_exception";
        }
    };

    export template<class TSQueue>
    class mpmc_sender {
    public:
        using value_type = typename TSQueue::value_type;
        using queue_type = TSQueue;

        constexpr mpmc_sender(std::weak_ptr <queue_type> queue, std::weak_ptr <std::condition_variable_any> cv)
                : m_queue{std::move(queue)}, m_cv{std::move(cv)} {
        }

        constexpr void send(auto &&... args) const {
            auto queue = m_queue.lock();
            auto cv = m_cv.lock();

            if (queue && cv) {
                queue->emplace(std::forward<decltype(args)>(args)...);
                cv->notify_one();
            } else {
                throw channel_closed_exception{"Channel is closed"};
            }
        }

        constexpr bool is_closed_approx() const {
            return m_queue.expired();
        }

    private:
        std::weak_ptr <queue_type> m_queue;
        std::weak_ptr <std::condition_variable_any> m_cv;
    };

    export template<typename TSQueue>
    class mpmc_receiver {
    public:
        using value_type = typename TSQueue::value_type;
        using queue_type = TSQueue;

        constexpr mpmc_receiver(std::shared_ptr <queue_type> queue, std::shared_ptr <std::condition_variable_any> cv)
                : m_queue{std::move(queue)}, m_cv{std::move(cv)} {
        }

    private:
        std::shared_ptr <queue_type> m_queue;
        std::shared_ptr <std::condition_variable_any> m_cv;

    public:
        [[nodiscard]] constexpr std::optional <value_type> receive_weak() const {
            return m_queue->pop();
        }

        [[nodiscard]] constexpr std::optional <value_type> receive_strong() const {
            std::unique_lock lock{m_queue->get_mutex()};
            m_cv->wait_for(lock, std::chrono::milliseconds{100}, [this] { return !m_queue->empty_approx(); });
            return m_queue->pop();
        }

        constexpr mpmc_sender<queue_type> subscribe() {
            return mpmc_sender{std::weak_ptr{m_queue}, std::weak_ptr{m_cv}};
        }
    };

    export template<typename T>
    constexpr auto make_simple_mpmc_channel() {
        auto queue = std::make_shared < thread_safe_queue < std::queue < T>>>();
        auto cv = std::make_shared<std::condition_variable_any>();
        return std::make_pair(mpmc_sender{std::weak_ptr{queue}, std::weak_ptr{cv}}, mpmc_receiver{queue, cv});
    }

    export template<typename TSQueue>
    constexpr auto make_mpmc_channel() {
        auto queue = std::make_shared<TSQueue>();
        auto cv = std::make_shared<std::condition_variable_any>();
        return std::make_pair(mpmc_sender{std::weak_ptr{queue}, std::weak_ptr{cv}}, mpmc_receiver{queue, cv});
    }
}