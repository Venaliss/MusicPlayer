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
    ImGui::Begin("MusicPlayer");
    
    // Отображение текущего трека
    std::string currentTrack = AudioEngine::Instance()->getCurrentTrack();
    ImGui::Text("Track name: %s", currentTrack.empty() ? "No track" : currentTrack.c_str());
    
    // Панель управления воспроизведением
    if (ImGui::Button("After")) {
        AudioEngine::Instance()->prev();
    }
    ImGui::SameLine();
    if (ImGui::Button("Stop")) {
        AudioEngine::Instance()->stop();
    }
    ImGui::SameLine();
    if (ImGui::Button("Play")) {
        AudioEngine::Instance()->play();
    }
    ImGui::SameLine();
    if (ImGui::Button("Next")) {
        AudioEngine::Instance()->next();
    }
    
    ImGui::Separator();
    ImGui::Text("Playlist");
    
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
    if (ImGui::Button("Add")) {
        // Здесь должна вызываться функция открытия диалога выбора файла.
        // Используйте ImGuiFileDialog или аналогичный подход.
        // В данном примере просто добавляем тестовый трек.
        AudioEngine::Instance()->addTrack("new_track.mp3");
    }
    ImGui::SameLine();
    if (ImGui::Button("Delete")) {
        if (selectedTrackIndex != -1 && selectedTrackIndex < static_cast<int>(playlist.size()))
            ImGui::OpenPopup("Really delete?");
    }
    
    // Диалог подтверждения удаления трека
    if (ImGui::BeginPopupModal("Really delete?", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("You want delete the track?");
        ImGui::Separator();
        if (ImGui::Button("Yes", ImVec2(120, 0))) {
            if (selectedTrackIndex != -1 && selectedTrackIndex < static_cast<int>(playlist.size())) {
                std::string trackToRemove = playlist[selectedTrackIndex];
                AudioEngine::Instance()->removeTrack(trackToRemove);
                selectedTrackIndex = -1; // сброс выбора
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("No", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    
    ImGui::End();
}
