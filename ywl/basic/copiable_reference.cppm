module;

#include <stdc++pch.hpp>

export module ywl.basic.copiable_reference;

namespace ywl::basic {
    template<typename Base, typename Derived>
    concept base_of = std::is_base_of_v<Base, Derived> || std::is_same_v<Derived, Base>;

    template<typename Derived, typename Base>
    concept derived_from = std::is_base_of_v<Base, Derived> || std::is_same_v<Derived, Base>;

    template<typename T>
    class copiable_reference_base {
    public:
        using value_type = T;
        using pointer_type = T *;
        using reference_type = T &;
        using const_pointer_type = const T *;
        using const_reference_type = const T &;
        using rvalue_type = T &&;
        using const_rvalue_type = const T &&;
        using copy_constructor_type = void* (*)(void *);
        using deleter_type = void (*)(void *);

    public:
        void *m_data{};
        uintptr_t m_t_to_void_offset{};
        copy_constructor_type m_copy_constructor{};
        deleter_type m_deleter{};

    public:
        constexpr copiable_reference_base() = default;

        constexpr copiable_reference_base(
            void *data,
            uintptr_t t_to_void_offset,
            copy_constructor_type copy_constructor,
            deleter_type deleter)
            : m_data(data),
              m_t_to_void_offset(t_to_void_offset),
              m_copy_constructor(copy_constructor),
              m_deleter(deleter) {
        }

        template<typename... Args>
        constexpr static copiable_reference_base create_reference(Args &&... args) {
            auto data = static_cast<void *>(new T{std::forward<Args>(args)...});
            struct copy_constructor {
                static void * copy(void *ptr) {
                    return new T{*static_cast<T *>(ptr)};
                }
            };

            struct deleter {
                static void destroy(void *ptr) {
                    delete static_cast<T *>(ptr);
                }
            };

            return copiable_reference_base{
                data,
                0,
                &copy_constructor::copy,
                &deleter::destroy
            };
        }

    private:
        template<base_of<T> Base>
        [[nodiscard]] constexpr uintptr_t get_base_to_void_offset() const {
            if constexpr (std::is_same_v<Base, T>) {
                return m_t_to_void_offset;
            } else {
                const T *data = this->cast();
                const Base *base = static_cast<const Base *>(data);
                uintptr_t offset = reinterpret_cast<uintptr_t>(base) - reinterpret_cast<uintptr_t>(data);
                return offset + m_t_to_void_offset;
            }
        }

        template<derived_from<T> Derived>
        [[nodiscard]] constexpr uintptr_t get_derived_to_void_offset() const {
            if constexpr (std::is_same_v<Derived, T>) {
                return m_t_to_void_offset;
            } else {
                const T *data = this->cast();
                const auto *derived = static_cast<const Derived *>(data);
                uintptr_t offset = reinterpret_cast<uintptr_t>(derived) - reinterpret_cast<uintptr_t>(data);
                return offset + m_t_to_void_offset;
            }
        }

    public:
        constexpr pointer_type cast() {
            T *data = reinterpret_cast<T *>(reinterpret_cast<uintptr_t>(m_data) + m_t_to_void_offset);
            return data;
        }

        constexpr const_pointer_type cast() const {
            const T *data = reinterpret_cast<const T *>(reinterpret_cast<uintptr_t>(m_data) + m_t_to_void_offset);
            return data;
        }

        constexpr void reset() {
            if (m_data) {
                assert(m_copy_constructor && m_deleter);
                m_deleter(m_data);
                m_data = nullptr;
                m_copy_constructor = nullptr;
                m_deleter = nullptr;
            }
        }

        [[nodiscard]] constexpr void *clone_ptr() const {
            return m_copy_constructor(m_data);
        }

    public:
        template<typename>
        friend class copiable_reference_base;

    public:
        constexpr copiable_reference_base(const copiable_reference_base &base) : m_data(base.clone_ptr()),
            m_t_to_void_offset(base.m_t_to_void_offset),
            m_copy_constructor(base.m_copy_constructor),
            m_deleter(base.m_deleter) {
        }

        constexpr copiable_reference_base(copiable_reference_base &&base) noexcept
            : m_data(std::exchange(base.m_data, nullptr)),
              m_t_to_void_offset(
                  std::exchange(
                      base.m_t_to_void_offset, 0)),
              m_copy_constructor(
                  std::exchange(
                      base.m_copy_constructor, nullptr)),
              m_deleter(std::exchange(
                  base.m_deleter, nullptr)) {
        }

