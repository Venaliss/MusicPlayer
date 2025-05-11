#include "AudioEngine.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <cstring>
#include "minimp3_ex.h"  // Убедитесь, что MINIMP3_IMPLEMENTATION определён только в одном файле

// Singleton for AudioEngine
AudioEngine* AudioEngine::Instance() {
    static AudioEngine instance;
    return &instance;
}

AudioEngine::AudioEngine()
    : currentTrackIndex_(-1), audioBufferPos_(0), isPlaying_(false)
{
    // Initialize SDL audio subsystem.
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL audio initialization error: " << SDL_GetError() << std::endl;
    }
    
    SDL_AudioSpec desiredSpec;
    SDL_zero(desiredSpec);
    desiredSpec.freq = 44100;           // Frequency 44100 Hz
    desiredSpec.format = AUDIO_S16LSB;    // 16-bit PCM
    desiredSpec.channels = 2;             // Stereo
    desiredSpec.samples = 4096;           // Buffer size
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
    auto it = std::find(playlist_.begin(), playlist_.end(), filePath);
    if (it != playlist_.end()) {
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

// --- Updated functions using SDL audio locks ---

void AudioEngine::play() {
    if (playlist_.empty())
        return;
    if (deviceId_ == 0) {
        std::cerr << "Audio device not available, cannot play track!" << std::endl;
        return;
    }
    // Lock audio device to pause the callback and avoid concurrent buffer access
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
    SDL_PauseAudioDevice(deviceId_, 0);  // Start playback
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
    SDL_PauseAudioDevice(deviceId_, 1);  // Stop playback
}

// --- LoadTrack remains similar, but ensure full update is protected ---
bool AudioEngine::loadTrack(const std::string &filePath) {
    // Check file existence
    std::ifstream infile(filePath);
    if (!infile.good()) {
        std::cerr << "File not found: " << filePath << std::endl;
        return false;
    }
    infile.close();

    mp3dec_t dec;
    mp3dec_init(&dec);

    mp3dec_file_info_t info;
    std::memset(&info, 0, sizeof(info));

    if (mp3dec_load(&dec, filePath.c_str(), &info, nullptr, 0) != 0) {
         std::cerr << "Error decoding MP3 file: " << filePath << std::endl;
         return false;
    }

    if (info.hz != 44100) {
         std::cerr << "Unsupported frequency (" << info.hz << " Hz) in file: " << filePath << std::endl;
         free(info.buffer);
         return false;
    }

    if (info.samples == 0 || info.buffer == nullptr) {
         std::cerr << "No samples available in file: " << filePath << std::endl;
         if (info.buffer)
             free(info.buffer);
         return false;
    }
    
    std::vector<int16_t> buffer;
    if (info.channels == 1) {
         buffer.resize(info.samples * 2);
         mp3d_sample_t *samples_ptr = info.buffer;
         for (size_t i = 0; i < info.samples; i++) {
             buffer[i * 2]     = samples_ptr[i];
             buffer[i * 2 + 1] = samples_ptr[i];
         }
    } else if (info.channels == 2) {
         buffer.resize(info.samples);
         std::memcpy(buffer.data(), info.buffer, info.samples * sizeof(mp3d_sample_t));
    } else {
         std::cerr << "Unsupported number of channels (" << info.channels << ") in file: " << filePath << std::endl;
         free(info.buffer);
         return false;
    }
    
    {
         std::lock_guard<std::mutex> lock(mutex_);
         audioBuffer_ = std::move(buffer);
         audioBufferPos_ = 0;
    }
    
    free(info.buffer);
    std::cout << "Track successfully loaded: " << filePath 
              << ", buffer length: " << audioBuffer_.size() << " samples" << std::endl;
    return true;
}

void AudioEngine::audioCallback(Uint8* stream, int len) {
    // Callback uses mutex to safely read data.
    std::lock_guard<std::mutex> lock(mutex_);
    SDL_memset(stream, 0, len);
    if (!isPlaying_ || audioBuffer_.empty())
         return;
    
    int bytesPerSample = sizeof(int16_t);
    int channels = 2;
    int bytesPerFrame = bytesPerSample * channels;
    int framesToWrite = len / bytesPerFrame;

    for (int i = 0; i < framesToWrite; i++) {
        if (audioBufferPos_ + channels <= audioBuffer_.size()) {
            SDL_memcpy(stream + i * bytesPerFrame, &audioBuffer_[audioBufferPos_], bytesPerFrame);
            audioBufferPos_ += channels;
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
