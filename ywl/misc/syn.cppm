export module ywl.misc.syn;

import ywl.std.prelude;
import ywl.basic.string_literal;
import ywl.basic.exceptions;

namespace ywl::misc::syn {
    using const_iter_type = std::string_view::const_iterator;
    // using should_stop_type = std::function<bool(const_iter_type)>;

    struct should_stop_type {
    public:
        const_iter_type end_it;
        std::function<bool(const_iter_type)> fn;

        constexpr bool operator()(const_iter_type current_it) const {
            if (current_it >= end_it) {
                return true;
            }
            if (fn) {
                return fn(current_it);
            }
            return false;
        }

        should_stop_type(const_iter_type end_it, std::function<bool(const_iter_type)> fn)
            : end_it(end_it), fn(fn) {}
    };

    export template<typename T>
    concept is_token_type = requires(T, const_iter_type begin, const should_stop_type &fn) {
        typename T::result_type;
        { T::parse(begin, fn) } -> std::same_as<std::pair<const_iter_type, std::optional<typename T::result_type>>>;
    };

    template<typename T>
    using parse_result_type = std::pair<const_iter_type, std::optional<T>>;

    // is will return true if the token is found, and false otherwise, is will not consume the token
    // parse will return the token and consume it, if the token is not found, it will return the end iterator

    const_iter_type consume_white_space(const_iter_type &begin, const should_stop_type &fn) {
        // will consume white space, tab, new line, carriage return
        while (!fn(begin) && (*begin == ' ' || *begin == '\t' || *begin == '\n' || *begin == '\r')) {
            ++begin;
        }

        return begin;
    }

    export enum class error_type {
        integer_overflow,
        floating_point_overflow,
        floating_point_format_error,
        string_format_error,
        string_invalid_escape_error,
        subview_not_destructed,
        parenthesis_mismatch
    };

    export class token_view_stream {
        const_iter_type current_iterator;
        should_stop_type should_stop;
        std::function<void(token_view_stream &)> on_destruct;
        std::optional<error_type> error;

    public:
        explicit token_view_stream(std::string_view buffer) : current_iterator(buffer.cbegin()), should_stop{
                                                                  buffer.cend(),
                                                                  nullptr
                                                              }, on_destruct{}, error{} {
            current_iterator = consume_white_space(current_iterator, should_stop);
        }

        token_view_stream(const_iter_type begin, should_stop_type should_stop,
                          std::function<void(token_view_stream &)> on_destruct = {})
            : current_iterator(begin), should_stop(should_stop), on_destruct(on_destruct), error{} {
            current_iterator = consume_white_space(current_iterator, should_stop);
        }

        token_view_stream(const token_view_stream &) = delete;

        token_view_stream &operator=(const token_view_stream &) = delete;

        token_view_stream(token_view_stream &&other) noexcept
            : current_iterator(std::move(other.current_iterator)),
              should_stop(std::move(other.should_stop)), on_destruct(std::move(other.on_destruct)),
              error(std::move(other.error)) {
            other.discard();
        }

        token_view_stream &operator=(token_view_stream &&other) noexcept {
            auto cpy = std::move(*this);

            current_iterator = std::move(other.current_iterator);
            should_stop = std::move(other.should_stop);
            on_destruct = std::move(other.on_destruct);
            error = std::move(other.error);
            other.discard();
        }

        constexpr bool is_finished() const {
            return should_stop(current_iterator);
        }

        template<char c>
        bool parse_char_token() {
            if (is_finished()) {
                return false;
            }
            if (*current_iterator == c) {
                ++current_iterator;
                current_iterator = consume_white_space(current_iterator, should_stop);
                return true;
            }
            return false;
        }

        template<basic::string_literal str>
        bool parse_literal_token() {
            if (is_finished()) {
                return false;
            }
            const auto &str_data = str.data;
            auto str_length = str.length();
            auto str_end = str_data + str_length;
            auto current_end = current_iterator + str_length;
            if (should_stop(current_end)) {
                return false;
            }
            if (std::equal(str_data, str_end, current_iterator)) {
                current_iterator = current_end;
                current_iterator = consume_white_space(current_iterator, should_stop);
                return true;
            }
            return false;
        }

        template<is_token_type T>
        auto parse_by() -> std::optional<typename T::result_type> {
            if (is_finished()) {
                return {};
            }
            auto [result_begin, result] = T::parse(current_iterator, should_stop);
            current_iterator = consume_white_space(result_begin, should_stop);
            return result;
        }

