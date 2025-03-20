module;

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "imagehlp.lib")
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
        // 初始化符号处理
        void init_symbols() {
            SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
            SymInitialize(GetCurrentProcess(), NULL, TRUE);
        }

        // 处理异常时保存上下文并抛出异常
        LONG WINAPI vectored_exception_handler(PEXCEPTION_POINTERS ep) {
            // 保存当前上下文
            CONTEXT *context = ep->ContextRecord;

            // 根据异常类型抛出对应异常
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


    export template<int(*main)(int, char **)>
    int run(int argc, char **argv) {
#ifdef _WIN32
        detail::init_symbols();
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

        return 0;
    }
}
