export module ywl.utils.impl;

import ywl.std.prelude;
import ywl.basic.exceptions;

namespace ywl::utils {
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

    constexpr bool create_directory(const std::filesystem::path &path) {
        // if directory is "" return true
        if (path.empty()) {
            return true;
        }
        if (std::filesystem::exists(path)) {
            return true;
        }
        return std::filesystem::create_directories(path);
    }

    export constexpr std::ifstream read_or_create_file(const std::filesystem::path& path) {
        if (!ywl::utils::create_directory(path.parent_path())) {
            throw ywl::basic::runtime_error(
                std::format("Cannot open file because the directory {} cannot be created.",
                            path.parent_path().string()));
        }

        if (std::filesystem::exists(path)) {
            if (auto ifs = std::ifstream{path}) {
                return ifs;
            }

            throw ywl::basic::runtime_error(std::format("Cannot open file: {}", path.string()));
        }
        // create the file
        if (std::ofstream{path}) {
            if (auto ifs = std::ifstream{path}) {
                return ifs;
            }

            throw ywl::basic::runtime_error(std::format("Cannot open file: {}", path.string()));
        }

        throw ywl::basic::runtime_error(std::format("Cannot create file: {}", path.string()));
    }

    export constexpr std::ofstream write_or_create_file(const std::filesystem::path& path) {
        if (ywl::utils::create_directory(path.parent_path())) {
            if (std::ofstream ofs{path}) {
                return ofs;
            }
            throw ywl::basic::runtime_error(std::format("Cannot open file: {}", path.string()));
        }

        throw ywl::basic::runtime_error(
            std::format("Cannot open file because the directory {} cannot be created.", path.string()));
    }
}
