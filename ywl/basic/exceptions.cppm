module;

// #include <source_location>
// #include <stacktrace>
// #include <utility>
// #include <format>
// #include <optional>
// #include <stdexcept> // NOLINT
import ywl.std.prelude;

export module ywl.basic.exceptions;

import ywl.basic.string_literal;

namespace ywl::basic {
    export class ywl_exception_base : public std::exception {
        std::string message;
        std::source_location location;
        std::stacktrace stacktrace;

        mutable std::optional<std::string> full_message;

    public:
        ywl_exception_base() noexcept = delete;

        explicit ywl_exception_base(std::string message,
                                    std::source_location location = std::source_location::current(),
                                    std::stacktrace stacktrace = std::stacktrace::current())
            noexcept : message{std::move(message)}, location{std::move(location)},
                       stacktrace{std::move(stacktrace)} {} // NOLINT

        [[nodiscard]] virtual const char *exception_reminder() const noexcept {
            return "ywl_exception_base";
        }

        ywl_exception_base(const ywl_exception_base &) = default;

        ywl_exception_base(ywl_exception_base &&) noexcept = default;

        ywl_exception_base &operator=(const ywl_exception_base &) = default;

        ywl_exception_base &operator=(ywl_exception_base &&) noexcept = default;

        [[nodiscard]] const char *what() const noexcept override {
            // return message.c_str();
            if (!full_message) {
                full_message = std::format("{}: {}\nat: {}:{}:{}\nstacktrace:\n{}", exception_reminder(), message,
                                           location.file_name(), location.line(), location.column(), stacktrace);
            }

            return full_message->c_str();
        }

        [[nodiscard]] const std::source_location &get_location() const noexcept {
            return location;
        }

        [[nodiscard]] const std::stacktrace &get_stacktrace() const noexcept {
            return stacktrace;
        }

        ~ywl_exception_base() noexcept override = default;
    };

    export class ywl_impl_error : public ywl_exception_base {
    public:
        ywl_impl_error() noexcept = delete;

        ywl_impl_error(std::string message, std::source_location location = std::source_location::current(),
                       std::stacktrace stacktrace = std::stacktrace::current())
            noexcept : ywl_exception_base{std::move(message), std::move(location), std::move(stacktrace)} {}

        ywl_impl_error(const ywl_impl_error &) = default;

        ywl_impl_error(ywl_impl_error &&) noexcept = default;

        ywl_impl_error &operator=(const ywl_impl_error &) = default;

        ywl_impl_error &operator=(ywl_impl_error &&) noexcept = default;

        [[nodiscard]] const char *exception_reminder() const noexcept override {
            return "ywl_impl_error";
        }
    };

    export template<ywl::basic::string_literal reminder = "RUNTIME ERROR">
    class runtime_error : public ywl_exception_base {
    public:
        runtime_error() noexcept = delete;

        runtime_error(std::string message, std::source_location location = std::source_location::current(),
                      std::stacktrace stacktrace = std::stacktrace::current())
            noexcept : ywl_exception_base{std::move(message), std::move(location), std::move(stacktrace)} {}

        runtime_error(const runtime_error &) = default;

        runtime_error(runtime_error &&) noexcept = default;

        runtime_error &operator=(const runtime_error &) = default;

        runtime_error &operator=(runtime_error &&) noexcept = default;

        [[nodiscard]] const char *exception_reminder() const noexcept override {
            return reminder.data;
        }
    };

    export template<ywl::basic::string_literal reminder = "LOGIC ERROR">
    class logic_error : public ywl_exception_base {
    public:
        logic_error() noexcept = delete;

        logic_error(std::string message, std::source_location location = std::source_location::current(),
                      std::stacktrace stacktrace = std::stacktrace::current())
            noexcept : ywl_exception_base{std::move(message), std::move(location), std::move(stacktrace)} {}

        logic_error(const logic_error &) = default;

        logic_error(logic_error &&) noexcept = default;

        logic_error &operator=(const logic_error &) = default;

        logic_error &operator=(logic_error &&) noexcept = default;

        [[nodiscard]] const char *exception_reminder() const noexcept override {
            return reminder.data;
        }
    };
}
