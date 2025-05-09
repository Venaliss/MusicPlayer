#ifndef IAUDIO_DECODER_H
#define IAUDIO_DECODER_H

#include <string>
#include <vector>

class IAudioDecoder {
public:
    virtual ~IAudioDecoder() {}
    
    // Загружает файл с аудио данными
    virtual bool load(const std::string& filename) = 0;
    
    // Декодирует аудио данные в буфер с плавающей точкой
    virtual bool decode(std::vector<float>& outBuffer) = 0;
    
    // Возвращает название трека
    virtual std::string getTrackTitle() const = 0;
};

#endif // IAUDIO_DECODER_H
