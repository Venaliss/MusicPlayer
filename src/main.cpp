#include "UI/ImGuiManager.h"
#include "UI/MainWindow.h"
#include <GLFW/glfw3.h> // Для glfwPollEvents()
#include "imgui.h"

int main(int, char**)
{
    // Инициализируем менеджер ImGui.
    ImGuiManager imguiManager;
    if (!imguiManager.Init()) {
        return -1;
    }

    // Создаем экземпляр главного окна плеера.
    MainWindow mainWindow;

    // Основной цикл приложения.
    while (!imguiManager.WindowShouldClose())
    {
        imguiManager.NewFrame();

        // Отрисовываем интерфейс главного окна.
        mainWindow.Render();

        // Выполняем рендеринг ImGui-данных и обновляем окно.
        imguiManager.Render();
        glfwPollEvents();
    }
    
    return 0;
}
