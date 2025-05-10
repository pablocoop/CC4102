# Compilador
CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall

# Archivos fuente
SRC = main.cpp mergesort_ext_aridad.cpp quicksort_externo.cpp contadores.cpp

# Nombre del ejecutable
TARGET = main

# Regla por defecto
all: $(TARGET)

# Compilación del ejecutable
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# Limpiar archivos generados
clean:
	rm -f $(TARGET) *.o *.bin run_*.bin merged_*.bin input_*.bin output_*.bin