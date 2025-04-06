export module ywl.basic.result;

import ywl.std.prelude;
import ywl.basic.exceptions;

namespace ywl::basic {
    template<typename T>
    class result_of_t {
    public:
        using value_type = T;

        constexpr result_of_t() = delete;

        constexpr explicit result_of_t(value_type &&value) : value(std::move(value)) {}

        constexpr result_of_t(const result_of_t &) = delete;

        constexpr result_of_t &operator=(const result_of_t &) = delete;

        constexpr result_of_t(result_of_t &&other) noexcept : value(std::move(other.value)) {}

        constexpr result_of_t &operator=(result_of_t &&other) noexcept {
            if (this != &other) {
                value = std::move(other.value);
            }
            return *this;
        }

        constexpr value_type take() && {
            return std::move(value);
        }

    private:
        value_type value{};
    };

    export template<typename T>
    constexpr auto make_result(auto &&... args) {
        return result_of_t{T{std::forward<decltype(args)>(args)...}};
    }

    template<typename T>
    class error_of_t {
    public:
        using value_type = T;

        constexpr error_of_t() = delete;

        constexpr explicit error_of_t(value_type &&value) : value(std::move(value)) {}

        constexpr error_of_t(const error_of_t &) = delete;

        constexpr error_of_t &operator=(const error_of_t &) = delete;

        constexpr error_of_t(error_of_t &&other) noexcept : value(std::move(other.value)) {}

        constexpr error_of_t &operator=(error_of_t &&other) noexcept {
            if (this != &other) {
                value = std::move(other.value);
            }
            return *this;
        }

        constexpr value_type take() && {
            return std::move(value);
        }

    private:
        value_type value{};
    };

    export template<typename T>
    constexpr auto error_of(auto &&... args) {
        return error_of_t{T{std::forward<decltype(args)>(args)...}};
    }

    export enum class result_state : uint8_t {
        none = 0,
        some = 1,
        error = 2,
    };

    export using result_state::none;
    export using result_state::some;
    export using result_state::error;

    export template<typename Result_Type, typename Error_Type>
    class result {
    public:
        using result_type = Result_Type;
        using error_type = Error_Type;

    private:
        std::optional<std::variant<result_type, error_type>> m_data{};

    public:
        constexpr result() = delete;

        constexpr result(const result &) = default;

        constexpr result(result &&) = default;

        constexpr result &operator=(const result &) = default;

        constexpr result &operator=(result &&) = default;

        constexpr result(result_of_t<result_type> &&value) : m_data(value.take()) {} // NOLINT

        constexpr result(error_of_t<error_type> &&error) : m_data(error.take()) {} // NOLINT

        constexpr result &operator=(result_of_t<result_type> &&value) {
            m_data = value.take();
            return *this;
        }

        constexpr result &operator=(error_of_t<error_type> &&error) {
            m_data = error.take();
            return *this;
        }

        [[nodiscard]] constexpr bool is_none() const {
            return !m_data.has_value();
        }

        [[nodiscard]] constexpr bool is_some() const {
            return m_data.has_value() && std::holds_alternative<result_type>(m_data);
        }

        [[nodiscard]] constexpr bool is_error() const {
            return m_data.has_value() && std::holds_alternative<error_type>(m_data);
        }

        [[nodiscard]] constexpr result_state get_state() const {
            if (!m_data.has_value()) {
                return result_state::none;
            }
            if (std::holds_alternative<result_type>(m_data)) {
                return result_state::some;
            }
            return result_state::error;
        }

        constexpr result_type &get_result() & {
            if (is_some()) {
                return std::get<result_type>(m_data);
            }
            throw runtime_error("Result is not some");
        }

        constexpr const result_type &get_result() const & {
            if (is_some()) {
                return std::get<result_type>(m_data);
            }
            throw runtime_error("Result is not some");
        }

        constexpr result_type &&get_result() && {
            if (is_some()) {
                return std::move(std::get<result_type>(m_data));
            }
            throw runtime_error("Result is not some");
        }

        constexpr const result_type &&get_result() const && {
            if (is_some()) {
                return std::move(std::get<result_type>(m_data));
            }
            throw runtime_error("Result is not some");
        }

        constexpr error_type &get_error() & {
            if (is_error()) {
                return std::get<error_type>(m_data);
            }
            throw runtime_error("Result is not error");
        }

        constexpr const error_type &get_error() const & {
            if (is_error()) {
                return std::get<error_type>(m_data);
            }
            throw runtime_error("Result is not error");
        }

        constexpr error_type &&get_error() && {
            if (is_error()) {
                return std::move(std::get<error_type>(m_data));
            }
            throw runtime_error("Result is not error");
        }

        constexpr const error_type &&get_error() const && {
            if (is_error()) {
                return std::move(std::get<error_type>(m_data));
            }
            throw runtime_error("Result is not error");
        }

        friend bool operator==(const result &self, result_state state) {
            return self.get_state() == state;
        }

        friend bool operator==(result_state state, const result &self) {
            return self.get_state() == state;
        }

        // can be structural bind to std::optional<result_type>, std::optional<error_type>

        template<size_t, typename, typename>
        constexpr friend auto ::std::get(result &&self);
    };
}


using namespace ywl::basic;

export template<typename Result_Type, typename Error_Type>
struct std::tuple_size<result<Result_Type, Error_Type>> : std::integral_constant<size_t, 2> {};

export template<typename Result_Type, typename Error_Type>
struct std::tuple_element<size_t{0}, result<Result_Type, Error_Type>> {
    using type = std::optional<Result_Type>;
};

export template<typename Result_Type, typename Error_Type>
struct std::tuple_element<size_t{1}, result<Result_Type, Error_Type>> {
    using type = std::optional<Error_Type>;
};

namespace std {
    export template<size_t I, typename Result_Type, typename Error_Type>
    constexpr auto &&get(result<Result_Type, Error_Type> &&self) {
        if constexpr (I == 0) {
            if (self.is_some()) {
                return std::move(self).get_result();
            }
            return decltype(std::move(self).get_result()){};
        } else if constexpr (I == 1) {
            if (self.is_error()) {
                return std::move(self).get_error();
            }
        } else {
            static_assert(false, "Index out of bounds");
        }
    }
}
