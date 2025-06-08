#include "MP3Decoder.h"
#include "minimp3_ex.h"
#include <cstring>
#include <cstdlib>

MP3Decoder::MP3Decoder() : sampleRate(0), channels(0), totalSamples(0) {
}

MP3Decoder::~MP3Decoder() {
}

bool MP3Decoder::load(const std::string& filename) {
    trackTitle = filename;
    return true;
}

bool MP3Decoder::decode(std::vector<float>& outBuffer) {
    if (trackTitle.empty()) {
        return false;
    }
    
    mp3dec_t dec;
    mp3dec_init(&dec);
    
    mp3dec_file_info_t info;
    std::memset(&info, 0, sizeof(info));

    int err = mp3dec_load(&dec, trackTitle.c_str(), &info, nullptr, 0);
    if (err != 0) {
        return false;
    }
    
    // Сохраним метаданные
    sampleRate = info.hz;
    channels = info.channels;
    totalSamples = info.samples; // Предполагается, что info.samples содержит общее число сэмплов

    outBuffer.resize(info.samples);
    int16_t* samples = info.buffer;
    for (size_t i = 0; i < info.samples; i++) {
        outBuffer[i] = samples[i] / 32768.0f;
    }
    
    free(info.buffer);
    return true;
}

std::string MP3Decoder::getTrackTitle() const {
    return trackTitle;
}

int MP3Decoder::getSampleRate() const {
    return sampleRate;
}

int MP3Decoder::getChannels() const {
    return channels;
}

int MP3Decoder::getTotalFrames() const {
    if (channels > 0)
        return totalSamples / channels;
    return 0;
}
