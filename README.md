# MusicPlayer

**Музыкальный плеер** – приложение, разработанное на C++ с использованием современных библиотек для работы с аудио и создания графического интерфейса. Проект демонстрирует подходы ООП, модульное разделение кода и интеграцию внешних зависимостей через CMake.

---

## О проекте

**MusicPlayer** позволяет:
- Воспроизводить аудиотреки (MP3, WAV, FLAC) с использованием специализированных декодеров.
- Управлять воспроизведением (play, stop, prev, next) и плейлистом.
- Использовать минималистичный и современный графический интерфейс, реализованный на базе Dear ImGui и ImGuiFileDialog.

---

## Используемые технологии и библиотеки

- **C++:**  
  Основной язык разработки, обеспечивающий высокую производительность и поддержку современного объектно-ориентированного программирования.

- **CMake:**  
  Кроссплатформенная система сборки, используемая для организации проекта и управления внешними зависимостями через FetchContent.

- **SDL2:**  
  Библиотека для работы с мультимедиа: открытие аудио устройств и управление аудиопотоками.

- **minimp3:**  
  Лёгкая библиотека для декодирования MP3-файлов, позволяющая извлекать PCM-данные из файлов.

- **Dear ImGui и ImGuiFileDialog:**  
  Инструменты для создания современного графического пользовательского интерфейса с акцентом на простоту и функциональность.

- **GLFW и OpenGL:**  
  Используются вместе с ImGui для создания и рендеринга окна приложения с кроссплатформенной поддержкой графики.

---

## Сборка и запуск

### Требования

- Компилятор, поддерживающий **C++17**
- **CMake** версии **3.15** или выше
- **Git**

### Инструкция по сборке

1. **Клонирование репозитория:**

   Откройте терминал и выполните:
   ```bash
   git clone https://github.com/yourusername/MusicPlayer.git
2. **Создание директории для сборки и переход в неё:**
   ```bash
    cd MusicPlayer
    mkdir build
    cd build
3. **Конфигурация проекта через CMake:**
   ```bash
    cmake ..
CMake автоматически загрузит внешние зависимости (ImGui, SDL2, minimp3, ImGuiFileDialog) и настроит проект.

4. **Сборка проекта:**
   ```bash
    cmake --build .
После успешной сборки в папке build появится исполняемый файл (например, MyIMGUIApp или MyIMGUIApp.exe для Windows).

## Запуск приложения из терминала

- **На Linux/macOS:**
    ```bash
    ./MyIMGUIApp

- **На Windows**
    ```bash
    ./MyIMGUIApp.exe
- **Важно для Windows:** Если приложение использует динамически загружаемую версию SDL2, убедитесь, что рядом с исполняемым файлом присутствует файл SDL2.dll (или SDL2d.dll для Debug-сборки). При необходимости скопируйте DLL-файл вручную или настройте команду post-build в CMake для автоматического копирования.


