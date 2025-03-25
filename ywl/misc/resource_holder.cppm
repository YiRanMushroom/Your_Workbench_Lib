export module ywl.misc.resource_holder;

import ywl.std.prelude;
import ywl.basic.exceptions;
import ywl.basic.helpers;

namespace ywl::misc {
    /*export template<typename T>
    concept is_unique_resource_holder_hint_type = requires
    {
        typename T::holder_storage_type;
        typename T::holder_value_type;
        requires std::is_trivial_v<typename T::holder_value_type>;
        requires std::is_default_constructible_v<typename T::holder_value_type>;

        { T::destroy_resource(std::declval<typename T::holder_storage_type &&>()) } -> std::same_as<void>;
        // destroy_resource could be called multiple times, it should check if the storage is valid.

        {
            T::move_storage(std::declval<typename T::holder_storage_type &>())
        } -> std::convertible_to<typename T::holder_storage_type>;

        {
            T::create_storage_from_resource(std::declval<typename T::holder_value_type &&>())
        } -> std::convertible_to<typename T::holder_storage_type>;

        {
            T::get_resource_from_storage(std::declval<const typename T::holder_storage_type &>())
        } -> std::same_as<typename T::holder_value_type>;

        {
            T::release_resource_from_storage(std::declval<typename T::holder_storage_type &&>())
        } -> std::same_as<typename T::holder_value_type>;

        {
            T::has_resource(std::declval<const typename T::holder_storage_type &>())
        } -> std::convertible_to<bool>;

        requires std::is_move_assignable_v<typename T::holder_storage_type>;
    };


    export template<typename T, typename enable = void>
    class unique_holder {
    };

    export template<typename T>
    class unique_holder<T, std::enable_if_t<is_unique_resource_holder_hint_type<T> > > {
    public:
        using storage_type = typename T::holder_storage_type;
        using value_type = typename T::holder_value_type;

    private:
        storage_type storage;

    public:
        constexpr unique_holder() = default;

        constexpr unique_holder(const unique_holder &) = delete;

        constexpr unique_holder &operator=(const unique_holder &) = delete;

        constexpr unique_holder(unique_holder &&other) noexcept : storage(T::move_storage(other.storage)) {
        }

        constexpr explicit unique_holder(value_type &&value) : storage(
            T::create_storage_from_resource(std::move(value))) {
        }

    private:
        constexpr explicit unique_holder(storage_type &&storage) : storage(T::move_storage(storage)) {
        }

    public:
        constexpr unique_holder &operator=(unique_holder &&other) noexcept {
            T::destroy_resource(T::move_storage(storage));
            storage = T::move_storage(other.storage);
            return *this;
        }

        constexpr value_type get() const {
            return T::get_resource_from_storage(storage);
        }

        constexpr value_type operator*() const {
            return get();
        }

        constexpr operator value_type() const { // NOLINT
            return get();
        }

        constexpr value_type release() {
            return T::release_resource_from_storage(T::move_storage(storage));
        }

        constexpr void reset() {
            if (has_value())
                T::destroy_resource(T::release_resource_from_storage(T::move_storage(storage)));
        }

        [[nodiscard]] constexpr bool has_value() const {
            return T::has_resource(storage);
        }

        constexpr operator bool() const { // NOLINT
            return has_value();
        }

        template<typename... Args>
        constexpr static unique_holder create(Args &&... args) {
            return unique_holder{T::create_storage_from_resource(T::create(std::forward<Args>(args)...))};
        }

        constexpr ~unique_holder() {
            T::destroy_resource(T::move_storage(storage));
        }
    };

    template<typename T>
    concept is_unique_resource_holder_identity_hint_type = requires
    {
        typename T::value_type;

        requires std::is_trivial_v<typename T::value_type>;

        {
            T::destroy_value(std::declval<typename T::value_type &&>())
        } -> std::same_as<void>;

        {
            T::move_value(std::declval<typename T::value_type &>())
        } -> std::convertible_to<typename T::value_type>;
    };

    export template<typename T>
    class unique_holder<T, std::enable_if_t<is_unique_resource_holder_identity_hint_type<T> > > {
    public:
        using value_type = typename T::value_type;

    private:
        value_type value{};

    public:
        constexpr unique_holder() = default;

        constexpr unique_holder(const unique_holder &) = delete;

        constexpr unique_holder &operator=(const unique_holder &) = delete;

        constexpr unique_holder(unique_holder &&other) noexcept : value(T::move_value(other.value)) {
        }

        constexpr explicit unique_holder(value_type &&value) : value(T::move_value(value)) {
        }

    public:
        constexpr unique_holder &operator=(unique_holder &&other) noexcept {
            T::destroy_value(T::move_value(value));
            value = T::move_value(other.value);
            return *this;
        }

        constexpr value_type get() const {
            return value;
        }

        constexpr value_type operator*() const {
            return get();
        }

        constexpr operator value_type() const { // NOLINT
            return get();
        }

        constexpr value_type release() {
            return T::move_value(value);
        }

        constexpr void reset() {
            if (has_value())
                T::destroy_value(T::move_value(value));
        }

        [[nodiscard]] constexpr bool has_value() const {
            if constexpr (requires { T::default_value; }) {
                return value != T::default_value;
            } else {
                return value != value_type{};
            }
        }

        constexpr operator bool() const { // NOLINT
            return has_value();
        }

        template<typename... Args>
        constexpr static unique_holder create(Args &&... args) {
            return unique_holder{T::create(std::forward<Args>(args)...)};
        }

        constexpr ~unique_holder() {
            reset();
        }
    };

    export template<typename T, typename construct, typename destruct> requires std::is_trivial_v<T>
    struct unique_hint_base_optional {
        using holder_storage_type = std::optional<T>;
        using holder_value_type = T;

        constexpr static void destroy_resource(holder_storage_type &&storage) {
            if (!storage)
                return;
            destruct{}(*storage);
            storage.reset();
        }

        constexpr static holder_storage_type move_storage(holder_storage_type &storage) {
            holder_storage_type new_storage = storage;
            storage.reset();
            return new_storage;
        }

        constexpr static holder_storage_type create_storage_from_resource(holder_value_type &&value) {
            return {value};
        }

        constexpr static holder_value_type get_resource_from_storage(const holder_storage_type &storage) {
            return *storage;
        }

        constexpr static holder_value_type release_resource_from_storage(holder_storage_type &&storage) {
            holder_value_type value = *storage;
            storage.reset();
            return value;
        }

        constexpr static bool has_resource(const holder_storage_type &storage) {
            return storage.has_value();
        }

        template<typename... Args>
        constexpr static holder_value_type create(Args &&... args) { // NOLINT
            return construct{}(std::forward<Args>(args)...);
        }
    };

    export template<typename T, typename construct, typename destruct, T default_v = T{}>
        requires std::is_trivial_v<T>
    struct unique_hint_base_default {
        using value_type = T;

        constexpr static value_type default_value = default_v;

        constexpr static void destroy_value(value_type &&value) {
            destruct{}(value);
        }

        constexpr static value_type move_value(value_type &value) {
            value_type new_value = value;
            value = default_value;
            return new_value;
        }

        template<typename... Args>
        constexpr static value_type create(Args &&... args) { // NOLINT
            return construct{}(std::forward<Args>(args)...);
        }
    };

    export template<typename T>
    concept is_shared_resource_holder_hint_type = requires
    {
        typename T::holder_value_type;

        requires std::is_trivial_v<typename T::holder_value_type>;

        {
            T::destroy_resource(std::declval<typename T::holder_value_type &&>())
        } -> std::same_as<void>;
    };

    namespace impl {
        template<is_shared_resource_holder_hint_type T>
        class shared_resource_control_block {
            std::atomic<size_t> strong_count;
            std::atomic<size_t> weak_count;

            using value_type = typename T::holder_value_type;

            value_type resource;

            constexpr explicit shared_resource_control_block(auto &&... args)
                : strong_count{1},
                  weak_count{1},
                  resource{
                      T::create(std::forward<decltype(args)>(args)...)
                  } {
            }

        public:
            constexpr shared_resource_control_block() = delete;

            constexpr shared_resource_control_block(const shared_resource_control_block &) = delete;

            constexpr shared_resource_control_block(shared_resource_control_block &&) = delete;

            constexpr shared_resource_control_block &operator=(const shared_resource_control_block &) = delete;

            constexpr shared_resource_control_block &operator=(shared_resource_control_block &&) = delete;

            constexpr shared_resource_control_block *provide_shared() {
                size_t original_strong = strong_count.load(std::memory_order_relaxed);
                while (original_strong > 0) {
                    if (strong_count.compare_exchange_weak(original_strong,
                                                           original_strong + 1,
                                                           std::memory_order_relaxed)) {
                        return provide_weak();
                    }
                }
                return nullptr;
            }

            constexpr shared_resource_control_block *provide_weak() {
                size_t original_weak = weak_count.fetch_add(1, std::memory_order_relaxed);
                if (original_weak == 0) {
                    throw basic::ywl_impl_error{
                        std::format("implementation in shared_resource_control_block is wrong, "
                                    "resource or control block was deleted before weak creation, weak_count: {}",
                                    original_weak)
                    };
                }
                return this;
            }

            constexpr void reduce_shared() {
                size_t original_strong = strong_count.fetch_sub(1, std::memory_order_relaxed);
                if (original_strong == 0) {
                    throw basic::ywl_impl_error{
                        std::format("implementation in shared_resource_control_block is wrong, "
                                    "resource was deleted before shared reduction, strong_count: {}",
                                    original_strong)
                    };
                }

                if (original_strong == 1) {
                    T::destroy_resource(std::move(resource));
                }

                reduce_weak();
            }

            constexpr void reduce_weak() {
                size_t original_weak = weak_count.fetch_sub(1, std::memory_order_relaxed);
                if (original_weak == 0) {
                    throw basic::ywl_impl_error{
                        std::format("implementation in shared_resource_control_block is wrong, "
                                    "resource was deleted before weak reduction, weak_count: {}",
                                    original_weak)
                    };
                }

                if (original_weak == 1) {
                    delete this;
                }
            }

            constexpr bool has_value() const {
                return strong_count.load(std::memory_order_relaxed) > 0;
            }

            constexpr value_type get() const {
                return resource;
            }

            constexpr static shared_resource_control_block *create(auto &&... args) {
                return new shared_resource_control_block{std::forward<decltype(args)>(args)...};
            }
        };
    }

    export template<is_shared_resource_holder_hint_type T>
    class weak_holder;

    export template<is_shared_resource_holder_hint_type T>
    class shared_holder {
        using control_block_type = impl::shared_resource_control_block<T>;
        control_block_type *control_block = nullptr;

    public:
        using value_type = typename T::holder_value_type;
        using weak_type = weak_holder<T>;

        friend class weak_holder<T>;

        constexpr shared_holder() = default;

        constexpr shared_holder(const shared_holder &other) : control_block{} {
            if (other.control_block) {
                control_block = other.control_block->provide_shared();
            }
        }

        constexpr shared_holder(shared_holder &&other) noexcept : control_block{other.control_block} {
            other.control_block = nullptr;
        }

        constexpr static shared_holder create(auto &&... args) {
            shared_holder holder;
            holder.control_block = control_block_type::create(std::forward<decltype(args)>(args)...);
            return holder;
        }

        constexpr void reset() {
            if (control_block) {
                control_block->reduce_shared();
                control_block = nullptr;
            }
        }

        constexpr ~shared_holder() {
            reset();
        }

        constexpr shared_holder &operator=(const shared_holder &other) {
            if (this == &other) {
                return *this;
            }

            reset();

            if (other.control_block) {
                control_block = other.control_block->provide_shared();
            }

            return *this;
        }

        constexpr shared_holder &operator=(shared_holder &&other) noexcept {
            if (this == &other) {
                return *this;
            }

            reset();

            control_block = other.control_block;
            other.control_block = nullptr;

            return *this;
        }

        [[nodiscard]] constexpr bool has_value() const {
            return control_block != nullptr;
        }

        constexpr operator bool() const { // NOLINT
            return has_value();
        }

        constexpr value_type get() const {
            if (*this) {
                return control_block->get();
            }
            throw basic::ywl_impl_error{"Cannot access a shared_holder with no value"};
        }

        constexpr value_type operator*() const {
            return get();
        }

        constexpr operator value_type() const { // NOLINT
            return get();
        }
    };

    export template<is_shared_resource_holder_hint_type T>
    class weak_holder {
        using control_block_type = impl::shared_resource_control_block<T>;
        control_block_type *control_block = nullptr;

    public:
        using value_type = typename T::holder_value_type;

        constexpr weak_holder() = default;

        constexpr weak_holder(const shared_holder<T> &shared) : control_block{} { // NOLINT
            if (shared.control_block) {
                control_block = shared.control_block->provide_weak();
            }
        }

        constexpr weak_holder(const weak_holder &other) : control_block{} {
            if (other.control_block) {
                control_block = other.control_block->provide_weak();
            }
        }

        constexpr weak_holder(weak_holder &&other) noexcept : control_block{other.control_block} {
            other.control_block = nullptr;
        }

        constexpr void reset() {
            if (control_block) {
                control_block->reduce_weak();
                control_block = nullptr;
            }
        }

        constexpr ~weak_holder() {
            reset();
        }

        constexpr shared_holder<T> lock() const {
            shared_holder<T> holder;
            holder.control_block = control_block->provide_shared();
            return holder;
        }
    };*/

