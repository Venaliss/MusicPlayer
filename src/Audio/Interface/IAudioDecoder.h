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

    // Дополнительные методы для метаданных:
    virtual int getSampleRate() const = 0;  // Частота дискретизации файла
    virtual int getChannels() const = 0;    // Число каналов
    virtual int getTotalFrames() const = 0; // Общее число аудиофреймов (каждый фрейм – набор сэмплов для всех каналов)
};

#endif
