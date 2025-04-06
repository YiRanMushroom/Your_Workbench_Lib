module;

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dbghelp.h>
#else
#include <csignal>
#endif

export module ywl.app.vm;

import ywl.std.prelude;
import ywl.basic.exceptions;
import ywl.utils.logger;

import ywl.misc.coroutine.framework;

namespace ywl::app::vm {
#ifdef _WIN32
    namespace detail {
        LONG WINAPI vectored_exception_handler(PEXCEPTION_POINTERS ep) {
            switch (ep->ExceptionRecord->ExceptionCode) {
                case EXCEPTION_ACCESS_VIOLATION: {
                    auto addr = reinterpret_cast<void *>(ep->ExceptionRecord->ExceptionInformation[1]);
                    throw ywl::basic::illegal_access_exception(
                        std::format("Segmentation fault at address {}", addr)
                    );
                }
                case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                case EXCEPTION_INT_DIVIDE_BY_ZERO:
                    throw ywl::basic::overflow_exception("Division by zero");
                case EXCEPTION_ILLEGAL_INSTRUCTION:
                    throw ywl::basic::illegal_instruction_exception("Illegal instruction");
                default:
                    return EXCEPTION_CONTINUE_SEARCH;
            }
        }
    }
#endif

    bool vm_initialized_cond = false;

    export bool vm_initialized() {
        return vm_initialized_cond;
    }

    void init_vm() {
#ifdef _WIN32
        AddVectoredExceptionHandler(1, detail::vectored_exception_handler);
#else
        std::signal(SIGSEGV, [](int sig) {
            throw ywl::basic::illegal_access_exception("Segmentation fault");
        });

        std::signal(SIGFPE, [](int sig) {
            throw ywl::basic::overflow_exception("Floating point exception");
        });

        std::signal(SIGILL, [](int sig) {
            throw ywl::basic::illegal_instruction_exception("Illegal instruction");
        });
#endif
        std::set_terminate([]() {
            utils::err_print_ln("Terminate called, this could due to compiler optimization of exception table.");
            try {
                std::rethrow_exception(std::current_exception());
            } catch (const std::exception &e) {
                utils::err_printf_ln("The last exception was: {}", e.what());
            } catch (...) {
                utils::err_print_ln("The last exception was unknown.");
            }
        });

        vm_initialized_cond = true;
    }

    export template<int(*main)(int, char **)>
    int run(int argc, char **argv) {
        init_vm();

        int result = 0;

        try {
            result = main(argc, argv);
        } catch (std::exception &e) {
            ywl::utils::err_printf_ln(
                "VM caught an uncaptured exception which is not handled by the provided main function:\n{}\nExiting...",
                e.what());
        } catch (...) {
            ywl::utils::err_print_ln("Unknown exception was caught in VM. Exiting...");
        }

        return result;
    }

    export template<
        ywl::misc::coroutine::co_awaitable<int> (*async_main)(int, char **),
        std::derived_from<ywl::misc::coroutine::co_executor_base> Executor_Type>
    constexpr int async_run(int argc, char **argv, auto&&... args) {
        init_vm();

        auto co_context = ywl::misc::coroutine::co_context::from_executor<Executor_Type>(
            std::forward<decltype(args)>(args)...);

        int result = 0;

        try {
            result = co_context.block_on(async_main(argc, argv));
        } catch (std::exception &e) {
            ywl::utils::err_printf_ln(
                "VM caught an uncaptured exception which is not handled by the provided main function:\n{}\nExiting...",
                e.what());
        } catch (...) {
            ywl::utils::err_print_ln("Unknown exception was caught in VM. Exiting...");
        }

        return result;
    }
}
