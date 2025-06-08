#include "MainWindow.h"
#include "UI/ImGuiManager.h"
#include "Audio/AudioEngine.h"
#include "imgui.h"
#include "ImGuiFileDialog.h" 
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

static int selectedTrackIndex = -1;  // индекс выбранного трека, -1 если ни один не выбран

MainWindow::MainWindow() {
    // Инициализация состояния при необходимости
}

void MainWindow::Render() {
    ImGui::Begin("Music Player");

    // Отображение имени текущего трека (получаем только имя файла)
    std::string currentTrack = AudioEngine::Instance()->getCurrentTrack();
    std::string currentTrackName = currentTrack.empty() ? "No track" : fs::path(currentTrack).filename().string();
    ImGui::Text("Track name: %s", currentTrackName.c_str());

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

    // Слайдер регулировки громкости
    static float volume = AudioEngine::Instance()->getVolume();
    if (ImGui::SliderFloat("Volume", &volume, 0.0f, 1.0f)) {
        AudioEngine::Instance()->setVolume(volume);
    }

    ImGui::Separator();

    // Слайдер времени трека
    int trackDuration = AudioEngine::Instance()->getTrackDuration();
    int currentTime = AudioEngine::Instance()->getCurrentTime();
    static int sliderTime = currentTime;
    if (ImGui::SliderInt("Track Time", &sliderTime, 0, trackDuration)) {
        AudioEngine::Instance()->setTrackPosition(sliderTime);
    }


    ImGui::Separator();
    ImGui::Text("Playlist");

    // Получаем вектор показываемых имен треков (без полного пути)
    std::vector<std::string> displayPlaylist = AudioEngine::Instance()->getDisplayPlaylist();
    ImGui::BeginChild("PlaylistArea", ImVec2(300, 200), true);
    for (size_t i = 0; i < displayPlaylist.size(); i++) {
        bool isSelected = (selectedTrackIndex == static_cast<int>(i));
        if (ImGui::Selectable(displayPlaylist[i].c_str(), isSelected))
            selectedTrackIndex = static_cast<int>(i);
    }
    ImGui::EndChild();

    // Кнопки управления плейлистом: добавление и удаление
    if (ImGui::Button("Add")) {
        // Открываем диалог выбора файла с фильтром для MP3, WAV и FLAC файлов.
        ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose audio file", ".mp3,.wav,.flac", {});
    }
    ImGui::SameLine();
    if (ImGui::Button("Delete")) {
        if (selectedTrackIndex != -1 && selectedTrackIndex < static_cast<int>(displayPlaylist.size()))
            ImGui::OpenPopup("Really delete?");
    }

    // Диалог подтверждения удаления трека из плейлиста
    if (ImGui::BeginPopupModal("Really delete?", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Are you sure you want to remove this track from the playlist?");
        ImGui::Separator();
        if (ImGui::Button("Yep", ImVec2(120, 0))) {
            // Для удаления используем полный путь из исходного плейлиста
            const auto &playlist = AudioEngine::Instance()->getPlaylist();
            if (selectedTrackIndex != -1 && selectedTrackIndex < static_cast<int>(playlist.size())) {
                std::string trackToRemove = playlist[selectedTrackIndex];
                AudioEngine::Instance()->removeTrack(trackToRemove);
                selectedTrackIndex = -1;
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Nah", ImVec2(120, 0)))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    // Отображение диалогового окна выбора файла для добавления трека
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            AudioEngine::Instance()->addTrack(filePathName);
        }
        ImGuiFileDialog::Instance()->Close();
    }

    ImGui::End();
}
