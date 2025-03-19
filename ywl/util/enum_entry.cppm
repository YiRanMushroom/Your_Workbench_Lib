export module ywl.util.enum_entry;

import ywl.std.prelude;

namespace ywl::util::enum_entry {
    export template<typename EnumType, EnumType enum_value, typename ValueType>
    struct enum_entry {
        using enum_type = EnumType;
        using value_type = ValueType;
        static constexpr EnumType value = enum_value;

        value_type data{};
    };

    export template<typename EnumType, EnumType enum_value>
    struct enum_entry<EnumType, enum_value, void> {
        using enum_type = EnumType;
        using value_type = void;
        static constexpr EnumType value = enum_value;

        enum_entry() = default;
    };

    export struct variant_pipe_flag_t {
    };

    export template<typename>
    constexpr bool is_variant_of_enum_entries_v = false;

    export template<typename EnumType, EnumType... enum_values, typename... ValueTypes>
    constexpr bool is_variant_of_enum_entries_v<std::variant<enum_entry<EnumType, enum_values, ValueTypes>...>> =
            true;

    export template<typename T>
    concept variant_of_enum_entries = is_variant_of_enum_entries_v<std::remove_cvref_t<T>>;

    template<typename Variant_Enum_Type, auto Enum_Value>
    struct find_enum_to_type {
        static_assert(false, "Not found");
    };

    struct not_found_t;

    template<typename Enum_Type, Enum_Type enum_value, Enum_Type last_enum_value, typename Last_Type>
    struct find_enum_to_type<std::variant<enum_entry<Enum_Type, last_enum_value, Last_Type>>, enum_value> {
        using type = std::conditional_t<enum_value == last_enum_value, enum_entry<Enum_Type, last_enum_value,
                Last_Type>,
            not_found_t>;
    };

    template<typename Enum_Type, Enum_Type enum_value, typename First_Type, typename... Rest_Types,
        Enum_Type first_enum_value, Enum_Type... rest_enum_values>
    struct find_enum_to_type<std::variant<enum_entry<Enum_Type, first_enum_value, First_Type>, enum_entry<Enum_Type,
                rest_enum_values, Rest_Types>...>, enum_value> {
        using type = std::conditional_t<enum_value == first_enum_value, enum_entry<Enum_Type, first_enum_value,
                First_Type>,
            typename find_enum_to_type<std::variant<enum_entry<Enum_Type, rest_enum_values, Rest_Types>...>,
                enum_value>::type>;
    };

    template<typename Variant_Enum_Type, auto Enum_Value>
    using find_enum_to_type_t = decltype(
        [] -> decltype(auto) {
            using result_type = typename find_enum_to_type<std::remove_cvref_t<Variant_Enum_Type>, Enum_Value>::type;
            static_assert(!std::is_same_v<result_type, not_found_t>, "Not found");
            return result_type{};
        }());

    template<typename Enum_Type, Enum_Type Enum_Value>
    struct holds_impl : variant_pipe_flag_t {
        constexpr static bool operator()(variant_of_enum_entries auto &&variant_enum) {
            using enum_entry_type = find_enum_to_type_t<decltype(variant_enum), Enum_Value>;
            return std::holds_alternative<enum_entry_type>(variant_enum);
        }
    };

    template<typename Enum_Type, Enum_Type Enum_Value>
    struct get_impl : variant_pipe_flag_t {
        constexpr static decltype(auto) operator()(variant_of_enum_entries auto &&variant_enum) {
            using enum_entry_type = find_enum_to_type_t<decltype(variant_enum), Enum_Value>;
            return (std::get<enum_entry_type>(std::forward<decltype(variant_enum)>(variant_enum)).data);
        }
    };

    template<typename Enum_Type, Enum_Type Enum_Value>
    struct get_if_move_impl : variant_pipe_flag_t {
        constexpr static auto operator()(variant_of_enum_entries auto &&variant_enum) {
            using enum_entry_type = find_enum_to_type_t<decltype(variant_enum), Enum_Value>;

            return std::move(std::get<enum_entry_type>(variant_enum).data);
        }
    };

    export template<auto Enum_Value>
    constexpr auto holds_enum = holds_impl<std::remove_cvref_t<decltype(Enum_Value)>, Enum_Value>{};

    export template<auto Enum_Value>
    constexpr auto get = get_impl<std::remove_cvref_t<decltype(Enum_Value)>, Enum_Value>{};

    export template<auto Enum_Value>
    constexpr auto get_if_move = get_if_move_impl<std::remove_cvref_t<decltype(Enum_Value)>,
        Enum_Value>{};

    template<typename>
    struct stored_type_of_enum {
        static_assert(false, "Not an enum entry");
    };

    template<typename Variant_Type, typename Enum_Type, Enum_Type Enum_Value>
    struct emplace_impl {
        constexpr auto static operator()(auto &&... args) {
            using enum_entry_type = find_enum_to_type_t<Variant_Type, Enum_Value>;
            using stored_type = typename enum_entry_type::value_type;
            if constexpr (std::is_same_v<stored_type, void>) {
                return Variant_Type(enum_entry_type());
            } else {
                return Variant_Type(enum_entry_type(stored_type(std::forward<decltype(args)>(args)...)));
            }
        }
    };

    export template<typename Variant_Type, auto Enum_Value>
    constexpr auto emplace = emplace_impl<Variant_Type, std::remove_cvref_t<decltype(Enum_Value)>, Enum_Value>{};
}


export template<ywl::util::enum_entry::variant_of_enum_entries Variant_Enum, std::convertible_to<
        ywl::util::enum_entry::variant_pipe_flag_t>
    Callable_Pipe_Type>
decltype(auto) operator|(Variant_Enum &&variant_enum, Callable_Pipe_Type &&callable_pipe) {
    return callable_pipe(std::forward<Variant_Enum>(variant_enum));
}
