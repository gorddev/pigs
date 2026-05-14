#pragma once

#include <../toolkit/apidef.h>

#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl3.h"
#include "../core/Window.hpp"

namespace pig::imgui {
    [[nodiscard]] inline ImGuiContext* create_context(const Window& window) {
        auto context = ImGui::CreateContext();
        ImGui_ImplOpenGL3_Init(glVersionHeader);
        ImGui_ImplSDL3_InitForOpenGL(window, window.getGLContext());


        int w, h, pw, ph;
        ImGuiIO& io = ImGui::GetIO();
        SDL_GetWindowSize(window, &w, &h);
        SDL_GetWindowSizeInPixels(window, &pw, &ph);

        io.DisplaySize = ImVec2((float)w, (float)h);
        io.DisplayFramebufferScale = ImVec2(
            window.getDPIScale(),
            window.getDPIScale()
        );

        return context;
    }

    inline void new_frame(ImGuiContext* context = nullptr) {
        if (context != nullptr) {
            ImGui::SetCurrentContext(context);
        }
        // Set up the new frames for each.
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
    }

    inline void render(ImGuiContext* context = nullptr) {
        if (context != nullptr) {
            ImGui::SetCurrentContext(context);
        }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    inline void shutdown(ImGuiContext* context = nullptr) {
        if (context != nullptr) {
            ImGui::SetCurrentContext(context);
        }
        ImGui_ImplSDL3_Shutdown();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui::DestroyContext();
    }
}


