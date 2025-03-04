module;

#include <source_location>
#include <stacktrace>
#include <utility>
#include <format>
#include <optional>
#include <stdexcept> // NOLINT

export module ywl.basic.exceptions;

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
                       stacktrace{std::move(stacktrace)} {
        } // NOLINT

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
    };

    export class ywl_impl_error : public ywl_exception_base {
    public:
        ywl_impl_error() noexcept = delete;

        ywl_impl_error(std::string message, std::source_location location = std::source_location::current(),
                       std::stacktrace stacktrace = std::stacktrace::current())
            noexcept : ywl_exception_base{std::move(message), std::move(location), std::move(stacktrace)} {
        }

        ywl_impl_error(const ywl_impl_error &) = default;

        ywl_impl_error(ywl_impl_error &&) noexcept = default;

        ywl_impl_error &operator=(const ywl_impl_error &) = default;

        ywl_impl_error &operator=(ywl_impl_error &&) noexcept = default;

        [[nodiscard]] const char *exception_reminder() const noexcept override {
            return "error in ywl implementation";
        }
    };
}