        ~token_view_stream() noexcept(false) {
            if (!is_finished())
                throw basic::runtime_error(
                    "Token view stream not finished, this can be avoided by explicitly calling discard");

            if (on_destruct) {
                on_destruct(*this);
            }
        }

        void discard() {
            should_stop = {
                current_iterator,
                [](const_iter_type) { return true; }
            };

            error = std::nullopt;
        }

        std::optional<error_type> get_error() const {
            return error;
        }

        template<char front_paren, char back_paren>
        std::optional<token_view_stream> sub_stream() {
            if (is_finished()) {
                return {};
            }
            if (*current_iterator != front_paren) {
                return {};
            }
            ++current_iterator;

            this->error = error_type::subview_not_destructed;

            return {
                token_view_stream{
                    current_iterator,
                    should_stop_type{
                        should_stop.end_it,
                        [](const_iter_type it) {
                            return *it == back_paren;
                        }
                    },
                    [this](token_view_stream &sub_view) {
                        if (sub_view.error.has_value()) {
                            this->error = sub_view.error;
                            return;
                        }
                        if (this->error.has_value() && *this->error == error_type::subview_not_destructed) {
                            this->error = std::nullopt;
                        }

                        if (!sub_view.is_finished()) {
                            this->error = error_type::parenthesis_mismatch;
                            return;
                        }

                        if (*sub_view.current_iterator != back_paren) {
                            throw basic::runtime_error(
                                "Sub stream not finished, this can be avoided by explicitly calling discard");
                        }

                        ++sub_view.current_iterator;
                        sub_view.current_iterator =
                                consume_white_space(sub_view.current_iterator, sub_view.should_stop);

                        this->current_iterator = sub_view.current_iterator;
                    }
                }
            };
        }
    };

