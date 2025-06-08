#include "WavDecoder.h"
#include <iostream>
#include <filesystem>

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

namespace fs = std::filesystem;

WavDecoder::WavDecoder() : m_loaded(false) {
}

WavDecoder::~WavDecoder() {
    if (m_loaded) {
        drwav_uninit(&m_wav);
    }
}

bool WavDecoder::load(const std::string& filename) {
    m_filename = filename;
    if (!drwav_init_file(&m_wav, filename.c_str(), nullptr)) {
        std::cerr << "Failed to open WAV file: " << filename << std::endl;
        m_loaded = false;
        return false;
    }
    m_loaded = true;
    return true;
}

bool WavDecoder::decode(std::vector<float>& outBuffer) {
    if (!m_loaded) {
        std::cerr << "WAV file not loaded." << std::endl;
        return false;
    }
    
    drwav_uint64 totalFrames = m_wav.totalPCMFrameCount;
    unsigned int channels = m_wav.channels;
    outBuffer.resize(totalFrames * channels);
    
    // Чтение всех фреймов в виде float.
    drwav_uint64 framesRead = drwav_read_pcm_frames_f32(&m_wav, totalFrames, outBuffer.data());
    return (framesRead == totalFrames);
}

std::string WavDecoder::getTrackTitle() const {
    return fs::path(m_filename).filename().string();
}

int WavDecoder::getSampleRate() const {
    if (m_loaded) {
        return static_cast<int>(m_wav.sampleRate);
    }
    return 0;
}

int WavDecoder::getChannels() const {
    if (m_loaded) {
        return static_cast<int>(m_wav.channels);
    }
    return 0;
}

int WavDecoder::getTotalFrames() const {
    if (m_loaded) {
        return static_cast<int>(m_wav.totalPCMFrameCount);
    }
    return 0;
}
