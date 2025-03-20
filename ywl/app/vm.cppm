module;

#include <csignal>

export module ywl.app.vm;

import ywl.std.prelude;
import ywl.basic.exceptions;
import ywl.util.logger;

namespace ywl::app::vm {
    export template<int(*main)(int, char**)>
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

        try {
            return main(argc, argv);
        } catch (std::exception& e) {
            ywl::util::err_print_ln(e.what());
        } catch (...) {
            ywl::util::err_print_ln("Unknown exception was caught in vm, exiting...");
        }

        return -1;
    }
}