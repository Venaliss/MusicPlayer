#include "UI/ImGuiManager.h"
#include "UI/MainWindow.h"
#include "Audio/AudioEngine.h"

// Для примера используем SDL2 и OpenGL как backend для ImGui
#include <SDL.h>
#include <SDL_opengl.h>
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

SDL_Window* g_Window = nullptr;
SDL_GLContext g_GLContext = nullptr;

int main(int argc, char* argv[]) {
    // Инициализация SDL и создание окна
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }
    
    // Создаем окно с OpenGL контекстом
    g_Window = SDL_CreateWindow("Музыкальный плеер", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    g_GLContext = SDL_GL_CreateContext(g_Window);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetSwapInterval(1);
    
    // Инициализируем ImGui через наш менеджер
    ImGuiManager::Init();
    
    MainWindow mainWindow;
    bool done = false;
    SDL_Event event;
    
    while (!done) {
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
        }
        // Начало нового кадра ImGui
        ImGuiManager::NewFrame();
        
        // Отрисовка главного окна плеера
        mainWindow.Render();
        
        // Завершаем кадр
        ImGui::Render();
        glViewport(0, 0, 800, 600);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(g_Window);
    }
    
    // Очистка
    ImGuiManager::Shutdown();
    SDL_GL_DeleteContext(g_GLContext);
    SDL_DestroyWindow(g_Window);
    SDL_Quit();
    
    return 0;
}
