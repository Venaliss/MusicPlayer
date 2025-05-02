#include "MainWindow.h"
#include "imgui.h"
#include <cstdio>

void MainWindow::Render()
{
    // Настройки стиля перед началом отрисовки окна
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.08f, 0.08f, 0.08f, 1.0f)); // Очень темный фон

    // Задаем размеры и позицию окна
    ImGui::SetNextWindowSize(ImVec2(400, 600), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(100, 50), ImGuiCond_Once);
    ImGui::Begin("Music Player", nullptr, ImGuiWindowFlags_NoResize);

    // Верхняя панель: название песни и исполнитель + время
    {
        ImGui::PushFont(nullptr); // Можно заменить на кастомный шрифт, если установлен
        ImGui::Text("Shoot Love");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Maroon 5");
        float contentWidth = ImGui::GetContentRegionAvail().x;
        ImGui::SameLine(contentWidth - 50);
        ImGui::Text("12:30");
        ImGui::PopFont();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Центральная графика: круговая обложка
    {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 pos = ImGui::GetCursorScreenPos();
        float radius = 120.0f;
        ImVec2 center = ImVec2(pos.x + radius, pos.y + radius);
        draw_list->AddCircleFilled(center, radius, IM_COL32(220, 220, 220, 255));
        draw_list->AddCircle(center, radius, IM_COL32(0, 200, 0, 255), 32, 6.0f);
        ImGui::Dummy(ImVec2(radius * 2, radius * 2));
    }

    ImGui::Spacing();

    // Иконки под обложкой
    {
        const float iconSize = 50.0f;
        if (ImGui::Button("♥", ImVec2(iconSize, iconSize))) { }
        ImGui::SameLine();
        if (ImGui::Button("EQ", ImVec2(iconSize, iconSize))) { }
        ImGui::SameLine();
        if (ImGui::Button("⋮", ImVec2(iconSize, iconSize))) { }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Прогресс-бар с временными метками
    {
        ImGui::Text("2:50");
        ImGui::SameLine();
        float progress = 170.0f / 220.0f;
        ImGui::ProgressBar(progress, ImVec2(0, 20));
        ImGui::SameLine();
        ImGui::Text("3:40");
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Нижняя панель управления
    {
        const float btnSize = 60.0f;
        // Можно добавить дополнительные настройки для кнопок, если надо
        if (ImGui::Button("🔀", ImVec2(btnSize, btnSize))) { }
        ImGui::SameLine();
        if (ImGui::Button("⏮", ImVec2(btnSize, btnSize))) { }
        ImGui::SameLine();
        if (ImGui::Button("⏯", ImVec2(btnSize, btnSize))) { }
        ImGui::SameLine();
        if (ImGui::Button("⏭", ImVec2(btnSize, btnSize))) { }
        ImGui::SameLine();
        if (ImGui::Button("📃", ImVec2(btnSize, btnSize))) { }
    }

    ImGui::End();
    ImGui::PopStyleColor(); // Откат цвета окна
}
