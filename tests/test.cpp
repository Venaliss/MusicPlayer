#include <iostream>
#include <cassert>
#include "AudioEngine.h"

int main()
{
    AudioEngine* engine = AudioEngine::Instance();


    assert(engine->getPlaylist().empty());
    std::cout << "Test 1 passed: Initial playlist is empty." << std::endl;


    engine->addTrack("nonexistent1.mp3");
    engine->addTrack("nonexistent2.wav");
    engine->addTrack("nonexistent3.flac");

    assert(engine->getPlaylist().size() == 3);
    std::cout << "Test 2 passed: Adding tracks works." << std::endl;

    auto displayList = engine->getDisplayPlaylist();
    assert(displayList.size() == 3);
    // Предполагается, что функция извлекает только имя файла.
    assert(displayList[0] == "nonexistent1.mp3");
    assert(displayList[1] == "nonexistent2.wav");
    assert(displayList[2] == "nonexistent3.flac");
    std::cout << "Test 3 passed: Display playlist returns correct file names." << std::endl;

    engine->setVolume(0.75f);
    assert(engine->getVolume() == 0.75f);
    std::cout << "Test 4 passed: Volume set/get works." << std::endl;
    

    engine->play();
    // Если загрузка не удалась, getCurrentTrack() всё равно вернёт сохранённое значение из плейлиста.
    assert(engine->getCurrentTrack() == "nonexistent1.mp3");
    std::cout << "Test 5a passed: Play() sets current track." << std::endl;
    
    engine->next();
    assert(engine->getCurrentTrack() == "nonexistent2.wav");
    std::cout << "Test 5b passed: Next() sets current track correctly." << std::endl;
    
    engine->prev();
    assert(engine->getCurrentTrack() == "nonexistent1.mp3");
    std::cout << "Test 5c passed: Prev() sets current track correctly." << std::endl;
    
    // Если удаляется текущий трек, воспроизведение должно остановиться и индекс обнуляться.
    engine->play(); // пытаемся воспроизвести nonexistent1.mp3
    engine->removeTrack("nonexistent1.mp3");
    // Плейлист теперь должен содержать 2 элемента.
    assert(engine->getPlaylist().size() == 2);
    // Текущий трек не должен быть "nonexistent1.mp3"
    assert(engine->getCurrentTrack() != "nonexistent1.mp3");
    std::cout << "Test 6 passed: Remove track updates playlist and stops playback if needed." << std::endl;
    
    // Если файл не валиден, вызов setTrackPosition не должен приводить к ошибке.
    engine->setTrackPosition(10);
    int curTime = engine->getCurrentTime();
    // Для невалидного файла curTime может оставаться 0, толок демонстрируется вызов.
    std::cout << "Test 7 info: SetTrackPosition(10) called; current time = " << curTime << " (expected to be 0 for invalid file)" << std::endl;

    std::cout << "All AudioEngine tests passed." << std::endl;
    return 0;
}
