cmake_minimum_required(VERSION 3.15)

# Устанавливаем политику CMP0079 и минимальную CMake версию для под-проектов
cmake_policy(SET CMP0079 NEW)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "Minimum CMake policy version for subprojects" FORCE)

project(MyIMGUIProject)

# Отключаем поддержку Wayland для GLFW и устанавливаем стандарт C++
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

# Fetch minimp3 (содержит minimp3_ex.h)
FetchContent_Declare(
  minimp3
  GIT_REPOSITORY https://github.com/lieff/minimp3.git
  GIT_TAG master
)
FetchContent_MakeAvailable(minimp3)

# Fetch ImGuiFileDialog (без использования его CMakeLists.txt)
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

# Fetch dr_libs (для dr_wav.h, dr_flac.h и т.д.)
FetchContent_Declare(
  dr_libs
  GIT_REPOSITORY https://github.com/mackron/dr_libs.git
  GIT_TAG master
)
FetchContent_MakeAvailable(dr_libs)

# ---------------- End Fetch Content ------------------

# --- Глобально задаем путь к ImGui (используется для ImGui, imgui_impl_*, ImGuiFileDialog и т.п.) ---
set(IMGUI_INCLUDE_DIR ${imgui_SOURCE_DIR})
include_directories(${IMGUI_INCLUDE_DIR})

# --- Создаем INTERFACE-цель для dr_libs ---
add_library(dr_libs INTERFACE)
target_include_directories(dr_libs INTERFACE ${dr_libs_SOURCE_DIR})

# --- Цель для minimp3_ex (если требуется отдельно) ---
add_library(minimp3_ex STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/minimp3_ex.cpp)
target_include_directories(minimp3_ex PUBLIC ${minimp3_SOURCE_DIR})

# --- Цель ImGuiFileDialog ---
add_library(imguifiledialog STATIC
    ${imguifiledialog_SOURCE_DIR}/ImGuiFileDialog.cpp
)
target_include_directories(imguifiledialog PUBLIC
    ${imguifiledialog_SOURCE_DIR}
    ${IMGUI_INCLUDE_DIR}
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

# --- Цель приложения MyIMGUIApp ---
add_executable(MyIMGUIApp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/UI/ImGuiManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/UI/MainWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Audio/AudioEngine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Audio/Decoders/MP3Decoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Audio/Decoders/WavDecoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Audio/Decoders/FlacDecoder.cpp
    ${IMGUI_SRC}
)

# Определяем SDL_MAIN_HANDLED для отключения вызова SDL_main
target_compile_definitions(MyIMGUIApp PRIVATE SDL_MAIN_HANDLED)

# --- Include-директории для MyIMGUIApp ---
target_include_directories(MyIMGUIApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Audio
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Audio/Decoders
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Audio/Interface
    ${CMAKE_CURRENT_SOURCE_DIR}/src/UI
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${imguifiledialog_SOURCE_DIR}
    ${sdl_SOURCE_DIR}/include
)

message(STATUS "ImGui source directory: ${imgui_SOURCE_DIR}")
message(STATUS "IMGUI_INCLUDE_DIR: ${IMGUI_INCLUDE_DIR}")
message(STATUS "minimp3 source directory: ${minimp3_SOURCE_DIR}")
message(STATUS "ImGuiFileDialog source directory: ${imguifiledialog_SOURCE_DIR}")
message(STATUS "SDL source directory: ${sdl_SOURCE_DIR}")
message(STATUS "dr_libs source directory: ${dr_libs_SOURCE_DIR}")

# --- Линковка приложения ---
target_link_libraries(MyIMGUIApp PRIVATE
    glfw
    minimp3_ex
    imguifiledialog
    dr_libs         # Обеспечивает доступ к заголовочным файлам dr_wav.h, dr_flac.h и т.д.
    SDL2
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
        "$<TARGET_FILE:SDL2>"
        $<TARGET_FILE_DIR:MyIMGUIApp>)
endif()

# ---------------- ТЕСТЫ ------------------
enable_testing()

add_executable(AudioEngineTests 
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Audio/AudioEngine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Audio/Decoders/MP3Decoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Audio/Decoders/WavDecoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Audio/Decoders/FlacDecoder.cpp
)

target_compile_definitions(AudioEngineTests PRIVATE SDL_MAIN_HANDLED)

target_include_directories(AudioEngineTests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Audio
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Audio/Decoders
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Audio/Interface
    ${CMAKE_CURRENT_SOURCE_DIR}/src/UI
    ${imgui_SOURCE_DIR}
    ${sdl_SOURCE_DIR}/include
)

target_link_libraries(AudioEngineTests PRIVATE
    minimp3_ex
    dr_libs
    SDL2
    SDL2main
)

add_test(NAME AudioEngineTests COMMAND AudioEngineTests)
