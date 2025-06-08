#ifndef FLAC_DECODER_H
#define FLAC_DECODER_H

#include "IAudioDecoder.h"
#include <string>
#include <vector>
#include "dr_flac.h"

class FlacDecoder : public IAudioDecoder {
public:
    FlacDecoder();
    virtual ~FlacDecoder();
    
    virtual bool load(const std::string& filename) override;
    virtual bool decode(std::vector<float>& outBuffer) override;
    virtual std::string getTrackTitle() const override;
    
    // Реализация методов для метаданных
    virtual int getSampleRate() const override;
    virtual int getChannels() const override;
    virtual int getTotalFrames() const override;

private:
    std::string m_filename;
    drflac* m_pFlac;   // Указатель на открытый FLAC файл.
    bool m_loaded;
};

#endif // FLAC_DECODER_H
