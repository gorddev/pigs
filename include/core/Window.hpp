#pragma once

/* Created by Gordie Novak on 3/15/26.
 * Purpose: 
 */

#include <SDL3/SDL_video.h>

#include "../toolkit/RenderingSettings.hpp"
#include "../toolkit/types/dim2.hpp"
#include "../toolkit/types/vec.hpp"

// made by gordie novak feb 16th

// new window management system so you can easily adjust window properties


namespace pg {


    /** It's a window that displays stuff! Create a window with @code pg::Window::make(...)@endcode
     *
     * There's many handy-dandy functions you have access to.
     */
    class Window {
        SDL_WindowID id;  ///< Internal id of the window. Do not change.

    public:
        /// Construct with a default width and height with a GL context attached.
        /// @param windowName The name of the window you want to create
        /// @param dim The width and height of the window.
        /// @param flags Creates specific flags for the window.
        /// @return The Window if successful. Will throw an exception on failure.
        static Window make(const char windowName[], dim2 dim, WindowProperty flags = WindowFlagNone);

        Window();
        Window(Window&&) noexcept;
        Window& operator=(Window&&) noexcept;
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;


        ~Window(); ///< Destructor

    // ************************************************************** //

        // <<< Setters >>> //

        // most important setters
        void setDimensions(dim2 dim);           ///< Sets the window width & height @code {w, h}@endcode (in pixels)
        void setWidth(uint32_t width);          ///< Sets the width of the window (in pixels).
        void setHeight(uint32_t height);        ///< Sets the height of the window (in pixels).
        void setPosition(vec2 pos) const;       ///< Sets the position of the window (in pixels)
        void setGLClearColor(vec4) const;       ///< Sets the clear color of the current OpenGL context.
        void setFullscreen();                   ///< Sets the window to fullscreen mode.
        void setWindowed();                     ///< Sets the window into "Windowed" mode.

        // secondary setters
        void setResizable(bool);                ///< Enables/disables window resizability
        void setFloatOnTop(bool);               ///< Enables/disables the window staying on top of other windows.
        void setMouseGrab(bool);                ///< Confines/frees the mouse to the window
        void setMouseLocking(bool);             ///< Locks/unlocks & hides/unhides the mouse (like in an FPS game).
        void setKeyboardGrab(bool) ;            ///< Locks/unlocks the keyboard to the window.
        void setIcon(const char pathToImage[]); ///< Sets the icon of the application to the image at the given path.
        void setName(const char name[]) const;  ///< Sets the name of the window
        void hide();                            ///< Makes the window hidden if currently visible.
        void show();                            ///< Makes the window visible if currently hidden.
        /** Sets the opacity of the window
          * @note Requires flag @code WindowTransparent@endcode enabled at launch.
          * @warning Not web-safe. Does not work within web browsers.  */
        void setOpacity(float opacity) const;

    // ************************************************************** //

        // <<< Getters & Properties >>> //

        [[nodiscard]] bool isFloatOnTop() const;    ///< Returns @code true@endcode If the window has priority of display over other windows.
        [[nodiscard]] bool isFocused() const;       ///< Returns @code true@endcode If the window is currently focused.
        [[nodiscard]] bool isFullscreen() const;    ///< Returns @code true@endcode if the window is in fullscreen mode.
        [[nodiscard]] bool isKeyboardGrab() const;  ///< Returns @code true@endcode If the keyboard is currently bound to the window.
        [[nodiscard]] bool isMouseConfined() const; ///< Returns @code true@endcode If the mouse is currently trapped within the window.
        [[nodiscard]] bool isMouseLocked() const;   ///< Returns @code true@endcode If the mouse is currently locked & hidden in the window.
        [[nodiscard]] bool isResizable() const;     ///< Returns @code true@endcode If the window is currently resizable.
        [[nodiscard]] bool isTransparent() const;   ///< Returns @code true@endcode If the window has transparency enabled.
        [[nodiscard]] bool isWindowed() const;      ///< Returns @code true@endcode if the window is in "windowed" mode.
        [[nodiscard]] bool isHidden() const;        ///< Returns @code true@endcode if the window is currently hidden.
        [[nodiscard]] bool isHighDPI() const;
        [[nodiscard]] bool hasProperty(WindowProperty) const; ///< Returns @code true@endcode if the window has the specified window property.

    // ************************************************************** //

        // <<< Window Positioning, Size, and Identification >>> //

        [[nodiscard]] float     getOpacity() const noexcept;        ///< Returns the opacity of the current window if transparency is enabled.
        [[nodiscard]] vec2      getDimensions() const noexcept;     ///< Returns the dimensions of the window {width, height} (in pixels)
        [[nodiscard]] float     getWidth() const noexcept;          ///< Returns the width of the current window (in pixels)
        [[nodiscard]] float     getHeight() const noexcept;         ///< Returns the height of the current window (in pixels)
        [[nodiscard]] vec2      getPosition() const noexcept;       ///< Returns the position of the current window {x, y} (in pixels)
        [[nodiscard]] dim2      getWindowPixelSize() const noexcept;
        [[nodiscard]] float     getPixelWidth() const noexcept;
        [[nodiscard]] float     getPixelHeight() const noexcept;
        [[nodiscard]] SDL_WindowID getWindowId() const noexcept;
        [[nodiscard]] SDL_GLContext getGLContext() const noexcept;  ///< Returns GLContext object

    // ************************************************************** //

        // <<< Utilities & Functionality  >>> //
        /// Call this function when an @code SDL_EVENT_WINDOW@endcode of any kind happens.
        void on_SDLWindowEvent(SDL_Event& e) noexcept;
        /// Converts position from pixels {pixel_x, pixel_y} to NDC/Clip Space (-1.0 to 1.0)
        [[nodiscard]] vec2 normalizeToClipSpace(const vec2& pos) const;
        /// Converts position from pixels {pixel_x, pixel_y} to normalized coordinates {0.0 to 1.0}
        [[nodiscard]] vec2 normalizeToWindow(const vec2& pos) const;

        /// Automatically converts to an SDL Window for SDL library calls, because this object is a wrapper around the SDL Window.
        operator SDL_Window* () const noexcept; // NOLINT(*-explicit-constructor)
        [[nodiscard]] float getDPIScale() const;

    private:
        SDL_Window *sdl_window; ///< Window the renderer draws to.
        WindowProperty flags;   ///< Internal flags for keeping track of states without repeat SDL calls
        SDL_GLContext gl_ctx;   ///< Allows us to access OpenGL functions & set the openGL context.
        vec2 dimensions;        ///< Internal Window dimensions
        vec2 pixelDimensions;   ///< InternalDimensions in Pixels
        float dpiScale;         ///< Scale due to High DPI displays.

        Window(SDL_Window*, WindowProperty, SDL_WindowID, SDL_GLContext, vec2);
        void updateWindowDimensions();
    };
}