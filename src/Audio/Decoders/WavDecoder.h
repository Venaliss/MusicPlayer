#ifndef WAV_DECODER_H
#define WAV_DECODER_H

#include "IAudioDecoder.h"
#include <string>
#include <vector>
#include "dr_wav.h"

class WavDecoder : public IAudioDecoder {
public:
    WavDecoder();
    virtual ~WavDecoder();
    
    virtual bool load(const std::string& filename) override;
    virtual bool decode(std::vector<float>& outBuffer) override;
    virtual std::string getTrackTitle() const override;
    
    // Методы метаданных
    virtual int getSampleRate() const override;
    virtual int getChannels() const override;
    virtual int getTotalFrames() const override;

private:
    std::string m_filename;
    drwav m_wav;       // Структура для хранения состояния WAV файла.
    bool m_loaded;
};

#endif // WAV_DECODER_H
