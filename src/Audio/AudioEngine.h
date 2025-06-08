#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H

#include <string>
#include <vector>
#include <mutex>
#include "SDL.h"

// реализует воспроизведение, управление плейлистом и позволяет
// получать длительность и текущую позицию трека на основе динамических метаданных.
class AudioEngine {
public:
    static AudioEngine* Instance();

    // Управление плейлистом.
    void addTrack(const std::string &filePath);
    void removeTrack(const std::string &filePath);
    std::string getCurrentTrack() const;
    const std::vector<std::string>& getPlaylist() const;

    // Возвращает вектор с отображаемыми именами треков.
    std::vector<std::string> getDisplayPlaylist() const;

    // Управление воспроизведением.
    void play();
    void stop();
    void next();
    void prev();

    // Управление положением трека.
    // Возвращает длительность текущего трека (в секундах).
    int getTrackDuration() const;
    // Возвращает текущую позицию воспроизведения (в секундах).
    int getCurrentTime() const;
    // Перематывает трек на заданное время (в секундах).
    void setTrackPosition(int seconds);

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

    // поля для динамических параметров текущего файла:
    int currentSampleRate_; // Частота дискретизации текущего файла
    int currentChannels_;   // Количество аудиоканалов текущего файла
    int totalFrames_;       // Общее число аудио-фреймов текущего файла

    mutable std::mutex mutex_;
};

#endif // AUDIOENGINE_H
