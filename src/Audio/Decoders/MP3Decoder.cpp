#include "MP3Decoder.h"
// Подключаем minimp3 – легковесную библиотеку для декодирования MP3:
// (здесь предполагается, что minimp3-интегрирован в проект, например, путем включения "minimp3.h")
#include "minimp3_ex.h"  // путь может отличаться

MP3Decoder::MP3Decoder() {
}

MP3Decoder::~MP3Decoder() {
}

bool MP3Decoder::load(const std::string& filename) {
    // В реальной реализации здесь происходит открытие файла, чтение заголовка,
    // инициализация декодера minimp3 и извлечение метаданных.
    // Для простоты используем имя файла как название трека.
    trackTitle = filename;
    // Допустим, загрузка всегда успешна.
    return true;
}

bool MP3Decoder::decode(std::vector<float>& outBuffer) {
    // Здесь можно реализовать декодирование с использованием minimp3_ex API.
    // В этом примере функция возвращает false как признак, что декодирование пока не реализовано.
    return false;
}

std::string MP3Decoder::getTrackTitle() const {
    return trackTitle;
}
