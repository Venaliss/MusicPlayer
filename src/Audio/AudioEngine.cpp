#include "AudioEngine.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <cstring>
#include <filesystem>
#include "Decoders/MP3Decoder.h"
#include "Audio/Interface/IAudioDecoder.h"
#include "Decoders/WavDecoder.h"
#include "Decoders/FlacDecoder.h"

namespace fs = std::filesystem;

// Singleton для AudioEngine
AudioEngine* AudioEngine::Instance() {
    static AudioEngine instance;
    return &instance;
}

AudioEngine::AudioEngine()
    : currentTrackIndex_(-1),
      audioBufferPos_(0),
      isPlaying_(false),
      volume_(1.0f) // Громкость по умолчанию = 100%
{
    // Инициализация аудио подсистемы SDL
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL audio initialization error: " << SDL_GetError() << std::endl;
    }
    
    SDL_AudioSpec desiredSpec;
    SDL_zero(desiredSpec);
    // Здесь устройство открывается с фиксированными параметрами (44100 Гц, 2 канала)
    desiredSpec.freq = 44100;           
    desiredSpec.format = AUDIO_S16LSB;    
    desiredSpec.channels = 2;             
    desiredSpec.samples = 4096;           
    desiredSpec.callback = AudioEngine::audioCallbackWrapper;
    desiredSpec.userdata = this;
    
    SDL_AudioSpec obtainedSpec;
    deviceId_ = SDL_OpenAudioDevice(nullptr, 0, &desiredSpec, &obtainedSpec, 0);
    if (deviceId_ == 0) {
        std::cerr << "Error opening audio device: " << SDL_GetError() << std::endl;
    } else {
        std::cout << "Audio device successfully opened: frequency " 
                  << obtainedSpec.freq << " Hz, channels: " 
                  << static_cast<int>(obtainedSpec.channels) << std::endl;
    }
}

AudioEngine::~AudioEngine() {
    if (deviceId_ != 0)
        SDL_CloseAudioDevice(deviceId_);
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void AudioEngine::addTrack(const std::string &filePath) {
    std::lock_guard<std::mutex> lock(mutex_);
    playlist_.push_back(filePath);
}

void AudioEngine::removeTrack(const std::string &filePath) {
    std::lock_guard<std::mutex> lock(mutex_);
    int removedIndex = -1;
    for (size_t i = 0; i < playlist_.size(); i++) {
         if (playlist_[i] == filePath) {
              removedIndex = static_cast<int>(i);
              break;
         }
    }
    if (removedIndex == -1)
        return; // трек не найден

    if (removedIndex == currentTrackIndex_) {
         stop();                     // останавливаем воспроизведение, если текущий трек удаляется
         currentTrackIndex_ = -1;    
    } else if (currentTrackIndex_ > removedIndex) {
         currentTrackIndex_--;
    }
    playlist_.erase(playlist_.begin() + removedIndex);
}

std::string AudioEngine::getCurrentTrack() const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (currentTrackIndex_ < 0 || currentTrackIndex_ >= static_cast<int>(playlist_.size()))
        return "";
    return playlist_[currentTrackIndex_];
}

const std::vector<std::string>& AudioEngine::getPlaylist() const {
    return playlist_;
}

std::vector<std::string> AudioEngine::getDisplayPlaylist() const {
    std::vector<std::string> displayList;
    for (const auto &fullPath : playlist_) {
        fs::path p(fullPath);
        displayList.push_back(p.filename().string());
    }
    return displayList;
}

void AudioEngine::setVolume(float vol) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (vol < 0.0f) vol = 0.0f;
    if (vol > 1.0f) vol = 1.0f;
    volume_ = vol;
}

float AudioEngine::getVolume() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return volume_;
}

void AudioEngine::play() {
    if (playlist_.empty())
        return;
    if (deviceId_ == 0) {
        std::cerr << "Audio device not available, cannot play track!" << std::endl;
        return;
    }
    SDL_LockAudioDevice(deviceId_);
    if (currentTrackIndex_ < 0 || currentTrackIndex_ >= static_cast<int>(playlist_.size()))
        currentTrackIndex_ = 0;
    if (!loadTrack(playlist_[currentTrackIndex_])) {
         SDL_UnlockAudioDevice(deviceId_);
         std::cerr << "Error loading track: " << playlist_[currentTrackIndex_] << std::endl;
         isPlaying_ = false;
         return;
    }
    audioBufferPos_ = 0;
    isPlaying_ = true;
    SDL_UnlockAudioDevice(deviceId_);
    SDL_PauseAudioDevice(deviceId_, 0); 
}

void AudioEngine::next() {
    if (playlist_.empty())
        return;
    if (deviceId_ == 0)
        return;
    SDL_LockAudioDevice(deviceId_);
    currentTrackIndex_ = (currentTrackIndex_ + 1) % playlist_.size();
    if (!loadTrack(playlist_[currentTrackIndex_])) {
         SDL_UnlockAudioDevice(deviceId_);
         std::cerr << "Error loading track: " << playlist_[currentTrackIndex_] << std::endl;
         isPlaying_ = false;
         return;
    }
    audioBufferPos_ = 0;
    SDL_UnlockAudioDevice(deviceId_);
    if (isPlaying_)
         SDL_PauseAudioDevice(deviceId_, 0);
}

