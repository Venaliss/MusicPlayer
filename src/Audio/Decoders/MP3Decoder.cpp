#include "MP3Decoder.h"
#include "minimp3_ex.h"  
#include <cstring>      // для std::memset
#include <cstdlib>      // для free

MP3Decoder::MP3Decoder() {
}

MP3Decoder::~MP3Decoder() {
}

bool MP3Decoder::load(const std::string& filename) {
    // Сохраняем имя файла (или путь) в переменной-члене.
    trackTitle = filename;
    return true;
}

bool MP3Decoder::decode(std::vector<float>& outBuffer) {
    // Проверяем, что имя файла не пустое.
    if (trackTitle.empty()) {
        return false;
    }
    
    // Инициализируем декодер.
    mp3dec_t dec;
    mp3dec_init(&dec);
    
    mp3dec_file_info_t info;
    std::memset(&info, 0, sizeof(info));
    
    // Загружаем MP3-файл.
    // Функция mp3dec_load возвращает 0 при успехе.
    int err = mp3dec_load(&dec, trackTitle.c_str(), &info, nullptr, 0);
    if (err != 0) {
        return false;
    }
    
    // Предполагаем, что info.buffer содержит int16_t сэмплы,
    // а info.samples – общее число сэмплов (для всех каналов).
    int16_t* samples = info.buffer;
    size_t nsamples = info.samples;
    
    // Переводим 16-битные сэмплы в float (масштабируем к диапазону [-1, 1]).
    outBuffer.resize(nsamples);
    for (size_t i = 0; i < nsamples; i++) {
        outBuffer[i] = samples[i] / 32768.0f;
    }
    
    // Освобождаем память, выделенную функцией mp3dec_load().
    free(info.buffer);
    
    return true;
}

std::string MP3Decoder::getTrackTitle() const {
    return trackTitle;
}
