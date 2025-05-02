cmake_minimum_required(VERSION 3.15)
project(MyIMGUIProject)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

# --- Fetch Dear ImGui ---
FetchContent_Declare(
  imgui
  URL https://github.com/ocornut/imgui/archive/refs/tags/v1.90.8.tar.gz
)
FetchContent_MakeAvailable(imgui)

# --- Fetch GLFW (cross-platform windowing library) ---
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG latest
)
FetchContent_MakeAvailable(glfw)

# --- Source files for ImGui ---
set(IMGUI_SRC
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_demo.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

# --- Executable ---
# Добавлены все необходимые исходники: основной файл и реализации классов UI.
add_executable(MyIMGUIApp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/UI/ImGuiManager.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/UI/MainWindow.cpp
  ${IMGUI_SRC}
)

# --- Include directories ---
target_include_directories(MyIMGUIApp PRIVATE
  ${imgui_SOURCE_DIR}
  ${imgui_SOURCE_DIR}/backends
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/src/UI
)
message(STATUS "ImGui source directory: ${imgui_SOURCE_DIR}")

# --- Link libraries ---
target_link_libraries(MyIMGUIApp PRIVATE glfw)

if (WIN32)
  target_link_libraries(MyIMGUIApp PRIVATE opengl32)
elseif(UNIX)
  find_package(OpenGL REQUIRED)
  target_link_libraries(MyIMGUIApp PRIVATE OpenGL::GL dl pthread)
endif()
