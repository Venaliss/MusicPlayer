#include "MainWindow.h"
#include "Audio/AudioEngine.h"
#include "imgui.h"
#include <string>

// Здесь можно использовать библиотеку ImGuiFileDialog для реального выбора файлов.
// В данном примере добавление трека симулируется, т.е. всегда добавляется "НовыйТрек.mp3".

// Индекс выбранного трека в плейлисте (если не выбран – значение -1)
static int selectedTrackIndex = -1;

MainWindow::MainWindow() {
    // Инициализация состояния (при необходимости)
}

void MainWindow::Render() {
    ImGui::Begin("Музыкальный плеер");
    
    // Отображение текущего трека
    std::string currentTrack = AudioEngine::Instance()->getCurrentTrack();
    ImGui::Text("Название трека: %s", currentTrack.empty() ? "Нет трека" : currentTrack.c_str());
    
    // Панель управления воспроизведением
    if (ImGui::Button("Предыдущий")) {
        AudioEngine::Instance()->prev();
    }
    ImGui::SameLine();
    if (ImGui::Button("Стоп")) {
        AudioEngine::Instance()->stop();
    }
    ImGui::SameLine();
    if (ImGui::Button("Проигрывать")) {
        AudioEngine::Instance()->play();
    }
    ImGui::SameLine();
    if (ImGui::Button("Следующий")) {
        AudioEngine::Instance()->next();
    }
    
    ImGui::Separator();
    ImGui::Text("Плейлист");
    
    // Область вывода плейлиста
    ImGui::BeginChild("PlaylistArea", ImVec2(300, 200), true);
    const auto& playlist = AudioEngine::Instance()->getPlaylist();
    for (size_t i = 0; i < playlist.size(); i++) {
        bool isSelected = (selectedTrackIndex == static_cast<int>(i));
        if (ImGui::Selectable(playlist[i].c_str(), isSelected)) {
            selectedTrackIndex = static_cast<int>(i);
        }
    }
    ImGui::EndChild();
    
    // Кнопки "Добавить" и "Убрать"
    if (ImGui::Button("Добавить")) {
        // Здесь должна вызываться функция открытия диалога выбора файла.
        // Используйте ImGuiFileDialog или аналогичный подход.
        // В данном примере просто добавляем тестовый трек.
        AudioEngine::Instance()->addTrack("НовыйТрек.mp3");
    }
    ImGui::SameLine();
    if (ImGui::Button("Убрать")) {
        if (selectedTrackIndex != -1 && selectedTrackIndex < static_cast<int>(playlist.size()))
            ImGui::OpenPopup("Подтверждение удаления");
    }
    
    // Диалог подтверждения удаления трека
    if (ImGui::BeginPopupModal("Подтверждение удаления", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Вы уверены, что хотите убрать этот трек из плейлиста?");
        ImGui::Separator();
        if (ImGui::Button("Да", ImVec2(120, 0))) {
            if (selectedTrackIndex != -1 && selectedTrackIndex < static_cast<int>(playlist.size())) {
                std::string trackToRemove = playlist[selectedTrackIndex];
                AudioEngine::Instance()->removeTrack(trackToRemove);
                selectedTrackIndex = -1; // сброс выбора
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Нет", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    
    ImGui::End();
}
