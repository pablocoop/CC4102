#ifndef MERGESORT_EXT_ARIDAD_H
#define MERGESORT_EXT_ARIDAD_H

#include <string>
#include <vector>
#include <cstdint>
#include <fstream>

// Tamaño de entero usado y configuración de bloques
extern const size_t INT64_SIZE;
extern const size_t BLOCK_SIZE;
extern const size_t MAX_IN_MEMORY;

extern size_t read_count;
extern size_t write_count;

// Nodo para el heap durante el merge
struct HeapNode {
    int64_t value;
    size_t file_index;

    bool operator>(const HeapNode &other) const;
};

// Lee un bloque desde archivo
void read_block(std::ifstream &file, std::vector<int64_t> &buffer, size_t count);

// Escribe un bloque a archivo
void write_block(std::ofstream &file, const std::vector<int64_t> &buffer, size_t count);

// Ordena un bloque en memoria y lo guarda como archivo temporal
std::string sort_and_save_run(std::ifstream &input, size_t count);

// Divide el archivo original en "runs" ordenadas
std::vector<std::string> create_initial_runs(const std::string &filename, size_t M);

// Realiza la mezcla d-way de los runs intermedios
std::vector<std::string> k_way_merge_d(const std::vector<std::string> &runs, size_t d);

// Función principal de MergeSort externo con aridad d
void ext_aridad_mergesort(const std::string &input_filename,
                          const std::string &output_filename,
                          size_t M,
                          size_t d);

#endif // MERGESORT_EXT_ARIDAD_H
