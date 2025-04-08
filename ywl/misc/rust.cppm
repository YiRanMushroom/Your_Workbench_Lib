module;

#include <stdc++pch.hpp>

export module ywl.misc.rust;

// import ywl.std.prelude;
import ywl.basic.helpers;

namespace ywl::misc::rust {
    export template<typename, typename...>
    class rust_enum {
        static_assert(false, "Template parameters hint must be a enum type with a list of constant_with_varable types");
    };

    template<typename EnumType, EnumType... EnumValues, typename... StoredTypes>
    class rust_enum<EnumType,
                ywl::basic::constant_with_variable<EnumValues, StoredTypes>...> {
    public:
        using variant_type = ywl::basic::unique_variant_t<StoredTypes...>;
        constexpr static auto values = std::make_tuple(EnumValues...);
        using constant_with_variable_types = std::tuple<ywl::basic::constant_with_variable<EnumValues, StoredTypes>...>;
        using enum_type = EnumType;

    private:
        enum_type m_enum;
        variant_type m_value;

        template<enum_type, typename...>
        struct find_constant_with_variable_from_enum;

        template<enum_type provided_enum>
        struct find_constant_with_variable_from_enum<provided_enum> {
            using type = void;
        };

        template<enum_type provided_enum, enum_type first_enum,
            typename first_stored_type, typename... rest_constant_with_variable>
        struct find_constant_with_variable_from_enum<provided_enum,
                    ywl::basic::constant_with_variable<first_enum, first_stored_type>,
                    rest_constant_with_variable...> {
            using type = std::conditional_t<provided_enum == first_enum,
                first_stored_type,
                typename find_constant_with_variable_from_enum<provided_enum, rest_constant_with_variable...>::type>;
        };

        template<enum_type provided_enum>
        using find_type_from_enum_t
        = typename find_constant_with_variable_from_enum<provided_enum, ywl::basic::constant_with_variable<EnumValues,
            StoredTypes>...>::type;

        rust_enum(enum_type e, variant_type v) : m_enum(e), m_value(v) {
        }

    public:
        constexpr static bool enums_are_not_equal() {
            // ywl::print_ln("Enum values: ", values);
            constexpr static auto count_element = []<enum_type element>() -> size_t {
                return []<size_t... idx>(std::index_sequence<idx...>) {
                    return ((std::get<idx>(values) == element ? 1 : 0) + ...);
                }(std::make_index_sequence<sizeof...(EnumValues)>{});
            };

            return []<size_t... idx>(std::index_sequence<idx...>) -> bool {
                return ((
                    count_element.template operator()<std::get<idx>(values)>() == 1
                ) && ...);
            }(std::make_index_sequence<sizeof...(EnumValues)>{});
        }

        static_assert(enums_are_not_equal(), "Enum values must be unique");

        template<enum_type provided_enum>
        constexpr bool holds_alternative() const {
            return m_enum == provided_enum;
        }

        template<enum_type provided_enum>
        decltype(auto) get(this auto &&self) {
            return std::get<find_type_from_enum_t<provided_enum>>(std::forward<decltype(self)>(self).m_value);
        }

        rust_enum() = delete;

        template<enum_type provided_enum>
        static rust_enum create(auto &&... args) {
            using stored_type = find_type_from_enum_t<provided_enum>;
            return rust_enum{
                provided_enum, variant_type(
                    stored_type(std::forward<decltype(args)>(args)...)
                )
            };
        }

        template<enum_type provided_enum>
        decltype(auto) match(this auto &&self, std::invocable<find_type_from_enum_t<provided_enum>> auto &&f) {
            if (self.template holds_alternative<provided_enum>()) {
                f(self.template get<provided_enum>());
            }
            return self;
        }
    };
}
