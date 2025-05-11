#include "AudioEngine.h"
#include <iostream>
#include <fstream>    // для проверки существования файла
#include <cmath>
#include <algorithm>
#include <cstring>
#include "minimp3_ex.h"  // убедитесь, что в одном файле проекта определён MINIMP3_IMPLEMENTATION

AudioEngine* AudioEngine::Instance() {
    static AudioEngine instance;
    return &instance;
}

AudioEngine::AudioEngine()
    : currentTrackIndex_(-1), audioBufferPos_(0), isPlaying_(false)
{
    // Инициализация аудио подсистемы SDL.
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        std::cerr << "Ошибка инициализации SDL аудио: " << SDL_GetError() << std::endl;
    }
    
    SDL_AudioSpec desiredSpec;
    SDL_zero(desiredSpec);
    desiredSpec.freq = 44100;           // Рабочая частота 44100 Гц
    desiredSpec.format = AUDIO_S16LSB;    // 16-bit PCM
    desiredSpec.channels = 2;             // Стерео
    desiredSpec.samples = 4096;           // Размер буфера
    desiredSpec.callback = AudioEngine::audioCallbackWrapper;
    desiredSpec.userdata = this;
    
    SDL_AudioSpec obtainedSpec;
    deviceId_ = SDL_OpenAudioDevice(nullptr, 0, &desiredSpec, &obtainedSpec, 0);
    if (deviceId_ == 0) {
        std::cerr << "Ошибка открытия аудио устройства: " << SDL_GetError() << std::endl;
    } else {
        std::cout << "Аудиоустройство успешно открыто: частота " 
                  << obtainedSpec.freq << " Гц, каналы: " 
                  << static_cast<int>(obtainedSpec.channels) << std::endl;
    }
}

AudioEngine::~AudioEngine() {
    SDL_CloseAudioDevice(deviceId_);
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void AudioEngine::addTrack(const std::string &filePath) {
    std::lock_guard<std::mutex> lock(mutex_);
    playlist_.push_back(filePath);
}

void AudioEngine::removeTrack(const std::string &filePath) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = std::find(playlist_.begin(), playlist_.end(), filePath);
    if(it != playlist_.end()){
        playlist_.erase(it);
    }
    if (currentTrackIndex_ >= static_cast<int>(playlist_.size())) {
        currentTrackIndex_ = -1;
    }
}

std::string AudioEngine::getCurrentTrack() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(mutex_));
    if (currentTrackIndex_ < 0 || currentTrackIndex_ >= static_cast<int>(playlist_.size()))
        return "";
    return playlist_[currentTrackIndex_];
}

const std::vector<std::string>& AudioEngine::getPlaylist() const {
    return playlist_;
}

void AudioEngine::prev() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (playlist_.empty())
        return;
    currentTrackIndex_ = (currentTrackIndex_ <= 0) ? playlist_.size() - 1 : currentTrackIndex_ - 1;
    if (!loadTrack(playlist_[currentTrackIndex_])) {
         std::cerr << "Ошибка загрузки трека: " << playlist_[currentTrackIndex_] << std::endl;
    }
}

void AudioEngine::next() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (playlist_.empty())
        return;
    currentTrackIndex_ = (currentTrackIndex_ + 1) % playlist_.size();
    if (!loadTrack(playlist_[currentTrackIndex_])) {
         std::cerr << "Ошибка загрузки трека: " << playlist_[currentTrackIndex_] << std::endl;
    }
}

void AudioEngine::play() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (playlist_.empty())
        return;
    if (currentTrackIndex_ < 0 || currentTrackIndex_ >= static_cast<int>(playlist_.size()))
        currentTrackIndex_ = 0;
    
    // Загружаем трек. Если загрузка не удалась, выводим сообщение.
    if (!loadTrack(playlist_[currentTrackIndex_])) {
         std::cerr << "Ошибка загрузки трека: " << playlist_[currentTrackIndex_] << std::endl;
         return;
    }
    isPlaying_ = true;
    SDL_PauseAudioDevice(deviceId_, 0);  // Запускаем воспроизведение
}

