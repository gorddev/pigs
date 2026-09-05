#pragma once


namespace pg {

    /** Represents a type erased pointer to a set of data. */

    class void_ptr {
        void* n_ptr;
        void_ptr() = default;
        friend class Exec;
    public:
        template<typename T>
        void_ptr& operator=(T* ptr) {
            this->n_ptr = ptr;
            return *this;
        }
        template<typename T>
        T& as() {
            return *static_cast<T*>(n_ptr);
        }
        operator void*() const {
            return n_ptr;
        }
    };

}