module;

#include <format>
#include <sstream>
#include <string>

export module ywl.risky.overloads.format;

export import ywl.overloads.std_overloads;

namespace ywl {
    namespace impl {
        template<typename T>
        constexpr std::string geneic_container_onetype_to_string(const T &t) {
            std::stringstream result;
            result << '[';
            for (auto it = t.begin(); it != t.end(); ++it) {
                result << *it;
                if (std::next(it) != t.end()) {
                    result << ", ";
                }
            }
            result << ']';
            return result.str();
        }
    }

    // export template<typename T>
    // struct format_type_function_ud<std::vector<T>> {
    //     static std::string to_string(const std::vector<T> &t) {
    //         return __impl::geneic_container_onetype_to_string(t);
    //     }
    // };
}