#include "FlacDecoder.h"
#include <iostream>
#include <filesystem>

#define DR_FLAC_IMPLEMENTATION
#include "dr_flac.h"

namespace fs = std::filesystem;

FlacDecoder::FlacDecoder() : m_pFlac(nullptr), m_loaded(false) {
}

FlacDecoder::~FlacDecoder() {
    if (m_loaded && m_pFlac != nullptr) {
        drflac_close(m_pFlac);
        m_pFlac = nullptr;
    }
}

bool FlacDecoder::load(const std::string& filename) {
    m_filename = filename;
    m_pFlac = drflac_open_file(filename.c_str(), nullptr);
    if (m_pFlac == nullptr) {
        std::cerr << "Failed to open FLAC file: " << filename << std::endl;
        m_loaded = false;
        return false;
    }
    m_loaded = true;
    return true;
}

bool FlacDecoder::decode(std::vector<float>& outBuffer) {
    if (!m_loaded || m_pFlac == nullptr) {
        std::cerr << "FLAC file not loaded." << std::endl;
        return false;
    }
    
    drflac_uint64 totalFrames = m_pFlac->totalPCMFrameCount;
    unsigned int channels = m_pFlac->channels;
    outBuffer.resize(totalFrames * channels);
    
    // Чтение всех сэмплов в виде float.
    drflac_uint64 framesRead = drflac_read_pcm_frames_f32(m_pFlac, totalFrames, outBuffer.data());
    return (framesRead == totalFrames);
}

std::string FlacDecoder::getTrackTitle() const {
    return fs::path(m_filename).filename().string();
}

int FlacDecoder::getSampleRate() const {
    // Если файл загружен, берем значение из m_pFlac
    if (m_pFlac) {
        return static_cast<int>(m_pFlac->sampleRate);
    }
    return 0;
}

int FlacDecoder::getChannels() const {
    if (m_pFlac) {
        return static_cast<int>(m_pFlac->channels);
    }
    return 0;
}

int FlacDecoder::getTotalFrames() const {
    if (m_pFlac) {
        return static_cast<int>(m_pFlac->totalPCMFrameCount);
    }
    return 0;
}
