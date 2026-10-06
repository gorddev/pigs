#include <core/Window.hpp>

#include <SDL3/SDL.h>

#include "../../include/toolkit/apidef.h"
#include <unwrap.hpp>

// created by gordie feb 16th. implementation for window

using namespace pg;

Window::Window(SDL_Window* win, WindowProperty flags, const SDL_WindowID id, const SDL_GLContext gl, const vec2 dim)
    : id(id), sdl_window(win), flags(flags), gl_ctx(gl), dimensions(dim)
{
    int pw, ph;
    SDL_GetWindowSizeInPixels(sdl_window, &pw, &ph);
    pixelDimensions = {static_cast<float>(pw), static_cast<float>(ph)};

    dpiScale = SDL_GetWindowDisplayScale(sdl_window);
}

Window& Window::updateWindowDimensions() {
    int w, h;
    SDL_GetWindowSize(sdl_window, &w, &h);
    dimensions = vec2{static_cast<float>(w), static_cast<float>(h)};
    int pw, ph;
    SDL_GetWindowSizeInPixels(sdl_window, &pw, &ph);
    pixelDimensions = {static_cast<float>(pw), static_cast<float>(ph)};
    dpiScale = SDL_GetWindowDisplayScale(sdl_window);
    glViewport(0, 0, pw, ph);
    return *this;
}

Window Window::make(const char windowName[], const dim2 dim, WindowProperty flags)
{
    ensure_SDL_init();

    flags |= SDL_WINDOW_OPENGL;

    #ifdef PIGS_OpenGL_Core
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    #else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    #endif
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

    SDL_Window* sdl_window = SDL_CreateWindow(windowName, dim.w, dim.h, flags);

    if (!sdl_window) {
        PG_Panic("Failed to make window with error: ",  SDL_GetError());
    }

    SDL_GLContext gl_context = SDL_GL_CreateContext(sdl_window);

    if (!gl_context)
        PG_Panic("Failed to make OpenGL context with error: ",  SDL_GetError());

    PIG_gladLoadGL((GLADloadproc)SDL_GL_GetProcAddress);

    printf("\x1B[90mOpenGL Context Initialized: %s%s\n", reinterpret_cast<const char*>(glGetString(GL_RENDERER)), "\x1B[0m");

    SDL_ClearError();

    SDL_GL_MakeCurrent(sdl_window, gl_context);
    SDL_GL_SetSwapInterval(1);

    int w, h;
    SDL_GetWindowSize(sdl_window, &w, &h);

    return Window {
        sdl_window,
        flags,
        SDL_GetWindowID(sdl_window),
        gl_context,
        {static_cast<float>(w), static_cast<float>(h)},
    };
}

Window::Window()
    : id(0), sdl_window(nullptr),
      flags(WindowFlagNone), gl_ctx(nullptr), dimensions{}, pixelDimensions{}, dpiScale(1.0f)
{
}

Window& Window::operator=(Window&& o) noexcept {
    std::swap(sdl_window, o.sdl_window);
    std::swap(flags, o.flags);
    std::swap(gl_ctx, o.gl_ctx);
    std::swap(dimensions, o.dimensions);
    std::swap(id, o.id);
    return *this;
}

Window::Window(Window&& o) noexcept : id(o.id), sdl_window(o.sdl_window), flags(o.flags), gl_ctx(o.gl_ctx),
                                      dimensions(o.dimensions), pixelDimensions(), dpiScale(1) {
    o.sdl_window = nullptr;
    o.flags      = WindowFlagNone;
    o.gl_ctx     = nullptr;
    o.dimensions = {};
}

Window::~Window() {
    SDL_DestroyWindow(sdl_window);
    sdl_window = nullptr;
    flags = WindowDestroyed;
}

vec2 Window::normalizeToWindow(const vec2& pos) const {
    return  {pos.x/dimensions.x, pos.y/dimensions.y};
}

Window::operator SDL_Window*() const noexcept {
    return sdl_window;
}

float Window::getDPIScale() const {
    return dpiScale;
}

Window& Window::setDimensions(const dim2 dim) {
    SDL_SetWindowSize(sdl_window, dim.w, dim.h);
    updateWindowDimensions();
    return *this;
}


Window& Window::setWidth(uint32_t width) {
    SDL_SetWindowSize(sdl_window, width, dimensions.h);
    updateWindowDimensions();
    return *this;
}

Window& Window::setHeight(uint32_t height) {
    SDL_SetWindowSize(sdl_window, dimensions.w, height);
    updateWindowDimensions();
    return *this;
}

Window& Window::setPosition(const vec2 pos) {
    SDL_SetWindowPosition(sdl_window, pos.x, pos.y);
    return *this;
}

Window& Window::setFullscreen()  {
    flags |= WindowFullscreen;
    SDL_SetWindowFullscreen(sdl_window, true);
    return *this;
}

Window& Window::setWindowed() {
    flags &= ~WindowFullscreen;
    SDL_SetWindowFullscreen(sdl_window, false);
    return *this;
}