void AudioEngine::prev() {
    if (playlist_.empty())
        return;
    if (deviceId_ == 0)
         return;
    SDL_LockAudioDevice(deviceId_);
    currentTrackIndex_ = (currentTrackIndex_ <= 0) ? playlist_.size() - 1 : currentTrackIndex_ - 1;
    if (!loadTrack(playlist_[currentTrackIndex_])) {
         SDL_UnlockAudioDevice(deviceId_);
         std::cerr << "Error loading track: " << playlist_[currentTrackIndex_] << std::endl;
         isPlaying_ = false;
         return;
    }
    audioBufferPos_ = 0;
    SDL_UnlockAudioDevice(deviceId_);
    if (isPlaying_)
         SDL_PauseAudioDevice(deviceId_, 0);
}

void AudioEngine::stop() {
    if (deviceId_ == 0)
         return;
    isPlaying_ = false;
    SDL_PauseAudioDevice(deviceId_, 1);
}

// Методы для работы с позицией и длительностью трека
int AudioEngine::getTrackDuration() const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (audioBuffer_.empty() || currentChannels_ <= 0 || currentSampleRate_ <= 0)
        return 0;
    int frames = static_cast<int>(audioBuffer_.size() / currentChannels_);
    return frames / currentSampleRate_;
}

int AudioEngine::getCurrentTime() const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (currentChannels_ <= 0 || currentSampleRate_ <= 0)
        return 0;
    int framesPlayed = static_cast<int>(audioBufferPos_ / currentChannels_);
    return framesPlayed / currentSampleRate_;
}

void AudioEngine::setTrackPosition(int seconds) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (currentSampleRate_ <= 0 || currentChannels_ <= 0)
        return;
    int newFrame = seconds * currentSampleRate_;
    int newSamplePos = newFrame * currentChannels_;
    if(newSamplePos < 0)
        newSamplePos = 0;
    if(newSamplePos > static_cast<int>(audioBuffer_.size()))
        newSamplePos = static_cast<int>(audioBuffer_.size());
    audioBufferPos_ = newSamplePos;
}

bool AudioEngine::loadTrack(const std::string &filePath) {
    // Проверяем, существует ли файл
    std::ifstream infile(filePath);
    if (!infile.good()) {
        std::cerr << "File not found: " << filePath << std::endl;
        return false;
    }
    infile.close();

    fs::path p(filePath);
    std::string ext = p.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    // Создаем декодер согласно расширению
    IAudioDecoder* decoder = nullptr;
    if (ext == ".mp3") {
        decoder = new MP3Decoder();
    } else if (ext == ".wav") {
        decoder = new WavDecoder();
    } else if (ext == ".flac") {
        decoder = new FlacDecoder();
    } else {
        std::cerr << "Unsupported file extension: " << ext << std::endl;
        return false;
    }
    
    if (decoder == nullptr || !decoder->load(filePath)) {
        std::cerr << "Error loading file with decoder: " << filePath << std::endl;
        delete decoder;
        return false;
    }
    
    std::vector<float> decodedBuffer;
    if (!decoder->decode(decodedBuffer)) {
        std::cerr << "Error decoding file: " << filePath << std::endl;
        delete decoder;
        return false;
    }
    
    // Получаем динамически метаданные из декодера
    currentSampleRate_ = decoder->getSampleRate();
    currentChannels_ = decoder->getChannels();
    totalFrames_ = decoder->getTotalFrames(); // Если декодер их предоставляет; иначе можно вычислить: decodedBuffer.size()/currentChannels_
    
    // Преобразуем float сэмплы ([-1.0, 1.0]) в int16_t
    std::vector<int16_t> intBuffer(decodedBuffer.size());
    for (size_t i = 0; i < decodedBuffer.size(); i++) {
        float sample = decodedBuffer[i];
        int sampleInt = static_cast<int>(sample * 32767.0f);
        if (sampleInt > 32767) sampleInt = 32767;
        if (sampleInt < -32768) sampleInt = -32768;
        intBuffer[i] = static_cast<int16_t>(sampleInt);
    }
    
    {
        std::lock_guard<std::mutex> lock(mutex_);
        audioBuffer_ = std::move(intBuffer);
        audioBufferPos_ = 0;
    }
    
    std::cout << "Track successfully loaded using decoder: " << filePath 
              << ", buffer length: " << audioBuffer_.size() << " samples" << std::endl;
    
    delete decoder;
    return true;
}

void AudioEngine::audioCallback(Uint8* stream, int len) {
    std::lock_guard<std::mutex> lock(mutex_);
    SDL_memset(stream, 0, len);
    if (!isPlaying_ || audioBuffer_.empty())
         return;
    
    // Здесь используется фиксированное число каналов, так как аудиоустройство открыто для 2-х каналов.
    // Если необходимо поддерживать динамическое число каналов, потребуется ресемплирование.
    int bytesPerSample = sizeof(int16_t);
    int outputChannels = 2; // выходное значение аудиоустройства
    int bytesPerFrame = bytesPerSample * outputChannels;
    int framesToWrite = len / bytesPerFrame;

    int16_t* out = reinterpret_cast<int16_t*>(stream);
    for (int i = 0; i < framesToWrite; i++) {
        if (audioBufferPos_ + outputChannels <= audioBuffer_.size()) {
            for (int ch = 0; ch < outputChannels; ch++) {
                int sample = audioBuffer_[audioBufferPos_ + ch];
                sample = static_cast<int>(sample * volume_);
                if (sample > 32767) sample = 32767;
                if (sample < -32768) sample = -32768;
                out[i * outputChannels + ch] = static_cast<int16_t>(sample);
            }
            audioBufferPos_ += outputChannels;
        } else {
            isPlaying_ = false;
            break;
        }
    }
}

void AudioEngine::audioCallbackWrapper(void* userdata, Uint8* stream, int len) {
    AudioEngine* engine = static_cast<AudioEngine*>(userdata);
    engine->audioCallback(stream, len);
}
