# Compilador y flags
CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Iinclude

# Directorios
SRC_DIR = src
BUILD_DIR = build
BIN = main

# Archivos fuente # 	$(SRC_DIR)/mergesort_externo.cpp 
SRC_FILES = \
	$(SRC_DIR)/main.cpp \
	$(SRC_DIR)/contadores.cpp \
	$(SRC_DIR)/mergesort_ext_aridad.cpp \
	$(SRC_DIR)/quicksort_externo.cpp

# Archivos objeto
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC_FILES))

# Regla por defecto
all: $(BIN)

# Crear ejecutable
$(BIN): $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compilar cada .cpp a .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Crear directorio build si no existe
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Limpiar archivos generados
clean:
	rm -rf $(BUILD_DIR) $(BIN) *.bin run_*.bin merged_*.bin input_*.bin output_*.bin

.PHONY: all clean
