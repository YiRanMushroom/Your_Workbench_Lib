module;

#ifdef _WIN32

#include <windows.h>
#include <dbghelp.h>

#else
#include <csignal>
#endif

export module ywl.app.vm;

import ywl.std.prelude;
import ywl.basic.exceptions;
import ywl.util.logger;

namespace ywl::app::vm {
#ifdef _WIN32
    namespace detail {
        LONG WINAPI vectored_exception_handler(PEXCEPTION_POINTERS ep) {
            switch (ep->ExceptionRecord->ExceptionCode) {
                case EXCEPTION_ACCESS_VIOLATION:
                    throw ywl::basic::illegal_access_exception("Access violation");
                case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                    throw ywl::basic::overflow_exception("Divide by zero");
                case EXCEPTION_FLT_OVERFLOW:
                    throw ywl::basic::overflow_exception("Floating point overflow");
                case EXCEPTION_INT_DIVIDE_BY_ZERO:
                    throw ywl::basic::integer_divide_by_zero_exception("Divide by zero");
                case EXCEPTION_INT_OVERFLOW:
                    throw ywl::basic::integer_overflow_exception("Integer overflow");
                case EXCEPTION_ILLEGAL_INSTRUCTION:
                    throw ywl::basic::illegal_instruction_exception("Illegal instruction");
                default:
                    throw ywl::basic::runtime_error("Unknown exception");
            }
        }
    }
#endif

    export template<int(*main)(int, char **)>
    int run(int argc, char **argv) {
#ifdef _WIN32
        AddVectoredExceptionHandler(1, detail::vectored_exception_handler);
#else
        std::signal(SIGSEGV, [](int) {
            throw ywl::basic::illegal_access_exception("Segmentation fault");
        });
        std::signal(SIGFPE, [](int) {
            throw ywl::basic::overflow_exception("Floating point exception");
        });
        std::signal(SIGILL, [](int) {
            throw ywl::basic::illegal_instruction_exception("Illegal instruction");
        });
#endif

        std::set_terminate([]() {
            try {
                std::rethrow_exception(std::current_exception());
            } catch (const std::exception &e) {
                ywl::util::err_print_ln("Terminate called, this could be due to vm failed to catch an exception");
                ywl::util::err_printf_ln("The last exception was: {}", e.what());
            } catch (...) {
                ywl::util::err_print_ln("Terminate called, this could be due to vm failed to catch an exception");
                ywl::util::err_print_ln("The last exception was unknown");
            }
            ywl::util::err_print_ln("Exiting...");
            std::exit(-1);
        });

        try {
            return main(argc, argv);
        } catch (std::exception &e) {
            ywl::util::err_printf_ln(
                    "VM caught an uncaptured exception which is not handled by the provided main function:\n{}\nExiting",
                    e.what());
        } catch (...) {
            ywl::util::err_print_ln("Unknown exception was caught in VM. Exiting...");
        }

        return -1;
    }
}