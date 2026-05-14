#pragma once

#include "../toolkit/apidef.h"

// made by gordie novak feb 25th

// used to store all the input data: read only except for the InputReceiver.



namespace pig {
    class Engine;

    enum MouseButtonType {
        MOUSE_L = SDL_BUTTON_LMASK,
        MOUSE_R = SDL_BUTTON_RMASK,
        MOUSE_MIDDLE = SDL_BUTTON_MMASK,
        MOUSE_X1 = SDL_BUTTON_X1MASK,
        MOUSE_X2 = SDL_BUTTON_X2MASK,
    };
    
    class Mouse {
        friend struct InputUpdater;
        friend class Window;
        /// Position & delta position of the mouse.
        vec2 mousePos{}, deltaMousePos{};
        /// How much the scroll wheel has movel
        vec2 deltaScrollWheel{0.f, 0.f};
        /// Bitmask representing the mouse state.
        uint32_t mouseFlags{};

        friend class EngineCore;

    public:
        Mouse();

        // <><><> Mouse positioning <><><>
        /// Returns the current mouse position.
        [[nodiscard]] vec2 getPos() const {
            return mousePos;
        }
        /// Returns the x-position of the mouse
        [[nodiscard]] float getX() const {
            return mousePos.x;
        }
        /// Returns the y-position of the mouse
        [[nodiscard]] float getY() const {
            return mousePos.y;
        }
        /// Returns the change in mouse position
        [[nodiscard]] vec2 getDeltaPos() const {
            return deltaMousePos;
        }

        /// Returns the change in MousePosX()
        [[nodiscard]] float getDeltaX() const {
            return deltaMousePos.x;
        }

        /// Returns the change in MousePosY()
        [[nodiscard]] float getDeltaY() const {
            return deltaMousePos.y;
        }

        /// Returns the total distance the scroll
        [[nodiscard]] vec2 getScrollWheelDist() const {
            return deltaScrollWheel;
        }

        /// Returns the change in the scroll wheel x
        [[nodiscard]] float getScrollWheelX() const {
            return deltaScrollWheel.x;
        }

        /// Returns the change in the scroll wheel y
        [[nodiscard]] float getScrollWheelY() const {
            return deltaScrollWheel.y;
        }

        /// @param button The type of mouse action you want to verify.
        /// - @code MOUSE_L@endcode › Left click
        /// - @code MOUSE_R@endcode › Right click
        /// - @code MOUSE_MIDDLE@endcode › Middle click
        /// - @code MOUSE_X1@endcode › First side button of the mouse
        /// - @code MOUSE_X2@endcode › Second side button of the mouse.
        /// @return @code true@endcode if the provided option was clicked this frame.
        [[nodiscard]] bool mouseState(const MouseButtonType button) const {
            return mouseFlags & button;
        }

        /// @return @code true@endcode if the mouse was left-clicked this frame.
        [[nodiscard]] bool isLeftClicked() const {
            return mouseFlags & MOUSE_L;
        }

        /// @return @code true@endcode if the mouse was right-clicked this frame
        [[nodiscard]] bool isRightClicked() const {
            return mouseFlags & MOUSE_R;
        }

        /// @return @code true@endcode if the mouse was middle-clicked this frame
        [[nodiscard]] bool isMiddleClicked() const {
            return mouseFlags & MOUSE_MIDDLE;
        }

        /// @return @code true@endcode if the mouse's first side button was clicked
        [[nodiscard]] bool isX1Clicked() const {
            return mouseFlags & MOUSE_X1;
        }

        /// @return @code true@endcode if the mouse's second side button was clicked
        [[nodiscard]] bool isX2Clicked() const {
            return mouseFlags & MOUSE_X2;
        }

        // Make sure the mouse state can't be accidentally copied.
        Mouse operator=(Mouse&&)              = delete;   ///< Nope. You can't do that.
        Mouse operator=(const Mouse& other)   = delete;   ///< Nope. You can't do that.
        Mouse(Mouse&& other)                  = delete;   ///< Nope. You can't do that.
        Mouse(const Mouse& other)             = delete;   ///< Nope. You can't do that.


        /* ---------------------------------------------------- */
        // Static update functions.

        /// Gets the current keyboard and mouse state from SDL and updates the given InputState object with it.
        /// @param mState The @code InputState@endcode object to update with SDL information.
        void updateWithSDL();

        /** Updates the mouse in accordance with SDL Events. More efficient than calling updateWithSDL().
         * @param e The SDL Event corresponding to the mouse */
        void onSDLMouseMotion(SDL_Event& e);
        void onSDLMouseButtonDown(SDL_Event& e);
        void onSDLMouseButtonUp(SDL_Event& e);
        void onSDLMouseWheel(SDL_Event& e);

        void clearMouseWheel();
    };

}
