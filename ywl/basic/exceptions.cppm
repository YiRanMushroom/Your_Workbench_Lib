module;

export module ywl.basic.exceptions;

import ywl.std.prelude;
import ywl.basic.string_literal;

namespace ywl::basic {
    export class ywl_exception_base : public std::exception {
        std::string message;
        std::source_location location;
        // std::stacktrace stacktrace;

        mutable std::optional <std::string> full_message;

    public:
        ywl_exception_base() noexcept = delete;

        explicit ywl_exception_base(std::string message,
                                    std::source_location location = std::source_location::current()
                //, std::stacktrace stacktrace = std::stacktrace::current()
        )
        noexcept: message{std::move(message)}, location{std::move(location)}
        // ,stacktrace{std::move(stacktrace)}
        {} // NOLINT

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
                full_message = std::format("{}: {}\nat: {}:{}:{}", exception_reminder(), message,
                                           location.file_name(), location.line(), location.column()
                        // , stacktrace
                );
            }

            return full_message->c_str();
        }

        [[nodiscard]] const std::source_location &get_location() const noexcept {
            return location;
        }

        /*[[nodiscard]] const std::stacktrace &get_stacktrace() const noexcept {
            return stacktrace;
        }*/

        ~ywl_exception_base() noexcept override = default;
    };

    export class ywl_impl_error : public ywl_exception_base {
    public:
        ywl_impl_error() noexcept = delete;

        ywl_impl_error(std::string message, std::source_location location = std::source_location::current()
                // , std::stacktrace stacktrace = std::stacktrace::current()
        )
        noexcept: ywl_exception_base{std::move(message), std::move(location)
                // , std::move(stacktrace)
        } {}

        ywl_impl_error(const ywl_impl_error &) = default;

        ywl_impl_error(ywl_impl_error &&) noexcept = default;

        ywl_impl_error &operator=(const ywl_impl_error &) = default;

        ywl_impl_error &operator=(ywl_impl_error &&) noexcept = default;

        [[nodiscard]] const char *exception_reminder() const noexcept override {
            return "ywl_impl_error";
        }
    };

    export template<ywl::basic::string_literal reminder = "RuntimeError">
    class runtime_error : public ywl_exception_base {
    public:
        runtime_error() noexcept = delete;

        runtime_error(std::string message, std::source_location location = std::source_location::current()
                // , std::stacktrace stacktrace = std::stacktrace::current()
        )
        noexcept: ywl_exception_base{std::move(message), std::move(location)
                // , std::move(stacktrace)
        } {}

        runtime_error(const runtime_error &) = default;

        runtime_error(runtime_error &&) noexcept = default;

        runtime_error &operator=(const runtime_error &) = default;

        runtime_error &operator=(runtime_error &&) noexcept = default;

        [[nodiscard]] const char *exception_reminder() const noexcept override {
            return reminder.data;
        }
    };

    export template<ywl::basic::string_literal reminder = "LogicError">
    class logic_error : public ywl_exception_base {
    public:
        logic_error() noexcept = delete;

        logic_error(std::string message, std::source_location location = std::source_location::current()
                // , std::stacktrace stacktrace = std::stacktrace::current()
        )
        noexcept: ywl_exception_base{std::move(message), std::move(location)
                // , std::move(stacktrace)
        } {}

        logic_error(const logic_error &) = default;

        logic_error(logic_error &&) noexcept = default;

        logic_error &operator=(const logic_error &) = default;

        logic_error &operator=(logic_error &&) noexcept = default;

        [[nodiscard]] const char *exception_reminder() const noexcept override {
            return reminder.data;
        }
    };

#define DECLARE_EXCEPTION(name, reminder_m) \
    export class name : public ywl::basic::runtime_error<reminder_m> { \
    public:\
        name() noexcept = delete;         \
        name(std::string message, std::source_location location = std::source_location::current()\
                /*, std::stacktrace stacktrace = std::stacktrace::current()*/\
        )                                 \
        noexcept: ywl::basic::runtime_error<reminder_m>{std::move(message), std::move(location) \
                /*, std::move(stacktrace)*/ \
        } {}                              \
        name(const name &) = default;     \
        name(name &&) noexcept = default; \
        name &operator=(const name &) = default;                     \
        name &operator=(name &&) noexcept = default;                 \
        [[nodiscard]] const char *exception_reminder() const noexcept override { \
            return ywl::basic::runtime_error<reminder_m>::exception_reminder(); \
        } \
    };

    DECLARE_EXCEPTION(null_pointer_exception, "NullPtrException")

    DECLARE_EXCEPTION(out_of_range_exception, "OutOfRangeException")

    DECLARE_EXCEPTION(invalid_argument_exception, "InvalidArgumentException")

    DECLARE_EXCEPTION(integer_overflow_exception, "IntegerOverflowException")

    DECLARE_EXCEPTION(illegal_access_exception, "IllegalAccessException")

    DECLARE_EXCEPTION(illegal_state_exception, "IllegalStateException")

    DECLARE_EXCEPTION(integer_divide_by_zero_exception, "IntegerDivideByZeroException")

    DECLARE_EXCEPTION(overflow_exception, "OverflowException")

    DECLARE_EXCEPTION(illegal_instruction_exception, "IllegalInstructionException")
}
