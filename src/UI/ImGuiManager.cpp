#include "ImGuiManager.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <SDL.h>

extern SDL_Window* g_Window;
extern SDL_GLContext g_GLContext;

void ImGuiManager::Init() {
    // Создаем контекст ImGui и настраиваем стили
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    
    // Инициализация ImGui с backend-ами SDL2 и OpenGL
    ImGui_ImplSDL2_InitForOpenGL(g_Window, g_GLContext);
    ImGui_ImplOpenGL3_Init("#version 130");
}

void ImGuiManager::Shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiManager::NewFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(g_Window);
    ImGui::NewFrame();
}
