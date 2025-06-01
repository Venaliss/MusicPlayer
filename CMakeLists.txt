cmake_minimum_required(VERSION 3.15)

# Устанавливаем политику CMP0079 NEW и минимальную политику для под-проектов
cmake_policy(SET CMP0079 NEW)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "Minimum CMake policy version for subprojects" FORCE)

project(MyIMGUIProject)

# Отключаем поддержку Wayland для GLFW
set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "Disable building with Wayland support" FORCE)
set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

# ---------------- Fetch Content ------------------

# Fetch Dear ImGui
FetchContent_Declare(
  imgui
  URL https://github.com/ocornut/imgui/archive/refs/tags/v1.90.8.tar.gz
)
FetchContent_MakeAvailable(imgui)

# Fetch GLFW
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG latest
)
FetchContent_MakeAvailable(glfw)

# Fetch minimp3 (в репозитории есть только minimp3_ex.h)
FetchContent_Declare(
  minimp3
  GIT_REPOSITORY https://github.com/lieff/minimp3.git
  GIT_TAG master
)
FetchContent_MakeAvailable(minimp3)

# Для ImGuiFileDialog не вызываем FetchContent_MakeAvailable, чтобы не использовать его CMakeLists.txt
FetchContent_Declare(
  imguifiledialog
  GIT_REPOSITORY https://github.com/aiekick/ImGuiFileDialog.git
  GIT_TAG master
)
FetchContent_GetProperties(imguifiledialog)
if (NOT imguifiledialog_POPULATED)
  FetchContent_Populate(imguifiledialog)
endif()

# Fetch SDL
set(SDL_SHARED ON CACHE BOOL "Build SDL as a shared library" FORCE)
set(SDL_STATIC OFF CACHE BOOL "Disable static SDL build" FORCE)
FetchContent_Declare(
  sdl
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG release-2.0.22
)
FetchContent_MakeAvailable(sdl)
# ---------------- End Fetch Content ------------------

# --- Глобально задаем путь к ImGui ---
set(IMGUI_INCLUDE_DIR ${imgui_SOURCE_DIR})
include_directories(${IMGUI_INCLUDE_DIR})

# --- Создаем цель для minimp3_ex (реализация MP3-декодера) ---
# Файл src/minimp3_ex.cpp должен содержать:
#   #define MINIMP3_IMPLEMENTATION
#   #include "minimp3_ex.h"
add_library(minimp3_ex STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/minimp3_ex.cpp)
target_include_directories(minimp3_ex PUBLIC ${minimp3_SOURCE_DIR})

# --- Создаем цель ImGuiFileDialog вручную ---
# Вместо использования собранной цели из imguifiledialog, добавляем исходный файл напрямую.
add_library(imguifiledialog STATIC
    ${imguifiledialog_SOURCE_DIR}/ImGuiFileDialog.cpp
)
target_include_directories(imguifiledialog 
    PUBLIC 
        ${imguifiledialog_SOURCE_DIR}     # Заголовочные файлы ImGuiFileDialog
        ${IMGUI_INCLUDE_DIR}               # Для поиска "imgui.h"
)

# --- Исходники для ImGui ---
set(IMGUI_SRC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

# --- Создаем исполняемый файл приложения ---
add_executable(MyIMGUIApp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/UI/ImGuiManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/UI/MainWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Audio/AudioEngine.cpp
    ${IMGUI_SRC}
)

# --- Include директории для MyIMGUIApp ---
target_include_directories(MyIMGUIApp PRIVATE
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/UI
    ${imguifiledialog_SOURCE_DIR}   # Для include ImGuiFileDialog.h, если используется в коде
    ${sdl_SOURCE_DIR}/include       # Чтобы компилятор нашел SDL.h
)

message(STATUS "ImGui source directory: ${imgui_SOURCE_DIR}")
message(STATUS "IMGUI_INCLUDE_DIR: ${IMGUI_INCLUDE_DIR}")
message(STATUS "minimp3 source directory: ${minimp3_SOURCE_DIR}")
message(STATUS "ImGuiFileDialog source directory: ${imguifiledialog_SOURCE_DIR}")
message(STATUS "SDL source directory: ${sdl_SOURCE_DIR}")

# --- Линковка ---
target_link_libraries(MyIMGUIApp PRIVATE
    glfw
    minimp3_ex
    imguifiledialog
    SDL2           # Цель, создаваемая SDL (в Debug – SDL2d.lib, в Release – SDL2.lib)
    SDL2main
)

if (WIN32)
    target_link_libraries(MyIMGUIApp PRIVATE opengl32)
elseif(UNIX)
    find_package(OpenGL REQUIRED)
    target_link_libraries(MyIMGUIApp PRIVATE OpenGL::GL dl pthread)
endif()

if(WIN32)
  add_custom_command(TARGET MyIMGUIApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "$<TARGET_FILE:SDL2>"   # Полный путь к DLL для SDL2
      $<TARGET_FILE_DIR:MyIMGUIApp>)
endif()

# ---------------- ТЕСТЫ ------------------
# В директории 'tests' должен быть файл test.cpp, содержащий тесты для AudioEngine и других функций.
enable_testing()

# Создаем тестовую цель, которая включает тестовый исходник и аудио движок.
add_executable(AudioEngineTests 
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Audio/AudioEngine.cpp  # Подключаем реализацию AudioEngine для тестирования
)

target_include_directories(AudioEngineTests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src      # Для поиска AudioEngine.h (если он находится здесь)
    ${CMAKE_CURRENT_SOURCE_DIR}/src/UI   # При необходимости, если используются UI-headers
    ${imgui_SOURCE_DIR}                   # Для ImGui (если требуется)
    ${sdl_SOURCE_DIR}/include             # Для SDL.h
)

# Линкуем минимальный набор зависимостей для тестов.
target_link_libraries(AudioEngineTests PRIVATE
    minimp3_ex
    SDL2
    SDL2main
)

# Регистрируем тестовую цель в CTest
add_test(NAME AudioEngineTests COMMAND AudioEngineTests)