        constexpr copiable_reference_base &operator=(const copiable_reference_base &base) {
            if (this != &base) {
                reset();
                m_data = base.clone_ptr();
                m_t_to_void_offset = base.m_t_to_void_offset;
                m_copy_constructor = base.m_copy_constructor;
                m_deleter = base.m_deleter;
            }
            return *this;
        }

        constexpr copiable_reference_base &operator=(copiable_reference_base &&base) noexcept {
            if (this != &base) {
                reset();
                m_data = std::exchange(base.m_data, nullptr);
                m_t_to_void_offset = std::exchange(base.m_t_to_void_offset, 0);
                m_copy_constructor = std::exchange(base.m_copy_constructor, nullptr);
                m_deleter = std::exchange(base.m_deleter, nullptr);
            }
            return *this;
        }

        template<derived_from<T> Derived>
        constexpr copiable_reference_base(const copiable_reference_base<Derived> &base)
            : m_data(base.clone_ptr()),
              m_t_to_void_offset(base.template get_base_to_void_offset<T>()),
              m_copy_constructor(base.m_copy_constructor),
              m_deleter(base.m_deleter) {
        }

        template<derived_from<T> Derived>
        constexpr copiable_reference_base(copiable_reference_base<Derived> &&base) noexcept
            : m_data(std::exchange(base.m_data, nullptr)),
              m_t_to_void_offset(base.template get_base_to_void_offset<T>()),
              m_copy_constructor(std::exchange(base.m_copy_constructor, nullptr)),
              m_deleter(std::exchange(base.m_deleter, nullptr)) {
            base.m_t_to_void_offset = 0;
        }

        template<derived_from<T> Derived>
        constexpr copiable_reference_base<Derived> static_clone_as() const {
            return {
                this->clone_ptr(),
                this->get_derived_to_void_offset<Derived>(),
                this->m_copy_constructor,
                this->m_deleter
            };
        }

        template<derived_from<T> Derived>
        constexpr copiable_reference_base<Derived> static_take_as() {
            uintptr_t offset = this->get_derived_to_void_offset<Derived>();
            this->m_t_to_void_offset = 0;
            return {
                std::exchange(this->m_data, nullptr),
                offset,
                std::exchange(this->m_copy_constructor, nullptr),
                std::exchange(this->m_deleter, nullptr)
            };
        }

        template<derived_from<T> Derived>
        constexpr copiable_reference_base<Derived> dyn_clone_as() const {
            if (dynamic_cast<const Derived *>(this->cast())) {
                return this->static_clone_as<Derived>();
            }
            return {};
        }

        template<derived_from<T> Derived>
        constexpr copiable_reference_base<Derived> dyn_take_as() {
            if (dynamic_cast<Derived *>(this->cast())) {
                return this->static_take_as<Derived>();
            }
            return {};
        }

        template<derived_from<T> Derived>
        constexpr copiable_reference_base &operator=(const copiable_reference_base<Derived> &base) {
            if (this != &base) {
                reset();
                m_data = base.clone_ptr();
                m_t_to_void_offset = base.template get_base_to_void_offset<T>();
                m_copy_constructor = base.m_copy_constructor;
                m_deleter = base.m_deleter;
            }
            return *this;
        }

        template<derived_from<T> Derived>
        constexpr copiable_reference_base &operator=(copiable_reference_base<Derived> &&base) noexcept {
            if (this != &base) {
                reset();
                m_data = std::exchange(base.m_data, nullptr);
                m_t_to_void_offset = base.template get_base_to_void_offset<T>();
                base.m_t_to_void_offset = 0;
                m_copy_constructor = std::exchange(base.m_copy_constructor, nullptr);
                m_deleter = std::exchange(base.m_deleter, nullptr);
            }
            return *this;
        }
    };

    export template<typename T>
    class copiable_reference;

    export template<typename Tp, typename... Args>
    constexpr copiable_reference<Tp> make_reference(Args &&... args);

    template<typename T>
    class copiable_reference {
    public:
        using value_type = T;
        using pointer_type = T *;
        using reference_type = T &;
        using const_pointer_type = const T *;
        using const_reference_type = const T &;

    private:
        copiable_reference_base<T> m_base;

    public:
        constexpr void reset() {
            m_base.reset();
        }

        constexpr copiable_reference() = default;

        // private:
        // template<typename Tp, typename... Args>
        // friend constexpr copiable_reference<Tp> make_reference(Args &&... args);

        constexpr explicit copiable_reference(copiable_reference_base<T> &&base) noexcept
            : m_base(std::move(base)) {
        }

    public:
        constexpr copiable_reference clone(this const copiable_reference &self) {
            return self;
        }

