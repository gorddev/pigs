#pragma once

#define INIT_QUEUE_SIZE 100
#include <vector>


namespace gutils {
    // Allows us to keep track of ids easily and re-use them (stack)
    template<typename T>
        requires(std::is_integral_v<T>)
    class stack {
    private:
        // List of all IDs
        std::vector<T> IDs;
        // Top of the stack
        uint32_t front = 0;
        uint32_t stackSize = INIT_QUEUE_SIZE;
        bool empty = true;

    public:
        //
        stack() {
            IDs.reserve(INIT_QUEUE_SIZE);
        }

        void push(const T id) {
            if (front >= stackSize) {
                stackSize += INIT_QUEUE_SIZE;
                IDs.reserve(stackSize);
            }
            IDs[front++] = id;
            empty = false;
        }

        T pop() {
            if (front - 1 == 0) empty = true;
            return (front != 0) ? IDs[--front] : -1;
        }

        [[nodiscard]] bool is_empty() const {
            return empty;
        }

        uint32_t num_elements() const {
            return front;
        }

        size_t size() const {
            return IDs.size();
        }

        T top() const {
            if (front == 0) return 0;
            return IDs[front - 1];
        }

        bool has(T t) const {
            for (auto& tt : IDs)
                if (tt == t) return true;
            return false;
        }
    };
}
