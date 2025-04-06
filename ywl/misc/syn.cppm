export module ywl.misc.syn;

import ywl.std.prelude;
import ywl.basic.string_literal;
import ywl.basic.exceptions;

namespace ywl::misc::syn {
    using const_iter_type = std::string_view::const_iterator;

    export template<typename T>
    concept is_token_type = requires(T, const_iter_type begin, const_iter_type end) {
        typename T::result_type;
        { T::parse(begin, end) } -> std::same_as<std::pair<const_iter_type, std::optional<typename T::result_type>>>;
    };

    template<typename T>
    using parse_result_type = std::pair<const_iter_type, std::optional<T>>;

    // is will return true if the token is found, and false otherwise, is will not consume the token
    // parse will return the token and consume it, if the token is not found, it will return the end iterator

    const_iter_type consume_white_space(const_iter_type &begin, const_iter_type end) {
        // will consume white space, tab, new line, carriage return
        while (begin != end && (*begin == ' ' || *begin == '\t' || *begin == '\n' || *begin == '\r')) {
            ++begin;
        }

        return begin;
    }

    export enum class error_type {
        integer_overflow,
        floating_point_overflow,
        floating_point_format_error,
        string_format_error,
        string_invalid_escape_error
    };

    export class token_view_stream {
        const_iter_type current_iterator;
        const_iter_type end;

    public:
        explicit token_view_stream(std::string_view buffer) : current_iterator(buffer.cbegin()), end(buffer.cend()) {
            current_iterator = consume_white_space(current_iterator, end);
        }

        constexpr bool is_finished() const {
            return current_iterator == end;
        }

        template<char c>
        bool parse_char_token() {
            if (is_finished()) {
                return false;
            }
            if (*current_iterator == c) {
                ++current_iterator;
                current_iterator = consume_white_space(current_iterator, end);
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
            if (current_end > end) {
                return false;
            }
            if (std::equal(str_data, str_end, current_iterator)) {
                current_iterator = current_end;
                current_iterator = consume_white_space(current_iterator, end);
                return true;
            }
            return false;
        }

        template<is_token_type T>
        auto parse_by() -> std::optional<typename T::result_type> {
            if (is_finished()) {
                return {};
            }
            auto [result_begin, result] = T::parse(current_iterator, end);
            current_iterator = consume_white_space(result_begin, end);
            return result;
        }
    };

    inline namespace token_type {
        export class identifier {
        public:
            using result_type = std::string;

            constexpr static parse_result_type<std::string> parse(const_iter_type begin, const_iter_type end) {
                auto curr = begin;

                if (curr == end || !std::isalpha(*curr) && *curr != '_') {
                    return {begin, std::nullopt};
                }
                auto original_begin = curr;
                ++curr;

                while (curr != end && (std::isalnum(*curr) || *curr == '_')) {
                    ++curr;
                }

                return {curr, std::string{original_begin, curr}};
            }
        };

        export template<std::integral T>
        class integer {
        public:
            using result_type = std::expected<T, error_type>;

            constexpr static parse_result_type<result_type> parse(const_iter_type begin, const_iter_type end) {
                if (begin == end) {
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

                while (begin != end && std::isdigit(*begin)) {
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

            constexpr static parse_result_type<result_type> parse(const_iter_type begin, const_iter_type end) {
                if (begin == end) {
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

                while (begin != end) {
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

                            begin = consume_white_space(begin, end);
                            if (begin == end) {
                                return {begin, std::unexpected{error_type::floating_point_format_error}};
                            }
                            auto [res_it, res]
                                    = integer<int>::parse(begin, end);
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
                const_iter_type begin, const_iter_type end) {
                if (begin == end) {
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

            constexpr static parse_result_type<result_type> parse(const_iter_type begin, const_iter_type end) {
                if (begin == end) {
                    return {begin, std::nullopt};
                }

                if (*begin != '"') {
                    return {begin, std::nullopt};
                }
                ++begin;

                std::stringstream ss;

                auto original_begin = begin;
                while (begin != end && *begin != '"') {
                    if (*begin == '\\') {
                        ++begin;
                        if (begin == end) {
                            return {original_begin, std::unexpected{error_type::string_format_error}};
                        }
                        auto pair = handle_escape_sequence(begin, end);
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

                if (begin == end) {
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

            constexpr static parse_result_type<result_type> parse(const_iter_type begin, const_iter_type end) {
                if (begin == end) {
                    return {begin, std::nullopt};
                }

                auto original_begin = begin;

                while (begin != end && std::isalnum(*begin)) {
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
