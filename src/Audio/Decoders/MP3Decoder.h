#ifndef MP3_DECODER_H
#define MP3_DECODER_H

#include <string>
#include <vector>
// Предполагается, что IAudioDecoder определён в "src/Audio/Interface/IAudioDecoder.h"
#include "../Interface/IAudioDecoder.h"

class MP3Decoder : public IAudioDecoder {
public:
    MP3Decoder();
    virtual ~MP3Decoder();

    // Загружает MP3-файл и извлекает необходимую информацию.
    bool load(const std::string& filename) override;
    
    // Декодирует аудио данные в PCM (пример, реализация зависит от библиотеки)
    bool decode(std::vector<float>& outBuffer) override;
    
    // Возвращает название трека (в данном примере – имя файла)
    std::string getTrackTitle() const override;
    
private:
    std::string trackTitle;
};

#endif // MP3_DECODER_H
