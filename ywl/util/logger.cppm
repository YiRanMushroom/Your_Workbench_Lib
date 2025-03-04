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
        logger() = delete;

        logger(const logger &) = delete;

        logger(logger &&) = delete;

        logger &operator=(const logger &) = delete;

        logger &operator=(logger &&) = delete;

        logger(std::ostream *osp, std::string source, std::string level, bool is_enabled) : osp{osp},
            source{std::move(source)}, level{std::move(level)}, is_enabled{is_enabled} {}

        logger &set_enabled(this logger &self, bool enabled) {
            self.is_enabled = enabled;
            return self;
        }

        logger &flush(this logger &self) {
            self.osp->flush();
            return self;
        }

        const logger &flush(this const logger &self) {
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
        std::ostream *osp;
        std::string source;
        std::string level;

    public:
        logger_builder() = default;

        decltype(auto) set_os(this auto &&self, std::ostream &os) {
            self.osp = &os;
            return self;
        }

        decltype(auto) set_source(this auto &&self, std::string source) {
            self.source = std::move(source);
            return self;
        }

        decltype(auto) set_level(this auto &&self, std::string level) {
            self.level = std::move(level);
            return self;
        }

        logger build(this auto &&self) {
            return logger{
                self.osp, std::forward<decltype(self.source)>(self.source),
                std::forward<decltype(self.level)>(self.level), true
            };
        }
    };
}
