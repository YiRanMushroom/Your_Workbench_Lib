module;

#include <cassert>
#include <concepts>
#include <cstring>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

export module ywl.miscellaneous.simple_buffer;

import ywl.basic.helpers;

namespace ywl::miscellaneous {
    export template<typename T>
    struct buffer_impl_t {
    };

    export template<typename T>
    concept bufferable = requires(std::vector<unsigned char> &container, const T &val)
    {
        { buffer_impl_t<T>::read_from(container) } -> std::same_as<T>;
        { buffer_impl_t<T>::write_to(val, container) };
    };

    export class simple_buffer {
    public:
        simple_buffer() = default;

        simple_buffer(const simple_buffer &) = delete;

        simple_buffer(simple_buffer &&) = default;

        simple_buffer &operator=(const simple_buffer &) = delete;

        simple_buffer &operator=(simple_buffer &&) = default;

        template<bufferable T>
        void push_back(this simple_buffer &self, const T &val) {
            buffer_impl_t<T>::write_to(val, self.m_buffer);
        }

        template<bufferable T>
        T pop_back(this simple_buffer &self) {
            return buffer_impl_t<T>::read_from(self.m_buffer);
        }

        template<bufferable T>
        simple_buffer &operator>>(this simple_buffer &self, T &val) {
            val = self.pop_back<T>();
            return self;
        }

        template<bufferable T>
        simple_buffer &operator<<(this simple_buffer &self, const T &val) {
            self.push_back(val);
            return self;
        }


    private:
        std::vector<unsigned char> m_buffer;
    };

    export template<typename T> requires std::is_trivial_v<T>
    struct buffer_impl_t<T> {
        static T read_from(std::vector<unsigned char> &container) {
            // read_from_back
            assert(container.size() >= sizeof(T));
            T val{val};
            std::memcpy(&val, container.data() + container.size() - sizeof(T), sizeof(T));
            container.resize(container.size() - sizeof(T));
            return val;
        }

        static void write_to(const T &val, std::vector<unsigned char> &container) {
            // write_to_back
            container.resize(container.size() + sizeof(T));
            std::memcpy(container.data() + container.size() - sizeof(T), &val, sizeof(T));
        }
    };

    export template<bufferable T1, bufferable T2>
    struct buffer_impl_t<std::pair<T1, T2> > {
        static std::pair<T1, T2> read_from(std::vector<unsigned char> &container) {
            return {buffer_impl_t<T1>::read_from(container), buffer_impl_t<T2>::read_from(container)};
        }

        static void write_to(const std::pair<T1, T2> &val, std::vector<unsigned char> &container) {
            buffer_impl_t<T2>::write_to(val.second, container), buffer_impl_t<T1>::write_to(val.first, container);
        }
    };

    export template<bufferable... Ts>
    struct buffer_impl_t<std::tuple<Ts...> > {
        using index_sequence = ywl::basic::index_sequence_t<sizeof...(Ts)>;
        using reverse_index_sequence = ywl::basic::reverse_index_sequence_t<sizeof...(Ts)>;

        static std::tuple<Ts...> read_from(std::vector<unsigned char> &container) {
            return [&]<size_t... Is>(std::index_sequence<Is...> seq) {
                return std::make_tuple(buffer_impl_t<Ts>::read_from(container)...);
            }(index_sequence{});
        }

        static void write_to(const std::tuple<Ts...> &val, std::vector<unsigned char> &container) {
            [&]<size_t... Is>(std::index_sequence<Is...> seq) {
                (buffer_impl_t<Ts>::write_to(std::get<Is>(val), container), ...);
            }(reverse_index_sequence{});
        }
    };

    export template<bufferable K, bufferable V>
    struct buffer_impl_t<std::unordered_map<K, V> > {
        static std::unordered_map<K, V> read_from(std::vector<unsigned char> &container) {
            std::unordered_map<K, V> val;
            size_t size = buffer_impl_t<size_t>::read_from(container);
            for (size_t i = 0; i < size; ++i) {
                auto entry = buffer_impl_t<std::pair<K, V> >::read_from(container);
                val.insert(std::move(entry));
            }
            return val;
        }

        static void write_to(const std::unordered_map<K, V> &val, std::vector<unsigned char> &container) {
            for (const auto &entry: val) {
                buffer_impl_t<std::pair<K, V> >::write_to(entry, container);
            }
            buffer_impl_t<size_t>::write_to(val.size(), container);
        }
    };

    export template<bufferable K, bufferable V>
    struct buffer_impl_t<std::map<K, V> > {
        static std::map<K, V> read_from(std::vector<unsigned char> &container) {
            std::map<K, V> val;
            size_t size = buffer_impl_t<size_t>::read_from(container);
            for (size_t i = 0; i < size; ++i) {
                auto key = buffer_impl_t<K>::read_from(container);
                auto value = buffer_impl_t<V>::read_from(container);
                val[std::move(key)] = std::move(value);
            }
            return val;
        }

        static void write_to(const std::map<K, V> &val, std::vector<unsigned char> &container) {
            for (const auto &[key, value]: val) {
                buffer_impl_t<V>::write_to(value, container);
                buffer_impl_t<K>::write_to(key, container);
            }
            buffer_impl_t<size_t>::write_to(val.size(), container);
        }
    };

    export template<bufferable T, template<typename...> typename Container>
        requires requires(Container<T> container)
        {
            { container.size() } -> std::convertible_to<size_t>;
            { container.push_back(std::declval<T>()) };
            std::rbegin(container);
            std::rend(container);
        }
    struct buffer_impl_t<Container<T> > {
        static Container<T> read_from(std::vector<unsigned char> &container) {
            size_t size = buffer_impl_t<size_t>::read_from(container);
            Container<T> val;
            if constexpr (requires { val.reserve(size); }) {
                val.reserve(size);
            }
            for (size_t i = 0; i < size; ++i) {
                val.push_back(buffer_impl_t<T>::read_from(container));
            }
            return val;
        }

        static void write_to(const Container<T> &val, std::vector<unsigned char> &container) {
            for (auto it = std::rbegin(val); it != std::rend(val); ++it) {
                buffer_impl_t<T>::write_to(*it, container);
            }
            buffer_impl_t<size_t>::write_to(val.size(), container);
        }
    };

    export template<>
    struct buffer_impl_t<std::string> {
        static std::string read_from(std::vector<unsigned char> &container) {
            size_t size = buffer_impl_t<size_t>::read_from(container);
            std::string val;
            val.resize(size);
            std::memcpy(val.data(), container.data() + container.size() - size, size);
            container.resize(container.size() - size);
            return val;
        }

        static void write_to(const std::string &val, std::vector<unsigned char> &container) {
            container.resize(container.size() + val.size());
            std::memcpy(container.data() + container.size() - val.size(), val.data(), val.size());
            buffer_impl_t<size_t>::write_to(val.size(), container);
        }
    };
}
