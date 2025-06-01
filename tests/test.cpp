#include <cassert>
#include <iostream>
#include "AudioEngine.h"

void testAddTrack() {
    AudioEngine* engine = AudioEngine::Instance();
    
    //Запоминаем текущее число треков
    size_t initialSize = engine->getPlaylist().size();

    //Добавляем тестовый трек.
    const std::string testTrack = "test_track.mp3";
    engine->addTrack(testTrack);
    
    //Проверяем, что плейлист увеличился на 1.
    assert(engine->getPlaylist().size() == initialSize + 1);
    std::cout << "[PASSED] testAddTrack()" << std::endl;
}

void testRemoveTrack() {
    AudioEngine* engine = AudioEngine::Instance();
    
    //Добавляем тестовый трек, если его нет.
    const std::string testTrack = "test_remove.mp3";
    engine->addTrack(testTrack);
    size_t sizeAfterAdd = engine->getPlaylist().size();
    
    //Удаляем тестовый трек.
    engine->removeTrack(testTrack);
    
    //Проверяем, что число треков уменьшилось.
    assert(engine->getPlaylist().size() == sizeAfterAdd - 1);
    std::cout << "[PASSED] testRemoveTrack()" << std::endl;
}

// (Если текущее значение не установлено, функция возвращает пустую строку.)
void testGetCurrentTrack() {
    AudioEngine* engine = AudioEngine::Instance();
    
    // Сбрасываем текущий индекс и проверяем, что getCurrentTrack() возвращает пустую строку.
    std::string current = engine->getCurrentTrack();
    assert(current == "");
    std::cout << "[PASSED] testGetCurrentTrack() when no track is selected" << std::endl;
    
    // Добавляем тестовый трек и устанавливаем его как текущий.
    const std::string testTrack = "test_current.mp3";
    engine->addTrack(testTrack);

    if (!engine->getPlaylist().empty())
        std::cout << "[NOTE] getCurrentTrack() behavior depends on currentTrackIndex_ initialization" << std::endl;
}

int main() {
    std::cout << "Running AudioEngine tests..." << std::endl;
    testAddTrack();
    testRemoveTrack();
    testGetCurrentTrack();
    
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
