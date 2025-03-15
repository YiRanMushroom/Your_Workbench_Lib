module;

import ywl.std.prelude;

export module ywl.util.impl;

namespace ywl::util {
    export constexpr std::optional<std::string> get_istream_input(auto &is) {
        if (auto *buffer = is.rdbuf()) {
            int next = buffer->sgetc();
            if (next != std::ios::traits_type::eof()) {
                std::string input;
                int ch;
                while ((ch = buffer->sbumpc()) != std::ios::traits_type::eof()) {
                    if (ch == '\n') {
                        if (!input.empty() && input.back() == '\r') {
                            input.pop_back();
                        }
                        return input;
                    }
                    input.push_back(static_cast<char>(ch));
                }
            }
        }
        return {};
    }
}