    export template<typename T, typename DeleteType = basic::nothing_fn_t, T default_value = T{}>
    class unique_owner_default {
    public:
        using value_type = T;
        using delete_type = DeleteType;

        constexpr unique_owner_default() : value{default_value} {}

        constexpr static unique_owner_default from_exchanged(value_type &&value) {
            return unique_owner_default{std::exchange(value, default_value)};
        }

        constexpr static unique_owner_default from_value(const value_type &value) {
            return unique_owner_default{value};
        }

        constexpr unique_owner_default(const unique_owner_default &) = delete;

        constexpr unique_owner_default(unique_owner_default &&other) noexcept : value{other.value} {
            other.value = default_value;
        }

        constexpr unique_owner_default &operator=(const unique_owner_default &) = delete;

        constexpr unique_owner_default &operator=(unique_owner_default &&other) noexcept {
            if (this == &other) {
                return *this;
            }

            this->reset();

            this->value = std::exchange(other.value, default_value);

            return *this;
        }

        constexpr auto operator<=>(const unique_owner_default &) const = default;

        constexpr const value_type &get() const {
            return value;
        }

        constexpr const value_type &operator*() const {
            return value;
        }

        constexpr value_type &get() {
            return value;
        }

        constexpr value_type &operator*() {
            return value;
        }

