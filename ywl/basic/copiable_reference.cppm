module;

#include <cassert>

export module ywl.basic.copiable_reference;

import ywl.std.prelude;

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
        copiable_reference_base() = default;

        copiable_reference_base(
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
        static copiable_reference_base create_reference(Args &&... args) {
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

        pointer_type cast() {
            T *data = reinterpret_cast<T *>(reinterpret_cast<uintptr_t>(m_data) + m_t_to_void_offset);
            return data;
        }

        const_pointer_type cast() const {
            const T *data = reinterpret_cast<const T *>(reinterpret_cast<uintptr_t>(m_data) + m_t_to_void_offset);
            return data;
        }

        void reset() {
            if (m_data) {
                assert(m_copy_constructor && m_deleter);
                m_deleter(m_data);
                m_data = nullptr;
                m_copy_constructor = nullptr;
                m_deleter = nullptr;
            }
        }

        [[nodiscard]] void *clone_ptr() const {
            return m_copy_constructor(m_data);
        }

    public:
        template<typename>
        friend class copiable_reference_base;

    private:
        template<derived_from<T> Derived>
        static uintptr_t get_derived_to_t_offset() {
            if constexpr (std::is_same_v<Derived, T>) {
                return 0;
            } else {
                Derived *derived_ptr = nullptr;
                T *t_ptr = static_cast<T *>(derived_ptr);
                return reinterpret_cast<uintptr_t>(derived_ptr) - reinterpret_cast<uintptr_t>(t_ptr);
            }
        }

    public:
        copiable_reference_base(const copiable_reference_base &base) :
            m_data(base.clone_ptr()),
            m_t_to_void_offset(base.m_t_to_void_offset),
            m_copy_constructor(base.m_copy_constructor),
            m_deleter(base.m_deleter) {
            std::cout << "copiable_reference_base copy constructor" << std::endl;
        }

        copiable_reference_base(copiable_reference_base &&base) noexcept
            : m_data(std::exchange(base.m_data, nullptr)),
              m_t_to_void_offset(
                  std::exchange(
                      base.m_t_to_void_offset, 0)),
              m_copy_constructor(
                  std::exchange(
                      base.m_copy_constructor, nullptr)),
              m_deleter(std::exchange(
                  base.m_deleter, nullptr)) {
            std::cout << "copiable_reference_base move constructor" << std::endl;
        }

        copiable_reference_base &operator=(const copiable_reference_base &base) {
            std::cout << "copiable_reference_base copy assignment" << std::endl;
            if (this != &base) {
                reset();
                m_data = base.clone_ptr();
                m_t_to_void_offset = base.m_t_to_void_offset;
                m_copy_constructor = base.m_copy_constructor;
                m_deleter = base.m_deleter;
            }
            return *this;
        }

        copiable_reference_base &operator=(copiable_reference_base &&base) noexcept {
            std::cout << "copiable_reference_base move assignment" << std::endl;
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
        copiable_reference_base(const copiable_reference_base<Derived> &base)
            : m_data(base.clone_ptr()),
              m_t_to_void_offset(get_derived_to_t_offset<Derived>() + base.m_t_to_void_offset),
              m_copy_constructor(base.m_copy_constructor),
              m_deleter(base.m_deleter) {
            std::cout << "copiable_reference_base copy constructor" << std::endl;
        }

        template<derived_from<T> Derived>
        copiable_reference_base(copiable_reference_base<Derived> &&base) noexcept
            : m_data(std::exchange(base.m_data, nullptr)),
              m_t_to_void_offset(get_derived_to_t_offset<Derived>()
                                 + std::exchange(base.m_t_to_void_offset, 0)),
              m_copy_constructor(std::exchange(base.m_copy_constructor, nullptr)),
              m_deleter(std::exchange(base.m_deleter, nullptr)) {
            std::cout << "copiable_reference_base move constructor" << std::endl;
        }

        template<derived_from<T> Derived>
        copiable_reference_base &operator=(const copiable_reference_base<Derived> &base) {
            std::cout << "copiable_reference_base copy assignment" << std::endl;
            if (this != &base) {
                reset();
                m_data = base.clone_ptr();
                m_t_to_void_offset = get_derived_to_t_offset<Derived>() + base.m_t_to_void_offset;
                m_copy_constructor = base.m_copy_constructor;
                m_deleter = base.m_deleter;
            }
            return *this;
        }

        template<derived_from<T> Derived>
        copiable_reference_base &operator=(copiable_reference_base<Derived> &&base) noexcept {
            std::cout << "copiable_reference_base move assignment" << std::endl;
            if (this != &base) {
                reset();
                m_data = std::exchange(base.m_data, nullptr);
                m_t_to_void_offset = get_derived_to_t_offset<Derived>()
                                     + std::exchange(base.m_t_to_void_offset, 0);
                m_copy_constructor = std::exchange(base.m_copy_constructor, nullptr);
                m_deleter = std::exchange(base.m_deleter, nullptr);
            }
            return *this;
        }
    };

    export template<typename T>
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
        void reset() {
            m_base.reset();
        }

        copiable_reference() = default;

    public:
        template<typename Tp, typename... Args>
        friend copiable_reference<Tp> make_reference(Args &&... args);

        copiable_reference(copiable_reference_base<T> &&base) noexcept
            : m_base(std::move(base)) {
        }

        copiable_reference clone(this const copiable_reference &self) {
            return self;
        }

        copiable_reference take(this copiable_reference &self) {
            return std::move(self);
        }

        template<base_of<T> Base = T>
        Base &as_ref() {
            return *static_cast<Base *>(m_base.cast());
        }

        template<base_of<T> Base = T>
        const Base &as_ref() const {
            return *static_cast<const Base *>(m_base.cast());
        }

        T *get() {
            return m_base.cast();
        }

        T *get() const {
            return m_base.cast();
        }

        T &operator*() {
            return *m_base.cast();
        }

        const T &operator*() const {
            return *m_base.cast();
        }

        T *operator->() {
            return m_base.cast();
        }

        const T *operator->() const {
            return m_base.cast();
        }

        copiable_reference(const copiable_reference &other) noexcept
            : m_base(other.m_base) {
        }

        copiable_reference(copiable_reference &&other) noexcept
            : m_base(std::move(other.m_base)) {
        }

        copiable_reference &operator=(const copiable_reference &other) {
            if (this != &other) {
                m_base = other.m_base;
            }
            return *this;
        }

        copiable_reference &operator=(copiable_reference &&other) noexcept {
            if (this != &other) {
                m_base = std::move(other.m_base);
            }
            return *this;
        }

    public:
        ~copiable_reference() {
            reset();
        }

        template<typename Base>
        friend class copiable_reference;

        // move constructor, which also receives any derived class
        template<derived_from<T> Derived>
        copiable_reference(copiable_reference<Derived> &&other) : // NOLINT
            m_base{
                std::move(other.m_base)
            } {
        }

        // copy constructor, which also receives any derived class
        template<derived_from<T> Derived>
        copiable_reference(const copiable_reference<Derived> &other) noexcept : // NOLINT
            m_base{
                other.m_base
            } {
        }

        // move assignment operator, which also receives any derived class
        template<derived_from<T> Derived>
        copiable_reference &operator=(copiable_reference<Derived> &&other) noexcept {
            if (this != &other) {
                this->m_base = std::move(other.m_base);
            }
            return *this;
        }

        // copy assignment operator, which also receives any derived class
        template<derived_from<T> Derived>
        copiable_reference &operator=(const copiable_reference<Derived> &other) noexcept {
            if (this != &other) {
                this->m_base = other.m_base;
            }
            return *this;
        }

    public:
        // dynamic cast
        template<derived_from<T> Derived>
        Derived *as_dyn() {
            if constexpr (std::is_same_v<Derived, T>) {
                return m_base.cast();
            } else {
                return dynamic_cast<Derived *>(m_base.cast());
            }
        }

        template<derived_from<T> Derived>
        const Derived *as_dyn() const {
            if constexpr (std::is_same_v<Derived, T>) {
                return m_base.cast();
            } else {
                return dynamic_cast<Derived *>(m_base.cast());
            }
        }
    };

    export template<typename T, typename... Args>
    copiable_reference<T> make_reference(Args &&... args) {
        return copiable_reference<T>{
            copiable_reference_base<T>::create_reference(std::forward<Args>(args)...)
        };
    }
}
