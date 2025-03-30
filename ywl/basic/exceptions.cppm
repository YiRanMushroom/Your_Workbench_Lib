module;

#include <version>

#ifdef _WIN32

#include <windows.h>
#include <dbghelp.h>

#endif

export module ywl.basic.exceptions;

import ywl.std.prelude;
import ywl.basic.string_literal;

namespace ywl::basic {
    std::optional<std::string> get_stacktrace() {
#if (__has_include(<stacktrace>) && !defined(__GLIBCXX__))
        return std::format("{}", std::stacktrace::current());
#elifdef  _WIN32
        constexpr int max_frames = 128;
        void *stack[max_frames];
        USHORT frames = CaptureStackBackTrace(0, max_frames, stack, nullptr);

        HANDLE process = GetCurrentProcess();
        SymInitialize(process, nullptr, true);
        DWORD options = SymGetOptions();
        options |= SYMOPT_LOAD_LINES | SYMOPT_UNDNAME;
        SymSetOptions(options);

        std::stringstream ss;

        constexpr size_t max_name_len = 256;
        constexpr size_t buffer_size = sizeof(SYMBOL_INFO) + max_name_len;
        uint8_t buffer[buffer_size];
        SYMBOL_INFO *symbol = reinterpret_cast<SYMBOL_INFO *>(buffer);
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen = max_name_len;

        IMAGEHLP_LINE64 line = {};
        line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
        DWORD line_displacement;

        for (USHORT i = 0; i < frames; i++) {
            DWORD64 address = reinterpret_cast<DWORD64>(stack[i]);
            DWORD64 displacement = 0;

            if (SymFromAddr(process, address, &displacement, symbol)) {
                ss << std::format("#{}: {} ", i, symbol->Name);

                if (SymGetLineFromAddr64(process, address, &line_displacement, &line)) {
                    ss << std::format("at {}:{}", line.FileName, line.LineNumber);
                }

            } else {
                ss << std::format("#{}: {:#018x}", i, static_cast<uintptr_t>(address));
            }

            if (i < frames - 1) {
                ss << '\n';
            }
        }

        SymCleanup(process);
        return ss.str();
#else
        return {};
#endif
    }


    export class ywl_exception_base : public std::exception {
        std::string message;
        std::source_location location;
        std::optional <std::string> stacktrace;
        mutable std::optional <std::string> full_message;

    public:
        ywl_exception_base() noexcept = delete;

        explicit ywl_exception_base(std::string message,
                                    std::source_location location = std::source_location::current()
                //, std::stacktrace stacktrace = std::stacktrace::current()
        )
        noexcept: message{std::move(message)}, location{std::move(location)}, stacktrace{
                get_stacktrace()
        }
        // ,stacktrace{std::move(stacktrace)}
        {

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
                full_message = std::format("{}: {}\nat: {}:{}:{}", exception_reminder(), message,
                                           location.file_name(), location.line(), location.column()
                        // , stacktrace
                );

                if (stacktrace) {
                    full_message->append("\nStacktrace:\n");
                    full_message->append(stacktrace.value());
                }
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

    DECLARE_EXCEPTION(null_optional_exception, "NullOptionalException")
}
