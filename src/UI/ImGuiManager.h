#ifndef IMGUI_MANAGER_H
#define IMGUI_MANAGER_H

#include "GLFW/glfw3.h"

class ImGuiManager {
public:
    ImGuiManager();
    ~ImGuiManager();

    bool Init();
    void NewFrame();
    void Render();
    bool WindowShouldClose();
    void Cleanup();

private:
    GLFWwindow* window;
    bool m_cleanedUp; // Флаг, отслеживающий, выполнена ли очистка
};

#endif // IMGUI_MANAGER_H
