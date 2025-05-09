#include "UI/ImGuiManager.h"
#include "UI/MainWindow.h"
#include "Audio/AudioEngine.h"

#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// Глобальная переменная окна для GLFW (используется в ImGuiManager)
GLFWwindow* g_Window = nullptr;

int main(int argc, char* argv[]) {
    // Инициализация GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Ошибка инициализации GLFW\n");
        return -1;
    }

    // Настройки OpenGL (здесь используется версия 3.0, соответствующая "#version 130")
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    // Создаем окно GLFW с OpenGL контекстом
    g_Window = glfwCreateWindow(800, 600, "Музыкальный плеер", nullptr, nullptr);
    if (g_Window == nullptr) {
        fprintf(stderr, "Не удалось создать окно GLFW\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(g_Window);
    glfwSwapInterval(1); // Вертикальная синхронизация

    // Инициализация ImGui через наш менеджер (используется теперь GLFW)
    ImGuiManager::Init();

    MainWindow mainWindow;
    bool done = false;

    // Основной цикл приложения
    while (!glfwWindowShouldClose(g_Window)) {
        // Обработка событий GLFW
        glfwPollEvents();

        // Начало нового кадра ImGui
        ImGuiManager::NewFrame();

        // Отрисовка главного окна плеера
        mainWindow.Render();

        // Завершаем кадр
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(g_Window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(g_Window);
    }

    // Очистка ресурсов
    ImGuiManager::Shutdown();
    glfwDestroyWindow(g_Window);
    glfwTerminate();

    return 0;
}
