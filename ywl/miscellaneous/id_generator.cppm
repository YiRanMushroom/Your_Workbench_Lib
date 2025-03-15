module;

import ywl.std.prelude;

export module ywl.miscellaneous.id_generator;

namespace ywl::miscellaneous {
    export template<typename T = uint64_t, T default_value = T{}> requires requires(T t) { ++t; std::numeric_limits<T>::max(); }
    class discrete_id_generator {
        T current_id{};
        std::vector<T> freed_ids{};

    public:
        using value_type = T;

        constexpr discrete_id_generator() = default;

        constexpr discrete_id_generator(const discrete_id_generator &) = delete;

        constexpr discrete_id_generator(discrete_id_generator &&other) noexcept {
            this->current_id = other.current_id;
            other.current_id = default_value;
            this->freed_ids = std::move(other.freed_ids);
        }

        constexpr discrete_id_generator &operator=(const discrete_id_generator &) = delete;

        constexpr discrete_id_generator &operator=(discrete_id_generator &&other) noexcept {
            this->current_id = other.current_id;
            other.current_id = default_value;
            this->freed_ids = std::move(other.freed_ids);
            return *this;
        }

        constexpr T generate() {
            if (!freed_ids.empty()) {
                T id = freed_ids.back();
                freed_ids.pop_back();
                return id;
            }

            if (current_id == std::numeric_limits<T>::max()) {
                throw std::runtime_error("No more ids available.");
            }

            ++current_id;

            return current_id;
        }

        constexpr void free(T &&t) {
            // if t is in freed_ids, the behavior is undefined
            // if t is default_value, do nothing
            if (t != default_value) {
                freed_ids.push_back(std::move(t));
                t = default_value;
            }
        }
    };
}
