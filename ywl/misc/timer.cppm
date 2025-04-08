module;

#include <stdc++pch.hpp>

export module ywl.misc.timer;

import ywl.overloads.std_overloads;

// import ywl.std.prelude;

namespace ywl::misc {
    export template<typename clock_type = std::chrono::high_resolution_clock,
        typename duration_type = std::chrono::milliseconds>
    class timer {
    private:
        mutable std::chrono::time_point<clock_type> start_time{};
        mutable duration_type time_passed{};

    public:
        constexpr static inline bool ywl_define_formatter = true;

        constexpr timer(const timer &) = delete;

        constexpr timer &operator=(const timer &) = delete;

        constexpr timer(timer &&) = delete;

        constexpr timer &operator=(timer &&) = delete;

        constexpr timer() = default;

        constexpr void start() {
            start_time = clock_type::now();
        }

        constexpr const duration_type &get_time_passed() const {
            if (start_time == std::chrono::time_point<clock_type>()) {
                return this->time_passed;
            }

            this->time_passed += std::chrono::duration_cast<duration_type>(clock_type::now() - start_time);
            this->start_time = clock_type::now();

            return this->time_passed;
        }

        constexpr void suspend() {
            this->time_passed += std::chrono::duration_cast<duration_type>(clock_type::now() - start_time);
            this->start_time = std::chrono::time_point<clock_type>();
        }

        constexpr operator const duration_type &() /* NOLINT(google-explicit-constructor) */
        {
            return get_time_passed();
        }

        constexpr void stop() {
            this->time_passed = std::chrono::duration_values<duration_type>::zero();
            this->start_time = std::chrono::time_point<clock_type>();
        }

        constexpr void restart() {
            this->time_passed = std::chrono::duration_values<duration_type>::zero();
            this->start_time = clock_type::now();
        }

        constexpr duration_type stop_and_report() {
            auto result = get_time_passed();
            stop();
            return result;
        }

        [[nodiscard]] constexpr std::string to_string() const {
            return "{}"_fmt(this->get_time_passed());
        }
    };

    export template<typename clock_type = std::chrono::high_resolution_clock,
        typename duration_type = std::chrono::milliseconds>
    class scoped_timer : public timer<clock_type, duration_type> {
    public:
        constexpr scoped_timer() {
            this->start();
        }
    };
}
