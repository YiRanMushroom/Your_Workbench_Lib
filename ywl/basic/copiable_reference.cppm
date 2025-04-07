module;

#include <cassert>

export module ywl.basic.copiable_reference;

import ywl.std.prelude;

namespace ywl::basic {
    template<typename Base, typename Derived>
    concept base_of = std::is_base_of_v<Base, Derived> || std::is_same_v<Derived, Base>;

    template<typename Derived, typename Base>
    concept derived_from = std::is_base_of_v<Base, Derived> || std::is_same_v<Derived, Base>;

    export template<typename T>
    class copiable_reference {
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

    private:
        pointer_type m_data{};
        copy_constructor_type m_copy_constructor{};
        deleter_type m_deleter{};

    public:
        void reset() {
            if (m_data) {
                assert(m_copy_constructor && m_deleter);
                m_deleter(reinterpret_cast<void *>(m_data));
                m_data = nullptr;
                m_copy_constructor = nullptr;
                m_deleter = nullptr;
            }
        }

        copiable_reference() = default;

    private:
        pointer_type clone_ptr() const {
            return static_cast<pointer_type>(m_copy_constructor(reinterpret_cast<void *>(m_data)));
        }

    public:
        copiable_reference(pointer_type data,
                           copy_constructor_type copy_constructor,
                           deleter_type deleter)
            : m_data(data),
              m_copy_constructor(copy_constructor),
              m_deleter(deleter) {
            assert(m_data && m_copy_constructor && m_deleter);
        }

        copiable_reference clone(this const copiable_reference &self) {
            pointer_type data = self.clone_ptr();
            return copiable_reference{
                data,
                self.m_copy_constructor,
                self.m_deleter
            };
        }

        copiable_reference take(this copiable_reference& self) {
            return std::move(self);
        }

        template<base_of<T> Base = T>
        Base &as_ref() {
            assert(m_data);
            return *reinterpret_cast<Base *>(m_data);
        }

        template<base_of<T> Base = T>
        const Base &as_ref() const {
            assert(m_data);
            return *reinterpret_cast<const Base *>(m_data);
        }

        T *get() {
            assert(m_data);
            return m_data;
        }

        T *get() const {
            assert(m_data);
            return m_data;
        }

        T &operator*() {
            assert(m_data);
            return *m_data;
        }

        const T &operator*() const {
            assert(m_data);
            return *m_data;
        }

        T *operator->() {
            assert(m_data);
            return m_data;
        }

        const T *operator->() const {
            assert(m_data);
            return m_data;
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
            m_data{std::exchange(other.m_data, nullptr)},
            m_copy_constructor{
                std::exchange(other.m_copy_constructor, nullptr)
            },
            m_deleter{std::exchange(other.m_deleter, nullptr)} {
        }

        // copy constructor, which also receives any derived class
        template<derived_from<T> Derived>
        copiable_reference(const copiable_reference<Derived> &other) noexcept : // NOLINT
            copiable_reference(other.clone()) {
        }

        // move assignment operator, which also receives any derived class
        template<derived_from<T> Derived>
        copiable_reference &operator=(copiable_reference<Derived> &&other) noexcept {
            if (this != &other) {
                reset();
                m_data = std::exchange(other.m_data, nullptr);
                m_copy_constructor = std::exchange(other.m_copy_constructor, nullptr);
                m_deleter = std::exchange(other.m_deleter, nullptr);
            }
            return *this;
        }

        // copy assignment operator, which also receives any derived class
        template<derived_from<T> Derived>
        copiable_reference &operator=(const copiable_reference<Derived> &other) noexcept {
            if (this != &other) {
                reset();
                m_data = other.clone_ptr();
                m_copy_constructor = other.m_copy_constructor;
                m_deleter = other.m_deleter;
            }
            return *this;
        }

    public:
        // dynamic cast
        template<derived_from<T> Derived>
        Derived *as_dyn() {
            if constexpr (std::is_same_v<Derived, T>) {
                return m_data;
            } else {
                return dynamic_cast<Derived *>(m_data);
            }
        }

        template<derived_from<T> Derived>
        const Derived *as_dyn() const {
            if constexpr (std::is_same_v<Derived, T>) {
                return m_data;
            } else {
                return dynamic_cast<Derived *>(m_data);
            }
        }

        // reinterpret_cast, dangerous.
        template<typename U>
        U *reinterpret_as() {
            assert(m_data);
            return reinterpret_cast<U *>(m_data);
        }

        template<typename U>
        const U *reinterpret_as() const {
            assert(m_data);
            return reinterpret_cast<const U *>(m_data);
        }
    };

    export template<typename T, typename... Args>
    copiable_reference<T> make_reference(Args &&... args) {
        T *data = new T{std::forward<Args>(args)...};

        struct copy_constructor {
            static void * copy(void *ptr) {
                return new T{*reinterpret_cast<T *>(ptr)};
            }
        };

        struct deleter {
            static void destroy(void *ptr) {
                delete reinterpret_cast<T *>(ptr);
            }
        };

        return copiable_reference<T>{
            data,
            &copy_constructor::copy,
            &deleter::destroy
        };
    }
}