void AudioEngine::stop() {
    std::lock_guard<std::mutex> lock(mutex_);
    isPlaying_ = false;
    SDL_PauseAudioDevice(deviceId_, 1);  // Останавливаем воспроизведение
}

// Функция загрузки MP3 трека с использованием minimp3
bool AudioEngine::loadTrack(const std::string &filePath) {
    // Проверяем, существует ли файл
    std::ifstream infile(filePath);
    if (!infile.good()) {
        std::cerr << "Файл не найден: " << filePath << std::endl;
        return false;
    }
    infile.close();
    
    mp3dec_t dec;
    mp3dec_init(&dec);

    mp3dec_file_info_t info;
    std::memset(&info, 0, sizeof(info));
    
    // Загрузка MP3 файла. Возвращает 0 при успехе.
    if (mp3dec_load(&dec, filePath.c_str(), &info, nullptr, 0) != 0) {
         std::cerr << "Ошибка декодирования MP3 файла: " << filePath << std::endl;
         return false;
    }
    
    // Проверяем частоту дискретизации – в данном примере поддерживаются только 44100 Гц.
    if (info.hz != 44100) {
         std::cerr << "Неподдерживаемая частота дискретизации: " << info.hz 
                   << " Гц. Файл: " << filePath << std::endl;
         free(info.buffer);
         return false;
    }
    
    std::vector<int16_t> buffer;
    
    // Если файл моно, преобразуем в стерео (дублируем каждый сэмпл)
    if (info.channels == 1) {
         buffer.resize(info.samples * 2);
         mp3d_sample_t *samples_ptr = info.buffer;
         for (size_t i = 0; i < info.samples; i++) {
             buffer[i * 2]     = samples_ptr[i];
             buffer[i * 2 + 1] = samples_ptr[i];
         }
    }
    // Если файл стереофонический, копируем данные (info.samples уже включает оба канала)
    else if (info.channels == 2) {
         buffer.resize(info.samples);
         std::memcpy(buffer.data(), info.buffer, info.samples * sizeof(mp3d_sample_t));
    }
    else {
         std::cerr << "Неподдерживаемое количество каналов (" << info.channels 
                   << ") в файле: " << filePath << std::endl;
         free(info.buffer);
         return false;
    }
    
    {
         std::lock_guard<std::mutex> lock(mutex_);
         audioBuffer_ = std::move(buffer);
         audioBufferPos_ = 0;
    }
    
    free(info.buffer);
    std::cout << "Трек успешно загружен: " << filePath << ", " 
              << "длина буфера: " << audioBuffer_.size() << " сэмплов" << std::endl;
    return true;
}

void AudioEngine::audioCallback(Uint8* stream, int len) {
    std::lock_guard<std::mutex> lock(mutex_);
    SDL_memset(stream, 0, len);
    if (!isPlaying_)
         return;
    
    int bytesPerSample = sizeof(int16_t);
    int channels = 2;
    int bytesPerFrame = bytesPerSample * channels;
    int framesToWrite = len / bytesPerFrame;
    
    for (int i = 0; i < framesToWrite; i++) {
        if (audioBufferPos_ < audioBuffer_.size()) {
            SDL_memcpy(stream + i * bytesPerFrame,
                       &audioBuffer_[audioBufferPos_],
                       bytesPerFrame);
            audioBufferPos_ += channels;
        } else {
            // Достигнут конец буфера
            isPlaying_ = false;
            break;
        }
    }
}

void AudioEngine::audioCallbackWrapper(void* userdata, Uint8* stream, int len) {
    AudioEngine* engine = static_cast<AudioEngine*>(userdata);
    engine->audioCallback(stream, len);
}
