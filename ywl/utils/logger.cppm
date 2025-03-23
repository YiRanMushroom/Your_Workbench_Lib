export module ywl.utils.logger;

import ywl.basic.string_literal;
import ywl.std.prelude;

namespace ywl::utils {
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
                                                                                                      source{std::move(source)}, level{std::move(level)}, is_enabled{is_enabled} {
        }

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

    inline struct flusher_t {
        std::ostream& os;

        flusher_t(const flusher_t&) = delete;

        flusher_t(std::ostream& os) : os{os} {}

        constexpr void flush() const {
            os.flush();
        }
    } cout_flusher{std::cout}, cerr_flusher{std::cerr};

    export template<typename... Tps>
    constexpr void printf(std::format_string<const Tps &...> fmt, const Tps &... args) {
        std::cout << std::format(fmt, args...);
    }

    export template<typename... Tps>
    constexpr const flusher_t& printf_ln(std::format_string<const Tps &...> fmt, const Tps &... args) {
        std::cout << std::format(fmt, args...) << '\n';
        return cout_flusher;
    }

    export template<ywl::basic::string_literal sep = " ", typename... Tps>
    constexpr void print(const Tps &... args) {
        const std::tuple refs = std::forward_as_tuple(args...);
        [&]<size_t ... Is>(std::index_sequence<Is...>) {
            ((std::cout <<
                        std::format("{}", std::get<Is>(refs)),
                    (Is != sizeof...(Tps) - 1
                     ? (void) (std::cout << sep.data)
                     : (void) nullptr)), ...);
        }(std::make_index_sequence<sizeof...(Tps)>{});
    }

    export template<ywl::basic::string_literal sep = " ", typename... Tps>
    constexpr const flusher_t& print_ln(const Tps &... args) {
        print<sep>(args...);
        std::cout << '\n';
        return cout_flusher;
    }

    export template<typename... Tps>
    constexpr void err_printf(std::format_string<const Tps &...> fmt, const Tps &... args) {
        std::cerr << std::format(fmt, args...);
    }

    export template<typename... Tps>
    constexpr const flusher_t& err_printf_ln(std::format_string<const Tps &...> fmt, const Tps &... args) {
        std::cerr << std::format(fmt, args...) << '\n';
        return cerr_flusher;
    }

    export template<ywl::basic::string_literal sep = " ", typename... Tps>
    constexpr void err_print(const Tps &... args) {
        const std::tuple refs = std::forward_as_tuple(args...);
        [&]<size_t ... Is>(std::index_sequence<Is...>) {
            ((std::cerr <<
                        std::format("{}", std::get<Is>(refs)),
                    (Is != sizeof...(Tps) - 1
                     ? (void) (std::cerr << sep.data)
                     : (void) nullptr)), ...);
        }(std::make_index_sequence<sizeof...(Tps)>{});
    }

    export template<ywl::basic::string_literal sep = " ", typename... Tps>
    constexpr const flusher_t& err_print_ln(const Tps &... args) {
        err_print<sep>(args...);
        std::cerr << '\n';
        return cerr_flusher;
    }
}
