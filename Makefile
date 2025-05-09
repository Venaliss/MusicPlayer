# Компиляторы
CXX       = g++
CC        = gcc

# Пути к исходникам и include-директориям
CXXFLAGS  = -std=c++17 -Wall -I./src -I./external/imgui -I./external/imgui/backends -I./external/minimp3 $(shell pkg-config --cflags glfw3)
CFLAGS    = -Wall -I./src -I./external/imgui -I./external/imgui/backends -I./external/minimp3 $(shell pkg-config --cflags glfw3)

# Флаги линковщика - используем pkg-config для GLFW, добавляем системные библиотеки
LDFLAGS   = $(shell pkg-config --libs glfw3) -lGL -ldl -lpthread

# Список исходных файлов C++.
CPP_SRCS  = \
    src/main.cpp \
    src/Audio/AudioEngine.cpp \
    src/Audio/Decoders/MP3Decoder.cpp \
    src/UI/ImGuiManager.cpp \
    src/UI/MainWindow.cpp \
    external/imgui/imgui.cpp \
    external/imgui/imgui_demo.cpp \
    external/imgui/imgui_draw.cpp \
    external/imgui/imgui_tables.cpp \
    external/imgui/imgui_widgets.cpp \
    external/imgui/backends/imgui_impl_glfw.cpp \
    external/imgui/backends/imgui_impl_opengl3.cpp

# Исходный файл на C для minimp3.
C_SRCS    = external/minimp3/minimp3_ex.c

# Правило для получения объектных файлов из файлов C++ и C.
OBJ_CPP   = $(CPP_SRCS:.cpp=.o)
OBJ_C     = $(C_SRCS:.c=.o)
OBJS      = $(OBJ_CPP) $(OBJ_C)

# Имя конечного исполняемого файла
TARGET    = MyIMGUIApp

# Правило сборки
all: $(TARGET)

$(TARGET): $(OBJS)
    $(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Команда для компиляции файлов C++
%.o: %.cpp
    $(CXX) $(CXXFLAGS) -c $< -o $@

# Команда для компиляции файлов C
%.o: %.c
    $(CC) $(CFLAGS) -c $< -o $@

# Очистка сгенерированных файлов
clean:
    rm -f $(OBJ_CPP) $(OBJ_C) $(TARGET)

.PHONY: all clean
