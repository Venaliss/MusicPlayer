#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H

#include <string>
#include <vector>
#include <mutex>
#include "SDL.h"

class AudioEngine {
public:
    static AudioEngine* Instance();

    // Управление плейлистом.
    void addTrack(const std::string &filePath);
    void removeTrack(const std::string &filePath);
    std::string getCurrentTrack() const;
    const std::vector<std::string>& getPlaylist() const;

    // Новая функция: возвращает вектор с отображаемыми именами треков.
    std::vector<std::string> getDisplayPlaylist() const;

    // Управление воспроизведением.
    void play();
    void stop();
    void next();
    void prev();

    // Управление громкостью.
    void setVolume(float vol);
    float getVolume() const;

private:
    AudioEngine();
    ~AudioEngine();
    bool loadTrack(const std::string &filePath);
    void audioCallback(Uint8* stream, int len);
    static void audioCallbackWrapper(void* userdata, Uint8* stream, int len);

    SDL_AudioDeviceID deviceId_;
    std::vector<std::string> playlist_;
    int currentTrackIndex_;
    std::vector<int16_t> audioBuffer_;
    size_t audioBufferPos_;
    bool isPlaying_;
    float volume_; // Громкость в диапазоне [0.0, 1.0]
    mutable std::mutex mutex_;
};

#endif // AUDIOENGINE_H
