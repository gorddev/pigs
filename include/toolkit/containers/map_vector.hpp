#pragma once
#include <unordered_map>
#include <variant>
#include <vector>

// made by gordie novak, feb 22nd, 2026

// the point of the pair vector is that it provides
// two access points to each element: a map, and a vector.
// vector is good for fast iteration. map is good for
// O(1) lookup and linking strings to the object.
// The map contains the vector indices.

// overall, this is a pretty expensive data structure and more
// exists for convenience rather than speed. it also ensures safety
// because it doesn't give you access to raw elements, only optionals.

namespace pg {

    template<typename Key, typename T, typename size_t = std::size_t>
    class map_vector {
        /// The unordered map that links a custom type to elements in the vector.
        std::unordered_map<Key, size_t> pair_map;
        /// The vector stores the actual elements
        std::vector<T> vector;
        /// A vector that holds destroyed elements
        std::vector<size_t> destroyed;
    public:
        map_vector() = default;
        explicit map_vector(size_t reserve) {
            vector.reserve(reserve);
            destroyed.reserve(reserve);
            pair_map.reserve(reserve);
        }

        map_vector(map_vector&& o) noexcept
            : pair_map(std::move(o.pair_map)), vector(std::move(o.vector)), destroyed(std::move(o.destroyed)) {}
        map_vector& operator=(map_vector&& o) noexcept {
            std::swap(this->vector, o.vector);
            std::swap(this->pair_map, o.pair_map);
            std::swap(this->destroyed, o.destroyed);
            return *this;
        }
        /// Returns the element at the given map entry.
        std::optional<T*> map(Key t) const {
            if (pair_map.contains(t))
                return const_cast<T*>(vector.data() + pair_map.at(t));
            return std::nullopt;
        }
        /// Returns the object at the given index in the vector
        std::optional<T*> at(size_t index) const {
            if (index < vector.size() && std::find(destroyed.begin(), destroyed.end(), index) == destroyed.end())
                return const_cast<T*>(vector.data() + index);
            return std::nullopt;
        }
        /// Adds an element to the vector with a key. Returns the index at which it was added
        size_t add(Key key, T element) {
            if (destroyed.empty()) {
                if (pair_map.contains(key)) {
                    auto index = pair_map[key];
                    std::destroy_at(vector.data() + index);
                    std::construct_at(vector.data() + index, std::move(element));
                    return index;
                }
                pair_map[key] = vector.size();
                vector.emplace_back(std::move(element));
                return vector.size() - 1;
            }
            auto index = destroyed.back();
            destroyed.pop_back();
            pair_map[key] = index;
            std::construct_at(vector.data() + index, std::move(element));
            return index;
        }
        /// Removes an element with the given key.
        void removeWithKey(Key key) {
            if (pair_map.contains(key)) {
                auto index = pair_map[key];
                pair_map.erase(key);
                std::destroy_at(vector.data() + index);
                destroyed.push_back(index);
            }
        }
        /// Removes an element with its vector index. Slower than remove with Key.
        void removeWithIndex(size_t index) {
            if (index < vector.size()) {
                for (auto it = pair_map.begin(); it != pair_map.end(); ++it) {
                    if (it->second == index) {
                        pair_map.erase(it);
                        break;
                    }
                }
                std::destroy_at(vector.data() + index);
                destroyed.push_back(index);
            }
        }
        /// Gets an element with either the key or the index. Requires that
        /// Keys are not an unsigned integral type.
        template<typename K>
            requires(!std::same_as<Key, size_t> && std::same_as<K, Key>)
        std::optional<T*> get(const K& key) const {
            return map(key);
        }

        template<size_t>
            requires(!std::same_as<Key, size_t>)
        std::optional<T*> get(const size_t index) const {
            return at(index);
        }
        /// Returns true if the map_vector has the provided key
        bool has(Key k) {
            return pair_map.contains(k);
        }

        // TODO: This iterates through destroyed elements. oops. i don't want to make custom iterator again its tiring.
        /// @warning If you have removed elements, this will iterate through them. Not fixed.
        auto begin() {
            return vector.begin();
        }
        auto end()  {
            return vector.end();
        }

        T& back() {
            return vector.back();
        }

        size_t size() {
            return vector.size();
        }

        T& operator[](size_t index) {
            return vector[index];
        }

        /// Returns the vector index of a specific key.
        std::optional<size_t> indexOf(Key k) {
            if (pair_map.contains(k)) {
                return pair_map.at(k);
            } return std::nullopt;
        }

        /// Returns true if the given index is destroyed
        [[nodiscard]] bool isDestroyed(size_t i) {
            for (auto& index: destroyed) {
                if (i == index)
                    return true;
            }
            return false;
        }
    };


}