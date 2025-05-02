#include "ImGuiManager.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>

// Обработчик ошибок GLFW.
static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

ImGuiManager::ImGuiManager() : window(nullptr), m_cleanedUp(false) { }

ImGuiManager::~ImGuiManager() {
    Cleanup();
}

bool ImGuiManager::Init() {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return false;
    
    // Выбор версии OpenGL и GLSL.
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    
    // Создание окна с размерами 1280x720.
    window = glfwCreateWindow(1280, 720, "Music Player", NULL, NULL);
    if (window == NULL)
        return false;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // VSync

    // Инициализация контекста ImGui.
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    // Инициализация платформенных реализаций.
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    return true;
}

void ImGuiManager::NewFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiManager::Render() {
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}

bool ImGuiManager::WindowShouldClose() {
    return glfwWindowShouldClose(window);
}

void ImGuiManager::Cleanup() {
    if (m_cleanedUp)
        return; // Если очистка уже выполнена, ничего не делаем

    // Вызываем shutdown для бэкендов ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Уничтожаем окно GLFW
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();

    m_cleanedUp = true;
}
