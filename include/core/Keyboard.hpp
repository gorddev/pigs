#pragma once
#include <array>
#include <bitset>
#include <cstring>

#include <SDL3/SDL_scancode.h>

#include <core/enums/KeypressEnum.hpp>
#include <core/callbacks/SDL_ForwardDeclaration.hpp>

#include <core/errors/unwrap.hpp>

// Created by Gordie Novak on 2/26/26.
// handles keyboard management and data access for currently held keys.


namespace pg {

    class Keyboard {
        friend class Engine;
        static constexpr int maxKeys = 256;
        /// Array of each key. True if the key is held down. False otherwise.
        std::array<bool, maxKeys> keys{};
        /** TRUE if the key was tapped this frame. False otherwise */
        std::bitset<maxKeys> tapped_1{};
        std::bitset<maxKeys> tapped_2{};
        std::bitset<maxKeys>* tapped_active = &tapped_1;

        friend struct InputUpdater;
        friend class EngineCore;

        Keyboard() = default;

    public:
        // <><><> Keys <><><>
        /// Returns true if the current key is held down. Direct access
        /// using SDL_Scancodes for extended support. This is the most flexible, but
        /// most verbose way to get a keybind.
        [[nodiscard]] bool isHeld(SDL_Scancode key) const {
            if (key < maxKeys)
                return keys[key];
            PG_Panic(
                "The SDL_Scancode ", key, " is not a valid scancode for retrieving user input.\n"
                                          "Scancodes must be less than ", maxKeys, ".");
            return false;
        }
        /// Returns true if the given character key is held down.
        /// @details For example, typing @code isHeld('A')@endcode or @code isHeld('a')@endcode will
        /// return true if the "A" key is held down.
        /// This is the least efficient way to get a keybind, but is the most readable.
        /// Performance impact is likely negligible.
        /// @returns @code true@endcode if the key is held down.
        [[nodiscard]] bool isHeld(char c) const {
            return keys[convertCharToScancode(c)];
        }
        /// The simplest way to get a keybind. Has the same performance as
        /// @code SDL_isHeld()@endcode, but with more limited key support.
        /// @details To use this function, just call it with your specified
        /// KeypressType: @code isHeld(KEYP_LSHIFT)@endcode.
        /// @returns @code true@endcode if the key is currently held down.
        [[nodiscard]] bool isHeld(const KeypressType key) const {
            return keys[key];
        };

        /// @param keyPresses The keys you want to check.
        /// Can be any combination of @code char@endcode, @code KeypressType@endcode, or @code SDL_Scancode@endcode.
        /// @return @code true@endcode if ALL keys are currently held down.
        template<typename... Args>
            requires (sizeof...(Args) > 0 && (isValidKeypressType<Args> && ...))
        [[nodiscard]] bool isAllHeld(Args&&... keyPresses) const {
            bool returnValue = true;
            ([&](const bool val) {
                if (!val)
                    returnValue = false;
            }(isHeld(keyPresses)), ...);
            return returnValue;
        }

        /// @param keyPresses The keys you want to check.
        /// Can be any combination of @code char@endcode, @code KeypressType@endcode, or @code SDL_Scancode@endcode.
        /// @return @code true@endcode if ANY of the keys are currently held down.
        template<typename... Args>
            requires (sizeof...(Args) > 0 && (isValidKeypressType<Args> && ...))
        [[nodiscard]] bool isAnyHeld(Args&&... keyPresses) const {
            bool returnValue = false;
            ([&](bool val) {
                if (val)
                    returnValue = true;
            }(isHeld(keyPresses)), ...);
            return returnValue;
        }

        /// Sets the status of the key to false for the current frame.
        ///
        /// Consider the following code block: @code
        /// void updatePlayerPos(Entity& e, Keyboard& k) {
        ///     if (k.isHeld('W')) {
        ///         e.move({0, 1, 0});
        ///         k.setKeyToFalse('W');
        ///     }
        /// }
        ///
        /// int main() {
        ///     // ...
        ///
        ///     Entity arr[2] = { player, enemy };
        ///
        ///     for (int i = 0; i < 2; i++) {
        ///         updatePlayerPos(arr[i], engine.keyboard);
        ///     }
        ///
        ///     // Result: Only the player's position is updated.
        /// }
        /// @endcode
        /// This allows you to propagate input only to certain entities.
        template<typename T>
            requires(isValidKeypressType<T>)
        void setKeyToFalse(T key) {
            if constexpr(std::is_same_v<T, char>)
                keys[convertCharToScancode(key)] = false;
            else
                keys[key] = false;
        }

        /// Converts the given character to its scancode representation.
        /// @param c The character you want to convert.
        /// @return The converted character in scancode form.
        static KeypressType convertCharToScancode(char c) {
            if ('1' <= c && c <= '9') {
                c -= ('1'- SDL_SCANCODE_1);
            } else if ('A' <= c && c <= 'Z') {
                c -= 61;
            } else if (c >= 'a' && c <= 'z') {
                c -= ('A' + 61);
            } else if (internal_keyboard_map::keyMap.contains(c)) {
                c = internal_keyboard_map::keyMap.at(c);
            } else PG_Panic("The key specified by \'", c, "\' is not a valid keymap.");
            return static_cast<KeypressType>(c);
        }

        template<typename T>
            requires(isValidKeypressType<T>)
        bool isTapped(T key) {
            if constexpr(std::is_same_v<T, char>)
                return (*tapped_active)[convertCharToScancode(key)];
            else
                return (*tapped_active)[key];
        }

    private:
        // Ensure the Keyboard can't be moved.
        Keyboard& operator=(Keyboard&& other)     = default;   ///< Nope. You can't do that.
        Keyboard& operator=(const Keyboard& other)= default;   ///< Nope. You can't do that.
        Keyboard(Keyboard&& other)                = default;   ///< Nope. You can't do that.
        Keyboard(const Keyboard& other)           = default;   ///< Nope. You can't do that.

        void update() {
            const bool* ptr = SDL_GetKeyboardState(nullptr);
            std::memcpy(keys.data(), ptr, maxKeys);
            if (tapped_active==&tapped_1) {
                tapped_1.reset();
                tapped_active = &tapped_2;
            } else {
                tapped_2.reset();
                tapped_active = &tapped_1;
            }
        }

        void onKeyPress(const SDL_Event& event) {
            if (tapped_active == &tapped_1) {
                tapped_2.set(event.key.scancode);
            } else {
                tapped_1.set(event.key.scancode);
            }
        }

        friend SDL_AppResult (::SDL_AppEvent(void*, SDL_Event* event));
    };
}
