module;

export module ywl.miscellaneous.random;

import ywl.std.prelude;

namespace ywl::miscellaneous {
    namespace ns_random::impl {
        template<typename T, typename engine_type = std::mt19937, template
            <typename> typename distribution_type = std::uniform_int_distribution>
        struct function_type {
        private:
            engine_type engine{std::random_device{}()};
            distribution_type<T> distribution;

        public:
            constexpr T operator()() {
                return distribution(engine);
            }

            constexpr function_type(T min, T max) : distribution(std::move(min), std::move(max)) {}
        };
    }

    export template<typename T, typename engine_type = std::mt19937, template
        <typename> typename distribution_type = std::uniform_int_distribution>
    constexpr decltype(auto) random_generator_int(T min, T max) {
        return ns_random::impl::function_type<T, engine_type, distribution_type>(min, max);
    }

    export template<typename T, typename engine_type = std::mt19937, template
        <typename> typename distribution_type = std::uniform_int_distribution>
    constexpr T random_int(T min, T max) {
        return random_generator_int<T, engine_type, distribution_type>(min, max)();
    }

    export template<typename T, typename engine_type = std::mt19937, template
        <typename> typename distribution_type = std::uniform_real_distribution>
    constexpr decltype(auto) random_generator_real(T min, T max) {
        return ns_random::impl::function_type<T, engine_type, distribution_type>(min, max);
    }

    export template<typename T, typename engine_type = std::mt19937, template
        <typename> typename distribution_type = std::uniform_real_distribution>
    constexpr T random_real(T min, T max) {
        return random_generator_real<T, engine_type, distribution_type>(min, max)();
    }

    export template<typename T, typename engine_type, template
        <typename> typename distribution_type>
    constexpr decltype(auto) random_generator(T min, T max) {
        return ns_random::impl::function_type<T, engine_type, distribution_type>(min, max);
    }

    export template<typename T, typename engine_type, template
        <typename> typename distribution_type>
    constexpr decltype(auto) random(T min, T max) {
        return random_generator<T, engine_type, distribution_type>(min, max)();
    }
}
