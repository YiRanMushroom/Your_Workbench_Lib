export module ywl.miscellaneous.multithreading.channel;

import ywl.std.prelude;
import ywl.miscellaneous.multithreading.thread_safe_queue;
import ywl.basic.exceptions;

namespace ywl::miscellaneous::multi_threading {
    export class channel_closed_exception : public basic::ywl_exception_base {
    public:
        channel_closed_exception() noexcept = delete;

        channel_closed_exception(std::string message, std::source_location location = std::source_location::current(),
                                 std::stacktrace stacktrace = std::stacktrace::current())
        noexcept: ywl_exception_base{std::move(message), std::move(location), std::move(stacktrace)} {}

        channel_closed_exception(const channel_closed_exception &) = default;

        channel_closed_exception(channel_closed_exception &&) noexcept = default;

        channel_closed_exception &operator=(const channel_closed_exception &) = default;

        channel_closed_exception &operator=(channel_closed_exception &&) noexcept = default;

        [[nodiscard]] const char *exception_reminder() const noexcept override {
            return "channel_closed_exception";
        }
    };

    template<class TSQueue>
    class mpmc_sender {
    public:
        using value_type = typename TSQueue::value_type;
        using queue_type = TSQueue;

        mpmc_sender(std::weak_ptr <queue_type> queue) : m_queue{std::move(queue)} {

        }

        void send(auto &&... args) const {
            if (auto queue = m_queue.lock()) {
                queue->emplace(std::forward<decltype(args)>(args)...);
            } else {
                throw channel_closed_exception{"Channel is closed"};
            }
        }

        bool is_closed_approx() const {
            return m_queue.expired();
        }

    private:
        std::weak_ptr <queue_type> m_queue;
    };

    export template<typename TSQueue>
    class mpmc_receiver {
    public:
        using value_type = typename TSQueue::value_type;
        using queue_type = TSQueue;

        mpmc_receiver(std::shared_ptr <queue_type> queue, std::shared_ptr <std::condition_variable> cv)
                : m_queue{std::move(queue)}, m_cv{std::move(cv)} {
        }

    private:
        std::shared_ptr <queue_type> m_queue;
        std::shared_ptr <std::condition_variable> m_cv;

    public:
        std::optional <value_type> receive_weak() const {
            return m_queue->pop();
        }

        std::optional <value_type> receive_strong() const {
            std::unique_lock lock{m_queue->get_mutex()};
            m_cv->wait(lock, [&] { return !m_queue->empty(); });
            lock.unlock(); // still can return nullopt, I don't want to use recursive mutex so I don't know how to fix it
            return m_queue->pop();
        }

        mpmc_sender<queue_type> subscribe() {
            return mpmc_sender{m_queue};
        }
    };

    export template<typename T>
    auto make_simple_mpmc_channel() {
        auto queue = std::make_shared < thread_safe_queue < std::queue < T>>>();
        auto cv = std::make_shared<std::condition_variable>();
        return std::make_pair(mpmc_sender{std::weak_ptr{queue}}, mpmc_receiver{queue, cv});
    }

    export template<typename TSQueue>
    auto make_mpmc_channel() {
        auto queue = std::make_shared<TSQueue>();
        auto cv = std::make_shared<std::condition_variable>();
        return std::make_pair(mpmc_sender{std::weak_ptr{queue}}, mpmc_receiver{queue, cv});
    }
}