        [[nodiscard]] constexpr bool has_value() const {
            return value != default_value;
        }

        constexpr operator bool() const { // NOLINT
            return has_value();
        }

        constexpr void reset() {
            if constexpr (!(std::is_same_v<delete_type, basic::nothing_fn_t> || std::is_same_v<delete_type, void>
                            || std::is_same_v<delete_type, nullptr_t>)) {
                if (value != default_value) {
                    delete_type{}(std::exchange(value, default_value));
                }
            }
        }

        constexpr ~unique_owner_default() {
            reset();
        }

        constexpr unique_owner_default(value_type value) : value{value} {}

    private:
        value_type value;
    };

    export template<typename T, typename DeleteType = basic::nothing_fn_t>
    class unique_owner_optional {
    public:
        using value_type = T;
        using delete_type = DeleteType;

        constexpr unique_owner_optional() = default;

        constexpr static unique_owner_optional from_exchanged(value_type &&value) {
            return unique_owner_optional{std::exchange(value, std::nullopt)};
        }

        constexpr static unique_owner_optional from_value(const value_type &value) {
            return unique_owner_optional{value};
        }

        constexpr unique_owner_optional(const unique_owner_optional &) = delete;

        constexpr unique_owner_optional(unique_owner_optional &&other) noexcept : value{
            std::exchange(other.value, std::optional<value_type>{})
        } {}

