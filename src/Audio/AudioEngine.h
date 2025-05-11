#pragma once
#include <string>
#include <vector>
#include <mutex>
#include <atomic>
#include <SDL.h>

class AudioEngine {
public:
    // Получить singleton-объект
    static AudioEngine* Instance();

    // Операции управления воспроизведением
    void play();
    void stop();
    void prev();
    void next();
    void addTrack(const std::string &filePath);
    void removeTrack(const std::string &filePath);
    std::string getCurrentTrack() const;
    const std::vector<std::string>& getPlaylist() const;

    // Функция обратного вызова для SDL-аудио
    void audioCallback(Uint8* stream, int len);
    static void audioCallbackWrapper(void* userdata, Uint8* stream, int len);

private:
    AudioEngine();
    ~AudioEngine();
    AudioEngine(const AudioEngine&) = delete;
    AudioEngine& operator=(const AudioEngine&) = delete;

    // Функция загрузки MP3 трека с использованием minimp3
    bool loadTrack(const std::string &filePath);

    // Плейлист и текущий индекс
    std::vector<std::string> playlist_;
    int currentTrackIndex_;

    // Декодированные PCM данные (16-bit, stereo) и позиция в буфере
    std::vector<int16_t> audioBuffer_;
    size_t audioBufferPos_;

    std::mutex mutex_;
    std::atomic<bool> isPlaying_;

    SDL_AudioDeviceID deviceId_;
};
