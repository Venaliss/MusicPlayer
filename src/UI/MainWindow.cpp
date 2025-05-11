#include "MainWindow.h"
#include "UI/ImGuiManager.h"
#include "Audio/AudioEngine.h"
#include "imgui.h"
#include "ImGuiFileDialog.h"  // убедитесь, что этот файл доступен в include-пути
#include <string>

static int selectedTrackIndex = -1;  // индекс выбранного трека, -1 если ни один не выбран

MainWindow::MainWindow() {
    // Инициализация состояния при необходимости
}

void MainWindow::Render() {
    ImGui::Begin("Music PLayer");

    // Вывод текущего трека
    std::string currentTrack = AudioEngine::Instance()->getCurrentTrack();
    ImGui::Text("Track name: %s", currentTrack.empty() ? "No track" : currentTrack.c_str());

    // Панель управления воспроизведением
    if (ImGui::Button("Prev"))
        AudioEngine::Instance()->prev();
    ImGui::SameLine();
    if (ImGui::Button("Stop"))
        AudioEngine::Instance()->stop();
    ImGui::SameLine();
    if (ImGui::Button("Play"))
        AudioEngine::Instance()->play();
    ImGui::SameLine();
    if (ImGui::Button("Next"))
        AudioEngine::Instance()->next();

    ImGui::Separator();
    ImGui::Text("Playlist");

    // Область показа плейлиста
    ImGui::BeginChild("PlaylistArea", ImVec2(300, 200), true);
    const auto& playlist = AudioEngine::Instance()->getPlaylist();
    for (size_t i = 0; i < playlist.size(); i++) {
        bool isSelected = (selectedTrackIndex == static_cast<int>(i));
        if (ImGui::Selectable(playlist[i].c_str(), isSelected)) {
            selectedTrackIndex = static_cast<int>(i);
        }
    }
    ImGui::EndChild();

    // Кнопки управления плейлистом: добавление и удаление
    if (ImGui::Button("Add")) {
        // Открываем диалог выбора файла с фильтром .mp3.
        // Четвёртым параметром передаём объект конфигурации по умолчанию.
        ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose mp3 file", ".mp3", IGFD::FileDialogConfig{});
    }
    ImGui::SameLine();
    if (ImGui::Button("Delete")) {
        if (selectedTrackIndex != -1 && selectedTrackIndex < static_cast<int>(playlist.size()))
            ImGui::OpenPopup("Really delete?");
    }

    // Диалог подтверждения удаления трека из плейлиста
    if (ImGui::BeginPopupModal("Really delete?", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Are you sure you want to remove this track from the playlist?");
        ImGui::Separator();
        if (ImGui::Button("Yep", ImVec2(120, 0))) {
            if (selectedTrackIndex != -1 && selectedTrackIndex < static_cast<int>(playlist.size())) {
                std::string trackToRemove = playlist[selectedTrackIndex];
                AudioEngine::Instance()->removeTrack(trackToRemove);
                selectedTrackIndex = -1; // сброс выбора
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Nah", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    // Отображение диалогового окна выбора файла
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            AudioEngine::Instance()->addTrack(filePathName);
        }
        ImGuiFileDialog::Instance()->Close();
    }

    ImGui::End();
}
