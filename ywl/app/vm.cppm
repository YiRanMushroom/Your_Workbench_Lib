module;

#include <csignal>

export module ywl.app.vm;

import ywl.std.prelude;
import ywl.basic.exceptions;
import ywl.util.logger;

namespace ywl::app::vm {
    export template<int(*main)(int, char **)>
    int run(int argc, char **argv) {
        // register signal handler for sigsegv

        std::signal(SIGSEGV, [](int sig) {
            throw ywl::basic::illegal_access_exception("Segmentation fault");
        });

        std::signal(SIGFPE, [](int sig) {
            throw ywl::basic::overflow_exception("Floating point exception");
        });

        std::signal(SIGILL, [](int sig) {
            throw ywl::basic::illegal_instruction_exception("Illegal instruction");
        });

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