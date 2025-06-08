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
}

void MainWindow::Render() {
    ImGuiIO& io = ImGui::GetIO();
    io.FontGlobalScale = 1.0f;         
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f); 

    ImGui::Begin("Music Player", nullptr, ImGuiWindowFlags_NoResize);

    // Отображение имени текущего трека
    std::string currentTrack = AudioEngine::Instance()->getCurrentTrack();
    std::string currentTrackName = currentTrack.empty() ? "No track" : fs::path(currentTrack).filename().string();
    ImGui::Text("Track: %s", currentTrackName.c_str());

    // Панель управления воспроизведением
    if (ImGui::Button("Prev"))
        AudioEngine::Instance()->prev();
    ImGui::SameLine();
    if (ImGui::Button("Stop"))
        AudioEngine::Instance()->stop();
    ImGui::SameLine();
    if (ImGui::Button("Play"))
        AudioEngine::Instance()->play(); // play() в AudioEngine обнуляет позицию воспроизведения
    ImGui::SameLine();
    if (ImGui::Button("Next"))
        AudioEngine::Instance()->next();

    ImGui::Separator();

    // Слайдер громкости
    static float volume = AudioEngine::Instance()->getVolume();
    if (ImGui::SliderFloat("Volume", &volume, 0.0f, 1.0f)) {
        AudioEngine::Instance()->setVolume(volume);
    }

    ImGui::Separator();

    // Слайдер времени трека
    // отдельная переменная sliderTime, которая обновляется, если пользователь не перетаскивает ползунок
    int trackDuration = AudioEngine::Instance()->getTrackDuration(); // длительность трека (сек)
    int currentTime = AudioEngine::Instance()->getCurrentTime();       // текущее время (сек)
    static int sliderTime = 0;
    static bool isDragging = false;

    // Начало нового трека: если текущий трек изменился и getCurrentTime() равен 0, сбрасывает sliderTime
    if (!isDragging && currentTime == 0) {
        sliderTime = 0;
    }
    
    // Отображает слайдер и отслеживает, перетаскивает ли пользователь его
    ImGui::PushItemWidth(300);
    if (ImGui::SliderInt("Time", &sliderTime, 0, trackDuration)) {
        // Если значение изменилось, обновляем позицию
        AudioEngine::Instance()->setTrackPosition(sliderTime);
    }
    // Отслеживание активности слайдера, чтобы не затирать значение sliderTime во время перетаскивания
    if (ImGui::IsItemActive())
        isDragging = true;
    else
    {
        isDragging = false;
        // Если пользователь не перетаскивает, обновляем значение из AudioEngine,
        sliderTime = currentTime;
    }
    ImGui::PopItemWidth();

    ImGui::Separator();
    ImGui::Text("Playlist");

    // Отображаем список треков
    std::vector<std::string> displayPlaylist = AudioEngine::Instance()->getDisplayPlaylist();
    ImGui::BeginChild("PlaylistArea", ImVec2(300, 200), true);
    for (size_t i = 0; i < displayPlaylist.size(); i++) {
        bool isSelected = (selectedTrackIndex == static_cast<int>(i));
        if (ImGui::Selectable(displayPlaylist[i].c_str(), isSelected))
            selectedTrackIndex = static_cast<int>(i);
    }
    ImGui::EndChild();

    // Кнопки добавления и удаления треков из плейлиста
    if (ImGui::Button("Add")) {
        // Диалог выбора файла с фильтром для mp3, wav, flac.
        ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose an audio file", ".mp3,.wav,.flac", {});
    }
    ImGui::SameLine();
    if (ImGui::Button("Delete")) {
        if (selectedTrackIndex != -1 && selectedTrackIndex < static_cast<int>(displayPlaylist.size())) {
            ImGui::OpenPopup("Really delete?");
        }
    }

    // Диалог подтверждения удаления трека
    if (ImGui::BeginPopupModal("Really delete?", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Are you sure you want to remove this track?");
        ImGui::Separator();
        if (ImGui::Button("Yes", ImVec2(120, 0))) {
            const auto &playlist = AudioEngine::Instance()->getPlaylist();
            if (selectedTrackIndex != -1 && selectedTrackIndex < static_cast<int>(playlist.size())) {
                std::string trackToRemove = playlist[selectedTrackIndex];
                AudioEngine::Instance()->removeTrack(trackToRemove);
                selectedTrackIndex = -1;
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("No", ImVec2(120, 0)))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    // Диалог добавления файла
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            AudioEngine::Instance()->addTrack(filePathName);
        }
        ImGuiFileDialog::Instance()->Close();
    }
    
    ImGui::End();
}