Window& Window::setResizable(bool b) {
    if (b)  flags |= WindowResizable;
    else    flags &= ~WindowResizable;
    SDL_SetWindowResizable(sdl_window, b);
    return *this;
}

Window& Window::setFloatOnTop(const bool b) {
    if (b)  flags |= WindowFloatOnTop;
    else    flags &= ~WindowFloatOnTop;
    SDL_SetWindowAlwaysOnTop(sdl_window, b);
    return *this;
}

Window& Window::setMouseGrab(const bool b) {
    if (b)  flags |= WindowMouseConfined;
    else    flags &= ~WindowMouseConfined;
    SDL_SetWindowMouseGrab(sdl_window, b);
    return *this;
}

Window& Window::setMouseLocking(const bool hidden) {
    if (hidden)  flags |= WindowMouseHidden;
    else        flags &= ~WindowMouseHidden;
    SDL_SetWindowRelativeMouseMode(sdl_window, hidden);
    return *this;
}

Window& Window::setKeyboardGrab(const bool b) {
    if (b)  flags |= WindowKeyboardGrabbed;
    else    flags &= ~WindowKeyboardGrabbed;
    SDL_SetWindowMouseGrab(sdl_window, b);
    return *this;
}

Window& Window::setIcon([[maybe_unused]] const char pathToImage[]) {
    /*
    SDL_Surface* surf; //= IMG_Load(pathToImage);
    #ifdef PIG_DEBUG
    if (!surf)
        std::cout << "Failed to load image: " << pathToImage << ".\n" << SDL_GetError() << std::endl;
    #endif
    //SDL_SetWindowIcon(sdl_window, surf);
    SDL_DestroySurface(surf);*/
    return *this;
}

Window& Window::setName(const char name[]) {
    SDL_SetWindowTitle(sdl_window, name);
    return *this;
}

Window& Window::hide() {
    flags |= WindowHidden;
    SDL_HideWindow(sdl_window);
    return *this;
}

Window& Window::show() {
    flags &= ~WindowHidden;
    SDL_ShowWindow(sdl_window);
    return *this;
}

Window& Window::setOpacity(const float opacity) {
    if (flags & WindowTransparent)
        SDL_SetWindowOpacity(sdl_window, opacity);
    else
        PG_Panic(
            "Cannot set window opacity, as flag 'WindowTransparent'"
            "was not enabled at launch.");
    return *this;
}

bool Window::isFullscreen() const {
    return flags & WindowFullscreen;
}

bool Window::isWindowed() const {
    return !(flags & WindowFullscreen);
}

bool Window::isHidden() const {
    return flags & WindowHidden;
}

bool Window::isHighDPI() const {
    return flags & WindowHighDPI;
}

bool Window::hasProperty(const WindowProperty flag) const {
    return flags & flag;
}

bool Window::isResizable() const {
    return flags & WindowResizable;
}

bool Window::isFloatOnTop() const {
    return flags & WindowFloatOnTop;
}

bool Window::isMouseConfined() const {
    return flags & WindowMouseConfined;
}

bool Window::isKeyboardGrab() const {
    return flags & WindowKeyboardGrabbed;
}

bool Window::isTransparent() const {
    return flags & WindowTransparent;
}

bool Window::isMouseLocked() const {
    return flags & WindowMouseHidden;
}

bool Window::isFocused() const {
    return flags & WindowFocused;
}

float Window::getOpacity() const noexcept {
    return SDL_GetWindowOpacity(sdl_window);
}

vec2 Window::getDimensions() const noexcept {
    return dimensions;
}

float Window::getWidth() const noexcept {
    return dimensions.w;
}

float Window::getHeight() const noexcept {
    return dimensions.h;
}

vec2 Window::getPosition() const noexcept {
    int x, y;
    SDL_GetWindowPosition(sdl_window, &x, &y);
    return {static_cast<float>(x), static_cast<float>(y)};
}

uint32_t Window::getWindowId() const noexcept {
    return id;
}

SDL_GLContext Window::getGLContext() const noexcept {
    return gl_ctx;
}

dim2 Window::getWindowPixelSize() const noexcept {
    int x, y;
    SDL_GetWindowSizeInPixels(sdl_window, &x, &y);
    return {x, y};
}

uint16_t Window::getPixelWidth() const noexcept {
    return static_cast<uint16_t>(pixelDimensions.w);
}

uint16_t Window::getPixelHeight() const noexcept {
    return static_cast<uint16_t>(pixelDimensions.h);
}

Window& Window::setGLClearColor(vec4 c) {
    SDL_GL_MakeCurrent(sdl_window, gl_ctx);
    glClearColor(c.r, c.g, c.b, c.a);
    return *this;
}

void Window::on_SDLWindowEvent(SDL_Event& e) noexcept {
    if (e.window.windowID == this->id) {
        updateWindowDimensions();
    }
}
