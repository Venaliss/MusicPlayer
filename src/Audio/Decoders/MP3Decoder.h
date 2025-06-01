#ifndef MP3_DECODER_H
#define MP3_DECODER_H

#include <string>
#include <vector>
// IAudioDecoder определён в "src/Audio/Interface/IAudioDecoder.h"
#include "../Interface/IAudioDecoder.h"

class MP3Decoder : public IAudioDecoder {
public:
    MP3Decoder();
    virtual ~MP3Decoder();

    // Загружает MP3-файл и сохраняет его путь (или название) для дальнейшего декодирования.
    bool load(const std::string& filename) override;
    
    // Декодирует аудио данные в PCM и помещает их в outBuffer (значения приводятся к float в диапазоне [-1, 1]).
    bool decode(std::vector<float>& outBuffer) override;
    
    // Возвращает название трека – в этой реализации совпадает с переданным путем.
    std::string getTrackTitle() const override;
    
private:
    std::string trackTitle; // Здесь хранится путь к файлу MP3
};

#endif // MP3_DECODER_H