    inline namespace token_type {
        export class identifier {
        public:
            using result_type = std::string;

            constexpr static parse_result_type<std::string> parse(const_iter_type begin, const should_stop_type &fn) {
                auto curr = begin;

                if (fn(curr) || !std::isalpha(*curr) && *curr != '_') {
                    return {begin, std::nullopt};
                }
                auto original_begin = curr;
                ++curr;

                while (fn(curr) && (std::isalnum(*curr) || *curr == '_')) {
                    ++curr;
                }

                return {curr, std::string{original_begin, curr}};
            }
        };

        export template<std::integral T>
        class integer {
        public:
            using result_type = std::expected<T, error_type>;

            constexpr static parse_result_type<result_type> parse(const_iter_type begin, const should_stop_type &fn) {
                if (fn(begin)) {
                    return {begin, std::nullopt};
                }

                bool is_negative = false;

                if constexpr (std::is_signed_v<T>) {
                    if (*begin == '-') {
                        ++begin;
                        is_negative = true;
                    } else if (*begin == '+') {
                        ++begin;
                    }
                }

                T result = 0;

                while (!fn(begin) && std::isdigit(*begin)) {
                    // check if the result would overflow
                    if constexpr (std::is_signed_v<T>) {
                        if (result > (std::numeric_limits<T>::max() - (*begin - '0')) / 10) {
                            return {begin, std::unexpected{error_type::integer_overflow}};
                        }
                    } else {
                        if (result > (std::numeric_limits<T>::max() - (*begin - '0')) / 10) {
                            return {begin, std::unexpected{error_type::integer_overflow}};
                        }
                    }

                    result = result * 10 + (*begin - '0');
                    ++begin;
                }

                if (is_negative) {
                    result = -result;
                }

                return {begin, result};
            }
        };

        export template<std::floating_point T>
        class floating_point {
        public:
            using result_type = std::expected<T, error_type>;

            constexpr static parse_result_type<result_type> parse(const_iter_type begin, const should_stop_type &fn) {
                if (fn(begin)) {
                    return {begin, std::nullopt};
                }

                // float points definitely have a sign
                bool is_negative = false;
                int exponent = 0;
                enum class now_reading {
                    integer,
                    fraction,
                    exponent
                } reading = now_reading::integer;

                T result = 0;
                T fraction_part = 0;
                T fraction_divisor = 1;

                auto original_begin = begin;

                while (!fn(begin)) {
                    if (*begin == '-') {
                        if (reading == now_reading::integer) {
                            ++begin;
                            is_negative = true;
                        } else {
                            return {begin, std::unexpected{error_type::floating_point_format_error}};
                        }
                    } else if (*begin == '+') {
                        if (reading == now_reading::integer) {
                            ++begin;
                        } else {
                            return {begin, std::unexpected{error_type::floating_point_format_error}};
                        }
                    } else if (*begin == '.') {
                        if (reading == now_reading::integer) {
                            reading = now_reading::fraction;
                            ++begin;
                        } else {
                            return {begin, std::unexpected{error_type::floating_point_format_error}};
                        }
                    } else if (*begin == 'e' || *begin == 'E') {
                        if (reading == now_reading::integer || reading == now_reading::fraction) {
                            reading = now_reading::exponent;
                            ++begin;

                            begin = consume_white_space(begin, fn);
                            if (fn(begin)) {
                                return {begin, std::unexpected{error_type::floating_point_format_error}};
                            }
                            auto [res_it, res]
                                    = integer<int>::parse(begin, fn); // using int for exponent, as it is usually small
                            if (res.has_value()) {
                                exponent = res->value();
                                begin = res_it;
                            } else {
                                return {begin, std::unexpected{error_type::floating_point_format_error}};
                            }
                        } else {
                            return {begin, std::unexpected{error_type::floating_point_format_error}};
                        }
                    } else if (std::isdigit(*begin)) {
                        if (reading == now_reading::integer) {
                            result = result * 10 + (*begin - '0');
                        } else if (reading == now_reading::fraction) {
                            fraction_part = fraction_part * 10 + (*begin - '0');
                            fraction_divisor *= 10;
                        } else {
                            throw basic::ywl_impl_error(
                                "Logic error: floating point parser is in exponent state");
                        }
                        ++begin;
                    } else {
                        break;
                    }
                }

                if (begin == original_begin) {
                    return {begin, std::nullopt};
                }

                result += fraction_part / fraction_divisor;
                if (is_negative) {
                    result = -result;
                }
                result *= std::pow(10, exponent);
                return {begin, result};
            }
        };

        // string literal
        export class string {
        public:
            using result_type = std::expected<std::string, error_type>;

            static std::pair<const_iter_type, std::optional<char>> handle_escape_sequence(
                const_iter_type begin, const should_stop_type &fn) {
                if (fn(begin)) {
                    return {begin, std::nullopt};
                }

                char escape_char = *begin;
                ++begin;

                switch (escape_char) {
                    case 'n':
                        return {begin, '\n'};
                    case 't':
                        return {begin, '\t'};
                    case 'r':
                        return {begin, '\r'};
                    case '\\':
                        return {begin, '\\'};
                    case '"':
                        return {begin, '"'};
                    default:
                        return {begin, std::nullopt};
                }
            }

            constexpr static parse_result_type<result_type> parse(const_iter_type begin, const should_stop_type &fn) {
                if (fn(begin)) {
                    return {begin, std::nullopt};
                }

                if (*begin != '"') {
                    return {begin, std::nullopt};
                }
                ++begin;

                std::stringstream ss;

                auto original_begin = begin;
                while (!fn(begin) && *begin != '"') {
                    if (*begin == '\\') {
                        ++begin;
                        if (fn(begin)) {
                            return {original_begin, std::unexpected{error_type::string_format_error}};
                        }
                        auto pair = handle_escape_sequence(begin, fn);
                        if (pair.second.has_value()) {
                            ss << *pair.second;
                            begin = pair.first;
                        } else {
                            return {original_begin, std::unexpected{error_type::string_invalid_escape_error}};
                        }
                    } else {
                        ss << *begin;
                        ++begin;
                    }
                }

                if (fn(begin)) {
                    return {original_begin, std::unexpected{error_type::string_format_error}};
                }
                if (*begin != '"') {
                    return {original_begin, std::unexpected{error_type::string_format_error}};
                }
                ++begin;
                return {begin, ss.str()};
            }
        };

        export class boolean {
        public:
            using result_type = std::expected<bool, error_type>;

            constexpr static parse_result_type<result_type> parse(const_iter_type begin, const should_stop_type &fn) {
                if (fn(begin)) {
                    return {begin, std::nullopt};
                }

                auto original_begin = begin;

                while (fn(begin) && std::isalnum(*begin)) {
                    ++begin;
                }

                size_t length = begin - original_begin;
                if (length != 4 && length != 5) {
                    return {original_begin, std::unexpected{error_type::string_format_error}};
                }

                constexpr auto true_str = "true";
                constexpr auto false_str = "false";
                if (length == 4 && std::equal(original_begin, begin, true_str, true_str + 4)) {
                    return {begin, true};
                }
                if (length == 5 && std::equal(original_begin, begin, false_str, false_str + 5)) {
                    return {begin, false};
                }
                return {original_begin, std::unexpected{error_type::string_format_error}};
            }
        };
    }
}