        constexpr copiable_reference take(this copiable_reference &self) {
            return std::move(self);
        }

        template<base_of<T> Base = T>
        constexpr Base &as_ref() {
            return *static_cast<Base *>(m_base.cast());
        }

        template<base_of<T> Base = T>
        constexpr const Base &as_ref() const {
            return *static_cast<const Base *>(m_base.cast());
        }

        constexpr T *get() {
            return m_base.cast();
        }

        constexpr T *get() const {
            return m_base.cast();
        }

        constexpr T &operator*() {
            return *m_base.cast();
        }

        constexpr const T &operator*() const {
            return *m_base.cast();
        }

        constexpr T *operator->() {
            return m_base.cast();
        }

        constexpr const T *operator->() const {
            return m_base.cast();
        }

        constexpr copiable_reference(const copiable_reference &other) noexcept
            : m_base(other.m_base) {
        }

        constexpr copiable_reference(copiable_reference &&other) noexcept
            : m_base(std::move(other.m_base)) {
        }

        constexpr copiable_reference &operator=(const copiable_reference &other) {
            if (this != &other) {
                m_base = other.m_base;
            }
            return *this;
        }

        constexpr copiable_reference &operator=(copiable_reference &&other) noexcept {
            if (this != &other) {
                m_base = std::move(other.m_base);
            }
            return *this;
        }

    public:
        constexpr ~copiable_reference() {
            reset();
        }

        template<typename Base>
        friend class copiable_reference;

        // move constructor, which also receives any derived class
        template<derived_from<T> Derived>
        constexpr copiable_reference(copiable_reference<Derived> &&other) : // NOLINT
            m_base{
                std::move(other.m_base)
            } {
        }

        // copy constructor, which also receives any derived class
        template<derived_from<T> Derived>
        constexpr copiable_reference(const copiable_reference<Derived> &other) noexcept : // NOLINT
            m_base{
                other.m_base
            } {
        }

        // move assignment operator, which also receives any derived class
        template<derived_from<T> Derived>
        constexpr copiable_reference &operator=(copiable_reference<Derived> &&other) noexcept {
            if (this != &other) {
                this->m_base = std::move(other.m_base);
            }
            return *this;
        }

        // copy assignment operator, which also receives any derived class
        template<derived_from<T> Derived>
        constexpr copiable_reference &operator=(const copiable_reference<Derived> &other) noexcept {
            if (this != &other) {
                this->m_base = other.m_base;
            }
            return *this;
        }

    public:
        // dynamic cast
        template<derived_from<T> Derived>
        constexpr Derived *dyn_as() {
            if constexpr (std::is_same_v<Derived, T>) {
                return m_base.cast();
            } else {
                return dynamic_cast<Derived *>(m_base.cast());
            }
        }

        template<derived_from<T> Derived>
        constexpr const Derived *dyn_as() const {
            if constexpr (std::is_same_v<Derived, T>) {
                return m_base.cast();
            } else {
                return dynamic_cast<Derived *>(m_base.cast());
            }
        }

        template<derived_from<T> Derived>
        constexpr Derived *static_as() {
            if constexpr (std::is_same_v<Derived, T>) {
                return m_base.cast();
            } else {
                return static_cast<Derived *>(m_base.cast());
            }
        }

        template<derived_from<T> Derived>
        constexpr const Derived *static_as() const {
            if constexpr (std::is_same_v<Derived, T>) {
                return m_base.cast();
            } else {
                return static_cast<Derived *>(m_base.cast());
            }
        }

        // static clone, take
        template<derived_from<T> Derived>
        constexpr copiable_reference<Derived> static_clone_as() const {
            return copiable_reference<Derived>{
                m_base.template static_clone_as<Derived>()
            };
        }

        template<derived_from<T> Derived>
        constexpr copiable_reference<Derived> static_take_as() {
            return copiable_reference<Derived>{
                m_base.template static_take_as<Derived>()
            };
        }

        template<derived_from<T> Derived>
        constexpr copiable_reference<Derived> dyn_clone_as() const {
            return copiable_reference<Derived>{
                m_base.template dyn_clone_as<Derived>()
            };
        }

        template<derived_from<T> Derived>
        constexpr copiable_reference<Derived> dyn_take_as() {
            return copiable_reference<Derived>{
                m_base.template dyn_take_as<Derived>()
            };
        }
    };

    template<typename Tp, typename... Args>
    constexpr copiable_reference<Tp> make_reference(Args &&... args) {
        return copiable_reference<Tp>{
            copiable_reference_base<Tp>::create_reference(std::forward<Args>(args)...)
        };
    }
}
