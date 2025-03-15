module;
#include <utility>
#include <tuple>

#include <vector>
#include <string>
#include <array>
#include <memory>
#include <functional>
#include <list>

#include <queue>
#include <stack>
#include <deque>

#include <algorithm>

#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>

#include <type_traits>

#include <optional>
#include <variant>
#include <any>
#include <exception>

#include <string_view>

#include <expected>

#include <iterator>

#include <stacktrace>

#include <source_location>

#include <concepts>

#include <coroutine>

#include <format>

#include <limits>

#include <random>

#include <mutex>
#include <shared_mutex>
#include <condition_variable>

#include <thread>
#include <future>
#include <atomic>

export module ywl.std.prelude;

export namespace std {
    using std::move;
    using std::forward;
    using std::pair;
    using std::make_pair;
    using std::tuple;
    using std::make_tuple;
    using std::get;
    using std::forward_as_tuple;
    using std::tie;

    using std::swap;
    using std::exchange;
    using std::vector;
    using std::string;
    using std::to_string;
    using std::array;

    using std::unique_ptr;
    using std::make_unique;
    using std::shared_ptr;
    using std::make_shared;
    using std::weak_ptr;

    using std::function;
    using std::move_only_function;
    using std::bind;

    using std::list;

    using std::deque;
    using std::queue;
    using std::priority_queue;
    using std::stack;

    using std::sort;
    using std::min;
    using std::max;
    using std::find;

    using std::hash;
    using std::unordered_map;
    using std::unordered_multimap;
    using std::unordered_set;
    using std::unordered_multiset;
    using std::map;
    using std::multimap;
    using std::set;
    using std::multiset;

    using std::begin;
    using std::end;
    using std::rbegin;
    using std::rend;

    using std::copy;
    using std::copy_n;

    using std::enable_if_t;
    using std::is_same_v;
    using std::is_base_of_v;
    using std::is_convertible_v;
    using std::is_integral_v;
    using std::is_trivial_v;

    using std::is_default_constructible_v;
    using std::is_move_constructible_v;
    using std::is_copy_constructible_v;
    using std::is_copy_assignable_v;
    using std::is_move_assignable_v;
    using std::is_trivially_copyable_v;
    using std::is_trivially_copy_constructible_v;
    using std::is_trivially_copy_assignable_v;
    using std::is_trivially_move_constructible_v;
    using std::is_trivially_move_assignable_v;
    using std::is_trivially_destructible_v;
    using std::is_nothrow_copy_constructible_v;
    using std::is_nothrow_copy_assignable_v;
    using std::is_nothrow_move_constructible_v;

    using std::declval;

    using std::same_as;
    using std::convertible_to;
    using std::derived_from;
    using std::common_reference_with;
    using std::common_with;
    using std::integral;
    using std::signed_integral;
    using std::unsigned_integral;

    using std::invoke_result_t;
    using std::is_invocable_v;
    using std::index_sequence;
    using std::make_index_sequence;
    using std::index_sequence_for;

    using std::optional;
    using std::nullopt;
    using std::nullopt_t;
    using std::make_optional;

    using std::variant;
    using std::visit;

    using std::any;
    using std::any_cast;

    using std::exception;
    using std::runtime_error;
    using std::logic_error;
    using std::current_exception;
    using std::rethrow_exception;
    using std::exception_ptr;

    using std::string_view;

    using std::expected;
    using std::unexpected;

    using std::iterator_traits;
    using std::input_iterator_tag;
    using std::output_iterator_tag;
    using std::forward_iterator_tag;
    using std::bidirectional_iterator_tag;
    using std::random_access_iterator_tag;
    using std::input_iterator;
    using std::output_iterator;
    using std::forward_iterator;
    using std::bidirectional_iterator;
    using std::random_access_iterator;

    using std::iter_swap;
    using std::advance;
    using std::distance;
    using std::next;
    using std::prev;

    using std::stacktrace;

    using std::source_location;

    using std::integral;
    using std::signed_integral;
    using std::unsigned_integral;

    using std::movable;
    using std::copyable;

    using std::coroutine_handle;
    using std::coroutine_traits;
    using std::suspend_always;
    using std::suspend_never;
    using std::noop_coroutine;

    using std::format;
    using std::format_to;
    using std::format_to_n;
    using std::formatted_size;
    using std::format_error;
    using std::format_parse_context;
    using std::format_context;
    using std::format_args;
    using std::vformat;

    using std::formatter;
    using std::format_string;

    using std::ifstream;
    using std::ofstream;
    using std::fstream;
    using std::ios_base;
    using std::ios;
    using std::streambuf;
    using std::streampos;
    using std::streamoff;
    using std::streamsize;

    using size_t = std::size_t;

    using std::numeric_limits;

    using std::mt19937;
    using std::uniform_int_distribution;
    using std::uniform_real_distribution;
    using std::normal_distribution;
    using std::bernoulli_distribution;

    using std::random_device;
    using std::default_random_engine;
    using std::seed_seq;

    using std::mutex;
    using std::recursive_mutex;
    using std::shared_mutex;
    using std::unique_lock;
    using std::lock_guard;
    using std::scoped_lock;
    using std::condition_variable;
    using std::condition_variable_any;
    using std::notify_all_at_thread_exit;

    using std::thread;
    using std::jthread;
    namespace this_thread {
        using std::this_thread::get_id;
        using std::this_thread::yield;
        using std::this_thread::sleep_for;
        using std::this_thread::sleep_until;
    }

    using std::future;
    using std::promise;
    using std::packaged_task;
    using std::async;
    using std::launch;
    using std::future_status;
    using std::future_errc;
    using std::future_error;
    using std::future_category;

    using std::atomic;
    using std::atomic_flag;

    using std::memory_order;
    using std::memory_order_relaxed;
    using std::memory_order_acquire;
    using std::memory_order_release;
    using std::memory_order_acq_rel;
    using std::memory_order_seq_cst;

    namespace chrono {
        using std::chrono::duration;
        using std::chrono::duration_cast;
        using std::chrono::time_point;
        using std::chrono::system_clock;
        using std::chrono::steady_clock;
        using std::chrono::high_resolution_clock;
        using std::chrono::time_point_cast;
        using std::chrono::time_point;
        using std::chrono::duration_values;

        using std::chrono::nanoseconds;
        using std::chrono::microseconds;
        using std::chrono::milliseconds;
        using std::chrono::seconds;
        using std::chrono::minutes;
        using std::chrono::hours;
    }

    using std::stringstream;
}

export
{
    using size_t = std::size_t;
    using uint8_t = std::uint8_t;
    using uint16_t = std::uint16_t;
    using uint32_t = std::uint32_t;
    using uint64_t = std::uint64_t;
    using int8_t = std::int8_t;
    using int16_t = std::int16_t;
    using int32_t = std::int32_t;
    using int64_t = std::int64_t;

    using nullptr_t = std::nullptr_t;
}