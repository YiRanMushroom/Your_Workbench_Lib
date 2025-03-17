export module ywl.miscellaneous.multithreading.thread_safe_queue;

import ywl.std.prelude;
import ywl.basic.exceptions;

namespace ywl::miscellaneous::multi_threading {
    export template<typename Queue_Type>
    concept is_queue = requires(Queue_Type queue) {
        typename Queue_Type::value_type;

        queue.emplace(std::declval<typename Queue_Type::value_type>()); // it is possible to be non-void
        { queue.pop() } -> std::same_as<void>;
        { queue.size() } -> std::same_as<size_t>;
        { queue.empty() } -> std::same_as<bool>;

        [] -> bool {
            if constexpr (requires {{ queue.front() } -> std::same_as<typename Queue_Type::value_type>; }) {
                return true;
            } else if constexpr (requires {{ queue.top() } -> std::same_as<typename Queue_Type::value_type>; }) {
                return true;
            }
            return false;
        }() == true;
    };

    export template<is_queue Queue_Type>
// this should support queue and priority_queue
    class thread_safe_queue {
    public:
        using queue_type = Queue_Type;
        using value_type = typename queue_type::value_type;

    private:
        queue_type m_queue{};
        std::mutex m_mutex{};

    public:
        thread_safe_queue() = default;

        thread_safe_queue(const thread_safe_queue &) = delete;

        thread_safe_queue(thread_safe_queue &&) = delete;

        thread_safe_queue &operator=(const thread_safe_queue &) = delete;

        thread_safe_queue &operator=(thread_safe_queue &&) = delete;

        void emplace(auto &&... args) {
            std::scoped_lock lock{m_mutex};
            m_queue.emplace(std::forward<decltype(args)>(args)...);
        }

        std::optional <value_type> pop() {
            std::scoped_lock lock{m_mutex};
            if (m_queue.empty()) {
                return std::nullopt;
            }

            value_type value = [&] -> value_type {
                if constexpr (requires { m_queue.top(); }) {
                    return std::move(m_queue.top());
                } else if constexpr (requires { m_queue.front(); }) {
                    return std::move(m_queue.front());
                } else {
                    throw ywl::basic::ywl_impl_error{
                            "Implementation error in ywl::miscellaneous::multithreading::thread_safe_queue, "
                            "this should never reach."};
                }
            }();

            m_queue.pop();

            return value;
        }

        void push(value_type value) {
            std::scoped_lock lock{m_mutex};
            m_queue.emplace(std::move(value));
        }

        [[nodiscard]] bool empty_approx() const {
            std::scoped_lock lock{m_mutex};
            return m_queue.empty();
        }

        [[nodiscard]] size_t size_approx() const {
            std::scoped_lock lock{m_mutex};
            return m_queue.size();
        }
    };
}