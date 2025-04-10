module;

#include <stdc++pch.hpp>

export module ywl.basic.helpers;

namespace ywl::basic {
    template<auto, typename>
    struct function_t_crtp {
        static_assert(false, "Fn_T is not a function or function pointer type");
    };

    template<auto * fn, typename Ret_T, typename... Args_T>
    struct function_t_crtp<fn, Ret_T(*)(Args_T...)> {
        static_assert(std::is_invocable_v<decltype(fn), Args_T...>,
                      "The function pointer type does not match the function signature");
        using function_ptr_type = Ret_T(*)(Args_T...);

        constexpr Ret_T operator()(Args_T... args) const noexcept(noexcept(fn(std::forward<Args_T>(args)...))) {
            return fn(std::forward<Args_T>(args)...);
        }

        constexpr operator function_ptr_type() const noexcept { // NOLINT
            return fn;
        }
    };

    export template<auto *fn>
    struct function_t : public function_t_crtp<fn, decltype(fn)> {};


    export struct nothing_fn_t {
        constexpr void operator()(auto &&...) const noexcept {}
    };

    template<size_t N, size_t... Is>
    struct make_reverse_index_sequence_impl :
            make_reverse_index_sequence_impl<N - 1, Is..., N - 1> {};

    template<size_t... Is>
    struct make_reverse_index_sequence_impl<0, Is...> {
        using type = std::index_sequence<Is...>;
    };

    export template<size_t N>
    using reverse_index_sequence_t = typename make_reverse_index_sequence_impl<N>::type;

    export template<size_t N>
    using index_sequence_t = std::make_index_sequence<N>;

    template<typename Tp, typename T>
    constexpr bool function_like_impl_v = false;

    template<typename Tp, typename Ret_T, typename... Args_T>
    constexpr bool function_like_impl_v<Tp, Ret_T(Args_T...)> = requires(Tp t, Args_T... args) {
        { t(std::forward<Args_T>(args)...) } -> std::convertible_to<Ret_T>;
    };

    export template<typename Tp, typename T>
    concept function_like = function_like_impl_v<Tp, T>;

    template<typename Tp, typename T>
    constexpr bool function_same_as_impl_v = false;

    template<typename Tp, typename Ret_T, typename... Args_T>
    constexpr bool function_same_as_impl_v<Tp, Ret_T(Args_T...)> = requires(Tp t, Args_T... args) {
        { t(std::forward<Args_T>(args)...) } -> std::same_as<Ret_T>;
    };

    export template<typename Tp, typename T>
    concept function_same_as = function_same_as_impl_v<Tp, T>;

    export template<typename Constant_Type, Constant_Type m_constant, typename T>
    struct constant_with_variable_t {
        using constant_type = Constant_Type;
        using variable_type = T;

    private:
        variable_type m_variable;

    public:
        constant_with_variable_t(variable_type var) noexcept : m_variable{std::move(var)} {}

        static constexpr Constant_Type constant() noexcept {
            return m_constant;
        }

        constexpr const variable_type &variable() const & noexcept {
            return m_variable;
        }

        constexpr variable_type &variable() & noexcept {
            return m_variable;
        }

        constexpr variable_type &&variable() && noexcept {
            return std::move(m_variable);
        }

        constexpr const variable_type &&variable() const && noexcept {
            return std::move(m_variable);
        }

        template<constant_type constant>
        static constexpr bool is_same() noexcept {
            return constant == m_constant;
        }
    };

    export template<auto constant, typename T>
    using constant_with_variable = constant_with_variable_t<decltype(constant), constant, T>;

    template<typename CheckType, typename TupleType>
    struct type_in_tuple {
        static_assert(false, "Type template parameter is not a tuple type");
    };

    template<typename CheckType, typename... Types>
    struct type_in_tuple<CheckType, std::tuple<Types...>> {
        static constexpr bool value = (std::same_as<CheckType, Types> || ...);
    };

    export template<typename CheckType, typename TupleType>
    constexpr bool type_in_tuple_v = type_in_tuple<CheckType, TupleType>::value;

    template<typename... Types>
    struct unique_tuple_impl;

    template<>
    struct unique_tuple_impl<> {
        using type = std::tuple<>;
    };

    template<typename Last>
    struct unique_tuple_impl<Last> {
        using type = std::tuple<Last>;
    };

    template<typename First, typename... Rest>
    struct unique_tuple_impl<First, Rest...> {
        using type = std::conditional_t<
            type_in_tuple_v<First, std::tuple<Rest...> >,
            typename unique_tuple_impl<Rest...>::type,
            decltype(
                std::tuple_cat(
                    std::tuple<First>{},
                    std::declval<typename unique_tuple_impl<Rest...>::type>()))>;
    };

