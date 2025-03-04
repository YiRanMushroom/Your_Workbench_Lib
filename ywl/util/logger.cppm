module;

#include <iostream>
#include <format>

export module ywl.util.logger;

namespace ywl::util {
    export class logger {
    private:
        std::ostream *osp;
        std::string source;
        std::string level;
        bool is_enabled;

    public:
        constexpr logger() = delete;

        constexpr logger(const logger &) = delete;

        constexpr logger(logger &&) = delete;

        constexpr logger &operator=(const logger &) = delete;

        constexpr logger &operator=(logger &&) = delete;

        constexpr logger(std::ostream *osp, std::string source, std::string level, bool is_enabled) : osp{osp},
            source{std::move(source)}, level{std::move(level)}, is_enabled{is_enabled} {}

        constexpr logger &set_enabled(this logger &self, bool enabled) {
            self.is_enabled = enabled;
            return self;
        }

        constexpr logger &flush(this logger &self) {
            self.osp->flush();
            return self;
        }

        constexpr const logger &flush(this const logger &self) {
            self.osp->flush();
            return self;
        }


        template<typename... Tps>
        constexpr decltype(auto) log_fmt(this auto &&self, std::format_string<const Tps &...> fmt,
                                         const Tps &... args) {
            if (self.is_enabled) {
                *(self.osp) << std::format("[{}: {}] ", self.source, self.level) << std::format(fmt, args...) << '\n';
            }

            return self;
        }

        template<typename... Tps>
        constexpr decltype(auto) operator()(this auto &&self, std::format_string<const Tps &...> fmt,
                                            const Tps &... args) {
            return self.log_fmt(fmt, args...);
        }

        template<typename... Tps>
        constexpr decltype(auto) log_no_fmt(this auto &&self, const Tps &... args) {
            if (self.is_enabled) {
                *(self.osp) << std::format("[{}: {}] ", self.source, self.level);
                ((*(self.osp) << std::format("{}", args) << ' '), ...);
                *(self.osp) << '\n';
            }

            return self;
        }

        template<typename... Tps>
        constexpr decltype(auto) operator[](this auto &&self, const Tps &... args) {
            return self.log_no_fmt(args...);
        }
    };

    export class logger_builder {
        std::ostream *osp{&std::cout};
        std::string source{"main"};
        std::string level{"INFO"};

    public:
        constexpr logger_builder() = default;

        constexpr decltype(auto) set_os(this auto &&self, std::ostream &os) {
            self.osp = &os;
            return self;
        }

        constexpr decltype(auto) set_source(this auto &&self, std::string source) {
            self.source = std::move(source);
            return self;
        }

        constexpr decltype(auto) set_level(this auto &&self, std::string level) {
            self.level = std::move(level);
            return self;
        }

        constexpr logger build(this auto &&self) {
            return logger{
                self.osp, std::forward<decltype(self.source)>(self.source),
                std::forward<decltype(self.level)>(self.level), true
            };
        }
    };

    export const inline auto default_logger = logger_builder{}.build();
    export const inline auto default_error_logger = logger_builder{}.set_os(std::cerr).set_level("ERROR").build();
}
