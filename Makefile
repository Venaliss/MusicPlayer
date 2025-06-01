# Makefile для проекта MusicPlayer

# Папка сборки (вы можете изменить её имя по необходимости)
BUILD_DIR := build

# Определяем имя конечного исполняемого файла
ifeq ($(OS),Windows_NT)
	TARGET := MyIMGUIApp.exe
	# Используем rm -rf для удаления каталога в Unix-совместимой оболочке (например, Git Bash)
	RM_CMD := rm -rf
	RUN_CMD := $(BUILD_DIR)/$(TARGET)
else
	TARGET := MyIMGUIApp
	RM_CMD := rm -rf
	RUN_CMD := ./$(BUILD_DIR)/$(TARGET)
endif

.PHONY: all clean rebuild run

# Цель "all" (по умолчанию) генерирует файлы сборки и собирает проект.
all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET):
	@echo "Generating build files with CMake..."
	cmake -S . -B $(BUILD_DIR)
	@echo "Building project..."
	cmake --build $(BUILD_DIR)

# Цель "clean" удаляет папку сборки.
clean:
	@echo "Cleaning build directory..."
	$(RM_CMD) $(BUILD_DIR)

# Цель "rebuild" выполняет очистку и последующую полную сборку.
rebuild: clean all

# Цель "run" сначала собирает проект, а затем запускает конечное приложение.
run: all
	@echo "Running $(TARGET)..."
	$(RUN_CMD)
