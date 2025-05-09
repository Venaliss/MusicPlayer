#include "AudioEngine.h"
#include <iostream>

AudioEngine* AudioEngine::Instance() {
    static AudioEngine engine;
    return &engine;
}

AudioEngine::AudioEngine() : currentTrackIndex(-1) {}

void AudioEngine::play() {
    if (!playlist.empty()) {
        if (currentTrackIndex < 0)
            currentTrackIndex = 0;
        std::cout << "Воспроизведение: " << playlist[currentTrackIndex] << std::endl;
    }
}

void AudioEngine::stop() {
    std::cout << "Остановка воспроизведения" << std::endl;
}

void AudioEngine::next() {
    if (!playlist.empty()) {
        currentTrackIndex = (currentTrackIndex + 1) % playlist.size();
        play();
    }
}

void AudioEngine::prev() {
    if (!playlist.empty()) {
        currentTrackIndex = (currentTrackIndex - 1 + playlist.size()) % playlist.size();
        play();
    }
}

void AudioEngine::addTrack(const std::string& filename) {
    playlist.push_back(filename);
    if (currentTrackIndex == -1)
        currentTrackIndex = 0;
}

void AudioEngine::removeTrack(const std::string& filename) {
    auto it = std::find(playlist.begin(), playlist.end(), filename);
    if (it != playlist.end()) {
        int index = static_cast<int>(std::distance(playlist.begin(), it));
        playlist.erase(it);
        if (currentTrackIndex >= static_cast<int>(playlist.size()))
            currentTrackIndex = playlist.empty() ? -1 : 0;
    }
}

const std::vector<std::string>& AudioEngine::getPlaylist() const {
    return playlist;
}

std::string AudioEngine::getCurrentTrack() const {
    if (currentTrackIndex >= 0 && currentTrackIndex < static_cast<int>(playlist.size()))
        return playlist[currentTrackIndex];
    return "";
}
