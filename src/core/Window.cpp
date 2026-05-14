#include <SDL3/SDL.h>

#include "Window.hpp"

#include "../../include/toolkit/apidef.h"
#include "toolkit/errors/pig_err.hpp"

#ifdef GAN_DEBUG
#include <iostream>
#include <ostream>
#endif

// created by gordie feb 16th. implementation for window

using namespace pig;

Window::Window(SDL_Window* win, WindowProperty flags, const SDL_WindowID id, const SDL_GLContext gl, const vec2 dim)
    : id(id), sdl_window(win), flags(flags), gl_ctx(gl), dimensions(dim)
{
    int pw, ph;
    SDL_GetWindowSizeInPixels(sdl_window, &pw, &ph);
    pixelDimensions = {static_cast<float>(pw), static_cast<float>(ph)};

    dpiScale = SDL_GetWindowDisplayScale(sdl_window);
}

void Window::updateWindowDimensions() {
    int w, h;
    SDL_GetWindowSize(sdl_window, &w, &h);
    dimensions = vec2{static_cast<float>(w), static_cast<float>(h)};
    int pw, ph;
    SDL_GetWindowSizeInPixels(sdl_window, &pw, &ph);
    pixelDimensions = {static_cast<float>(pw), static_cast<float>(ph)};
    dpiScale = SDL_GetWindowDisplayScale(sdl_window);
    glViewport(0, 0, pw, ph);
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
        err::panic("Window::Window()", "Failed to make window with error: ",  SDL_GetError());
    }

    SDL_GLContext gl_context = SDL_GL_CreateContext(sdl_window);

    if (!gl_context)
        err::panic("Window::Window()", "Failed to make OpenGL context with error: ",  SDL_GetError());

    GAN_gladLoadGL((GLADloadproc)SDL_GL_GetProcAddress);

    printf("OpenGL Context Initialized: %s\n", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));

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

Window::Window(Window&& o) noexcept : id(o.id), sdl_window(o.sdl_window), flags(o.flags), gl_ctx(o.gl_ctx), dimensions(o.dimensions) {
    o.sdl_window = nullptr;
    o.flags = WindowFlagNone;
    o.gl_ctx = nullptr;
    o.dimensions = {};
}

Window::~Window() {
    SDL_DestroyWindow(sdl_window);
    sdl_window = nullptr;
    flags = WindowDestroyed;
}

Window::operator SDL_Window*() const noexcept {
    return sdl_window;
}

float Window::getDPIScale() const {
    return dpiScale;
}

void Window::setDimensions(const dim2 dim) {
    SDL_SetWindowSize(sdl_window, dim.w, dim.h);
    updateWindowDimensions();
}


void Window::setWidth(uint32_t width) {
    SDL_SetWindowSize(sdl_window, width, dimensions.h);
    updateWindowDimensions();
}

void Window::setHeight(uint32_t height) {
    SDL_SetWindowSize(sdl_window, dimensions.w, height);
    updateWindowDimensions();
}

void Window::setPosition(const vec2 pos) const {
    SDL_SetWindowPosition(sdl_window, pos.x, pos.y);
}

void Window::setFullscreen()  {
    flags |= WindowFullscreen;
    SDL_SetWindowFullscreen(sdl_window, true);
}

void Window::setWindowed() {
    flags &= ~WindowFullscreen;
    SDL_SetWindowFullscreen(sdl_window, false);
}

void Window::setResizable(const bool b) {
    if (b)  flags |= WindowResizable;
    else    flags &= ~WindowResizable;
    SDL_SetWindowResizable(sdl_window, b);
}

void Window::setFloatOnTop(const bool b) {
    if (b)  flags |= WindowFloatOnTop;
    else    flags &= ~WindowFloatOnTop;
    SDL_SetWindowAlwaysOnTop(sdl_window, b);
}

void Window::setMouseGrab(const bool b) {
    if (b)  flags |= WindowMouseConfined;
    else    flags &= ~WindowMouseConfined;
    SDL_SetWindowMouseGrab(sdl_window, b);
}

void Window::setMouseLocking(const bool hidden) {
    if (hidden)  flags |= WindowMouseHidden;
    else        flags &= ~WindowMouseHidden;
    SDL_SetWindowRelativeMouseMode(sdl_window, hidden);
}

void Window::setKeyboardGrab(const bool b) {
    if (b)  flags |= WindowKeyboardGrabbed;
    else    flags &= ~WindowKeyboardGrabbed;
    SDL_SetWindowMouseGrab(sdl_window, b);
}

void Window::setIcon(const char pathToImage[]) {
    SDL_Surface* surf; //= IMG_Load(pathToImage);
    #ifdef GAN_DEBUG
    if (!surf)
        std::cout << "Failed to load image: " << pathToImage << ".\n" << SDL_GetError() << std::endl;
    #endif
    SDL_SetWindowIcon(sdl_window, surf);
    SDL_DestroySurface(surf);
}

void Window::setName(const char name[]) const {
    SDL_SetWindowTitle(sdl_window, name);
}

void Window::hide() {
    flags |= WindowHidden;
    SDL_HideWindow(sdl_window);
}

void Window::show() {
    flags &= ~WindowHidden;
    SDL_ShowWindow(sdl_window);
}

void Window::setOpacity(const float opacity) const {
    if (flags & WindowTransparent)
        SDL_SetWindowOpacity(sdl_window, opacity);
    else
        err::panic("pig::Window::setOpacity()",
            "Cannot set window opacity, as flag 'WindowTransparent'"
            "was not enabled at launch.");
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

float Window::getPixelWidth() const noexcept {
    return pixelDimensions.w;
}

float Window::getPixelHeight() const noexcept {
    return pixelDimensions.h;
}

void Window::setGLClearColor(vec4 c) const  {
    SDL_GL_MakeCurrent(sdl_window, gl_ctx);
    glClearColor(c.r, c.g, c.b, c.a);
}

void Window::on_SDLWindowEvent(SDL_Event& e) noexcept {
    if (e.window.windowID == this->id) {
        updateWindowDimensions();
    }
}

vec2 Window::normalizeToClipSpace(const vec2& pos) const {
    vec2 ret = {pos.x, dimensions.h - pos.y};
    ret.x /= dimensions.w;
    ret.y /= dimensions.h;
    ret.x -= 0.5f;
    ret.y -= 0.5f;
    ret.x *= 2;
    ret.y *= 2;
    return ret;
}

vec2 Window::normalizeToWindow(const vec2& pos) const {
    return {pos.x/dimensions.w, pos.y/dimensions.h};
}