        constexpr unique_owner_optional &operator=(const unique_owner_optional &) = delete;

        constexpr unique_owner_optional &operator=(unique_owner_optional &&other) noexcept {
            if (this == &other) {
                return *this;
            }

            this->reset();

            value = std::exchange(other.value, std::optional<value_type>{});

            return *this;
        }

        constexpr auto operator<=>(const unique_owner_optional &) const = default;

        constexpr const value_type &get() const {
            return *value;
        }

        constexpr const value_type &operator*() const {
            return *value;
        }

        constexpr bool has_value() const {
            return value.has_value();
        }

        constexpr operator bool() const {
            return has_value();
        }

        constexpr void reset() {
            if (value.has_value()) {
                if constexpr (!(std::is_same_v<delete_type, basic::nothing_fn_t> || std::is_same_v<delete_type, void>
                                || std::is_same_v<delete_type, nullptr_t>)) {
                    delete_type{}(*std::exchange(value, std::optional<value_type>{}));
                }
            }
        }

        constexpr unique_owner_optional(value_type value) : value{value} {}

    private:
        std::optional<value_type> value;
    };

    export template<typename T>
    struct inject_unique_holder_visible {
        constexpr decltype(auto) get() const {
            return static_cast<const T *>(this)->unique_holder_instance().get();
        }

        constexpr bool has_value() const {
            return static_cast<const T *>(this)->unique_holder_instance().has_value();
        }

        void reset() {
            static_cast<T *>(this)->unique_holder_instance().reset();
        }
    };
}
