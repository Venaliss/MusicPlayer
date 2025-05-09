#ifndef IMGUI_MANAGER_H
#define IMGUI_MANAGER_H

class ImGuiManager {
public:
    static void Init();
    static void Shutdown();
    static void NewFrame();
};

#endif // IMGUI_MANAGER_H
