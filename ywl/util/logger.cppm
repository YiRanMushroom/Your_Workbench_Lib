module;

#include <iostream>
#include <format>

export module ywl.util.logger;

namespace ywl::util {
    export class logger_info_t {
    public:
        constexpr static const char *source() {
            return "LOGGER";
        }

        constexpr static const char *level() {
            return "INFO";
        }

        constexpr static bool use_std_err() {
            return false;
        }

        constexpr static bool is_always_disabled() {
            return false;
        }
    };

    export inline logger_info_t default_logger_info{};

    template<typename T>
    concept is_logger_info = std::is_base_of_v<logger_info_t, T> || requires(T) {
        { T::source() } -> std::convertible_to<const char *>;
        { T::level() } -> std::convertible_to<const char *>;
        { T::use_std_err() } -> std::convertible_to<bool>;
        { T::is_always_disabled() } -> std::convertible_to<bool>;
    };

    export template<is_logger_info Info>
    class logger_t {
    public:
        logger_t() = default;

    private:
        static inline bool is_enabled{true};

        static inline bool do_flush{true};

    public:
        constexpr static void enable() {
            is_enabled = true;
        }

        constexpr static void disable() {
            is_enabled = false;
        }

        constexpr static void enable_flush() {
            do_flush = true;
        }

        constexpr static void disable_flush() {
            do_flush = false;
        }

        template<typename... Tps>
        constexpr void operator()(std::format_string<const Tps &...> fmt, const Tps &... args) const {
            if (is_enabled) {
                if constexpr (Info::use_std_err()) {
                    std::cerr << std::format("[{}: {}] ", Info::source(), Info::level())
                            << std::format(fmt, args...) << '\n';
                } else {
                    std::cout << std::format("[{}: {}] ", Info::source(), Info::level())
                            << std::format(fmt, args...) << '\n';
                    if (do_flush)
                        std::cout.flush();
                }
            }
        }

        template<typename... Tps>
        constexpr void log_fmt_multiple(std::format_string<const Tps &...> fmt, const Tps &... args) const {
            if (is_enabled) {
                if constexpr (Info::use_std_err()) {
                    std::cerr << std::format("===========[{}: {}]===========\n", Info::source(), Info::level())
                            << std::format(fmt, args...) << '\n';
                    std::cerr << std::format(fmt, args...) << '\n';
                    std::cerr << std::format("===========[ E N D ]===========\n", Info::source(), Info::level())
                            << std::format(fmt, args...) << '\n';
                } else {
                    std::cout << std::format("===========[{}: {}]===========\n", Info::source(), Info::level())
                            << std::format(fmt, args...) << '\n';
                    std::cout << std::format(fmt, args...) << '\n';
                    std::cout << std::format("===========[ E N D ]===========\n", Info::source(), Info::level())
                            << std::format(fmt, args...) << '\n';
                    if (do_flush)
                        std::cout.flush();
                }
            }
        }

        template<typename... Ts>
        constexpr void operator[](const Ts &... args) const {
            if (is_enabled) {
                if constexpr (Info::use_std_err()) {
                    std::cerr << std::format("[{}: {}] ", Info::source(), Info::level());
                    ((std::cerr << std::format("{}", args) << ' '), ...);
                    std::cerr << '\n';
                } else {
                    std::cout << std::format("[{}: {}] ", Info::source(), Info::level());
                    ((std::cout << std::format("{}", args) << ' '), ...);
                    std::cout << '\n';
                    if (do_flush)
                        std::cout.flush();
                }
            }
        }

        template<typename... Tps>
        constexpr void log_fmt(std::format_string<const Tps &...> fmt, const Tps &... args) const {
            return logger_t<Info>::operator()(fmt, args...);
        }

        template<typename... Ts>
        constexpr void log(const Ts &... args) const {
            return logger_t<Info>::operator[](args...);
        }
    };

    class logger_dispatcher_t {
    public:
        logger_dispatcher_t() = default;

        template<is_logger_info Info>
        decltype(auto) operator[](Info) {
            if constexpr (Info::is_always_disabled()) {
                return [](const auto &&...) {};
            } else {
                return logger_t<Info>{};
            }
        }
    };

    export inline logger_dispatcher_t logger{};

    export inline auto default_logger = logger[default_logger_info];

    class default_logger_info_error_t : public logger_info_t {
    public:
        constexpr static const char *level() {
            return "ERROR";
        }

        constexpr static bool use_std_err() {
            return true;
        }
    };

    export inline auto default_error_logger = logger[default_logger_info_error_t{}];
}
