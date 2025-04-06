export module ywl.utils.impl;

import ywl.std.prelude;
import ywl.basic.exceptions;

namespace ywl::utils {
    template<typename T> // not well implemented, this will cause memory leak
    class input_listener {
        T &m_input_stream;
        std::mutex m_mutex;
        std::queue<std::string> m_queue;

        std::jthread m_thread;

    public:
        input_listener(T &input_stream)
            : m_input_stream{input_stream}, m_mutex{}, m_queue{},
              m_thread([this] {
                  std::string line;
                  while (true) {
                      /*if (std::cin.rdbuf()->in_avail() <= 0) {
                          std::this_thread::sleep_for(std::chrono::milliseconds(100));
                          continue;
                      }*/
                      std::getline(m_input_stream, line);
                      if (line.empty()) {
                          continue;
                      }
                      std::lock_guard lock(m_mutex);
                      m_queue.push(line);
                  }
              }) {
        }

        std::optional<std::string> get_input() {
            std::lock_guard lock(m_mutex);
            if (m_queue.empty()) {
                return std::nullopt;
            }
            auto line = m_queue.front();
            m_queue.pop();
            return line;
        }

        ~input_listener() {
            m_thread.detach();
        }
    };

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

    export constexpr std::ifstream read_or_create_file(const std::filesystem::path &path) {
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

    export constexpr std::ofstream write_or_create_file(const std::filesystem::path &path) {
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
