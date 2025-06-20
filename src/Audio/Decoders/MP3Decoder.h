#ifndef MP3_DECODER_H
#define MP3_DECODER_H

#include "IAudioDecoder.h"
#include <string>
#include <vector>

class MP3Decoder : public IAudioDecoder {
public:
    MP3Decoder();
    virtual ~MP3Decoder();
    
    virtual bool load(const std::string& filename) override;
    virtual bool decode(std::vector<float>& outBuffer) override;
    virtual std::string getTrackTitle() const override;
    
    // Методы метаданных
    virtual int getSampleRate() const override;
    virtual int getChannels() const override;
    virtual int getTotalFrames() const override;

private:
    std::string trackTitle;
    int sampleRate;    // Частота дискретизации (например, 44100)
    int channels;      // Количество каналов (например, 2)
    int totalSamples;  // Общее число сэмплов.
};

#endif // MP3_DECODER_H
