#pragma once

#include <unordered_map>
#include <SDL3/SDL_scancode.h>

// made by gordie novak on feb 26th
// contains all the enums for a keyboard press type.

namespace pg {
    enum KeypressType {
        KEY_LSHIFT = SDL_SCANCODE_LSHIFT,
        KEY_RSHIFT = SDL_SCANCODE_RSHIFT,
        KEY_LCTRL = SDL_SCANCODE_LCTRL,
        KEY_RCTRL = SDL_SCANCODE_RCTRL,
        KEY_LALT = SDL_SCANCODE_LALT,
        KEY_RALT = SDL_SCANCODE_RALT,
        KEY_SPACE = SDL_SCANCODE_SPACE,
        KEY_BACKSPACE = SDL_SCANCODE_BACKSPACE,
        KEY_TAB = SDL_SCANCODE_TAB,
        KEY_RETURN = SDL_SCANCODE_RETURN,
        KEY_FSLASH = SDL_SCANCODE_SLASH,
        KEY_BACKSLASH = SDL_SCANCODE_BACKSLASH,
        KEY_COMMA = SDL_SCANCODE_COMMA,
        KEY_PERIOD = SDL_SCANCODE_PERIOD,
        KEY_APOSTROPHE = SDL_SCANCODE_APOSTROPHE,
        KEY_MINUS = SDL_SCANCODE_MINUS,
        KEY_EQUALS = SDL_SCANCODE_EQUALS,
        KEY_LBRACKET = SDL_SCANCODE_LEFTBRACKET,
        KEY_RBRACKET = SDL_SCANCODE_RIGHTBRACKET,
        KEY_SEMICOLON = SDL_SCANCODE_SEMICOLON,
        KEY_CAPSLOCK = SDL_SCANCODE_CAPSLOCK,
        KEY_GRAVE = SDL_SCANCODE_GRAVE,
        KEY_F1 = SDL_SCANCODE_F1,
        KEY_F2 = SDL_SCANCODE_F2,
        KEY_F3 = SDL_SCANCODE_F3,
        KEY_F4 = SDL_SCANCODE_F4,
        KEY_F5 = SDL_SCANCODE_F5,
        KEY_F6 = SDL_SCANCODE_F6,
        KEY_F7 = SDL_SCANCODE_F7,
        KEY_F8 = SDL_SCANCODE_F8,
        KEY_F9 = SDL_SCANCODE_F9,
        KEY_F10 = SDL_SCANCODE_F10,
        KEY_F11 = SDL_SCANCODE_F11,
        KEY_F12 = SDL_SCANCODE_F12,
        KEY_LEFT = SDL_SCANCODE_LEFT,
        KEY_RIGHT = SDL_SCANCODE_RIGHT,
        KEY_UP = SDL_SCANCODE_UP,
        KEY_DOWN = SDL_SCANCODE_DOWN,
        KEY_A = SDL_SCANCODE_A,
        KEY_B = SDL_SCANCODE_B,
        KEY_C = SDL_SCANCODE_C,
        KEY_D = SDL_SCANCODE_D,
        KEY_E = SDL_SCANCODE_E,
        KEY_F = SDL_SCANCODE_F,
        KEY_G = SDL_SCANCODE_G,
        KEY_H = SDL_SCANCODE_H,
        KEY_I = SDL_SCANCODE_I,
        KEY_J = SDL_SCANCODE_J,
        KEY_K = SDL_SCANCODE_K,
        KEY_L = SDL_SCANCODE_L,
        KEY_M = SDL_SCANCODE_M,
        KEY_N = SDL_SCANCODE_N,
        KEY_O = SDL_SCANCODE_O,
        KEY_P = SDL_SCANCODE_P,
        KEY_Q = SDL_SCANCODE_Q,
        KEY_R = SDL_SCANCODE_R,
        KEY_S = SDL_SCANCODE_S,
        KEY_T = SDL_SCANCODE_T,
        KEY_U = SDL_SCANCODE_U,
        KEY_V = SDL_SCANCODE_V,
        KEY_W = SDL_SCANCODE_W,
        KEY_X = SDL_SCANCODE_X,
        KEY_Y = SDL_SCANCODE_Y,
        KEY_Z = SDL_SCANCODE_Z,
    };

    inline bool operator==(const KeypressType k1, const KeypressType k2) noexcept {
        return static_cast<int>(k1) == static_cast<int>(k2);
    }
    inline bool operator==(const SDL_Scancode s, const KeypressType k) noexcept {
        return static_cast<int>(s) == static_cast<int>(k);
    }
    inline bool operator==(const KeypressType k, const SDL_Scancode s) noexcept {
        return s == k;
    }

    template<typename T>
    concept isValidKeypressType =
        std::is_same_v<T, KeypressType>
        || std::is_same_v<T, char>
        || std::is_same_v<T, SDL_Scancode>;

    namespace internal_keyboard_map {
        static const std::unordered_map<char, SDL_Scancode> keyMap = {
            {'`', SDL_SCANCODE_GRAVE},
            {';', SDL_SCANCODE_SEMICOLON},
            {'\'', SDL_SCANCODE_APOSTROPHE},
            {'[', SDL_SCANCODE_LEFTBRACKET},
            {']', SDL_SCANCODE_RIGHTBRACKET},
            {'0', SDL_SCANCODE_0},
            {'-', SDL_SCANCODE_MINUS},
            {'=', SDL_SCANCODE_EQUALS},
            {'\\', SDL_SCANCODE_BACKSLASH},
            {'/', SDL_SCANCODE_SLASH},
            {'.', SDL_SCANCODE_COMMA},
            {',', SDL_SCANCODE_COMMA},
            {' ', SDL_SCANCODE_SPACE}
        };
    }
}