    template<typename... Types>
    using unique_tuple_impl_t = typename unique_tuple_impl<Types...>::type;


    export template<typename TupleType>
    struct unique_tuple_from_tuple {
        static_assert(false, "Type template parameter is not a tuple type");
    };

    export template<typename... Types>
    struct unique_tuple_from_tuple<std::tuple<Types...> > {
        using type = unique_tuple_impl_t<Types...>;
    };

    export template<typename... Types>
    using unique_tuple_from_tuple_t = typename unique_tuple_from_tuple<std::tuple<Types...> >::type;

    export template<typename... Types>
    using unique_tuple_t = unique_tuple_impl_t<Types...>;

    export template<typename TupleType>
    struct unique_variant {
        static_assert(false, "Type template parameter is not a variant type");
    };

    template<typename... Types>
    struct unique_variant_impl {
        using unique_tuple_type = unique_tuple_impl_t<Types...>;
        using type = decltype([]<typename... Ts>(std::tuple<Ts...>) {
            return std::variant<Ts...>{};
        }(unique_tuple_type{}));
    };

    template<typename... Types>
    using unique_variant_impl_t = typename unique_variant_impl<Types...>::type;

    template<typename TupleType>
    struct unique_variant_from_tuple {
        static_assert(false, "Type template parameter is not a tuple type");
    };

    template<typename... Types>
    struct unique_variant_from_tuple<std::tuple<Types...> > {
        using type = unique_variant_impl_t<Types...>;
    };

    export template<typename... Types>
    using unique_variant_from_tuple_t = typename unique_variant_from_tuple<std::tuple<Types...> >::type;

    export template<typename... Types>
    using unique_variant_t = unique_variant_impl_t<Types...>;

    export template<typename... Fns>
    struct overload_fns : public Fns... {
        overload_fns(const Fns &...) {}
    };

    export template<typename Container_Type, typename Value_Type>
    struct copy_universal_reference {
        static_assert(false, "Container_Type is not a reference type");
    };

    export template<typename Container_Type, typename Value_Type>
    struct copy_universal_reference<Container_Type &, Value_Type> {
        using type = std::remove_cvref_t<Value_Type> &;
    };

    export template<typename Container_Type, typename Value_Type>
    struct copy_universal_reference<const Container_Type &, Value_Type> {
        using type = const std::remove_cvref_t<Value_Type> &;
    };

    export template<typename Container_Type, typename Value_Type>
    struct copy_universal_reference<Container_Type &&, Value_Type> {
        using type = std::remove_cvref_t<Value_Type> &&;
    };

    export template<typename Container_Type, typename Value_Type>
    struct copy_universal_reference<const Container_Type &&, Value_Type> {
        using type = const std::remove_cvref_t<Value_Type> &&;
    };

    export template<typename Container_Type, typename Value_Type>
    using copy_universal_reference_t = typename copy_universal_reference<Container_Type, Value_Type>::type;

    export template<typename Container_Type, typename Value_Type>
    decltype(auto) forward_value_based_on_container(Value_Type &&value) {
        using forward_type = copy_universal_reference_t<Container_Type, Value_Type>;
        return std::forward<forward_type>(value);
    }

    /*template<typename Target_Value_Type, typename Template_Type>
    struct fill_template {
        static_assert(false, "Cannot find the mapped value type.");
    };

    template<typename Target_Value_Type, template<typename...> typename Template_Type, typename First_Type, typename... Rest_Types>
    struct fill_template<Target_Value_Type, Template_Type<First_Type, Rest_Types...>> {
        using type = Template_Type<Target_Value_Type>;
    };*/

    export template<typename Target_Type, typename Original_Type, typename Container_Type>
    struct exchange_template_type {
        static_assert(false, "Cannot find the mapped value type.");
    };

    export template<typename Target_Type, typename Original_Type>
    struct exchange_template_type<Target_Type, Original_Type, Original_Type> {
        using type = Target_Type;
    };

    export template<typename Target_Type, typename Original_Type, template<typename...> typename Container_Type,
        typename... Types>
        requires (!std::is_same_v<Container_Type<Types...>, Original_Type>)
    struct exchange_template_type<Target_Type, Original_Type, Container_Type<Types...> > {
        using type = Container_Type<typename exchange_template_type<Target_Type, Original_Type, Types>::type...>;
    };

    export template<typename Target_Type, typename Original_Type, typename Container_Type>
    using exchange_template_type_t = typename exchange_template_type<Target_Type, Original_Type, Container_Type>::type;
}
