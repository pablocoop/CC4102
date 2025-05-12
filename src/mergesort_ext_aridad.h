#ifndef MERGESORT_EXT_ARIDAD_H
#define MERGESORT_EXT_ARIDAD_H

#include <string>
#include <vector>
#include <cstdint>

/**
 * @brief  Nodo para el heap en el k-way merge.
 */
struct HeapNode {
    int64_t value;      /**< Valor actual del run. */
    size_t file_index;  /**< Índice del archivo de origen. */
    bool operator>(const HeapNode &other) const;
};

/**
 * @brief  Lee `count` elementos de 64 bits desde `file`, incrementando read_count.
 * @param  file    Stream binario de entrada.
 * @param  buffer  Vector donde depositar los valores leídos.
 * @param  count   Número de elementos a leer.
 */
void read_block(std::ifstream &file, std::vector<int64_t> &buffer, size_t count);

/**
 * @brief  Escribe `count` elementos de `buffer` en `file`, incrementando write_count.
 * @param  file    Stream binario de salida.
 * @param  buffer  Vector con valores a escribir.
 * @param  count   Número de elementos a escribir.
 */
void write_block(std::ofstream &file, const std::vector<int64_t> &buffer, size_t count);

/**
 * @brief  Ordena iterativamente en memoria principal el vector A.
 * @param  A     Vector de enteros a ordenar.
 * @param  left  Ignorado (para compatibilidad recursiva).
 * @param  right Ignorado (para compatibilidad recursiva).
 */
void merge_sort_internal(std::vector<int64_t> &A, int left, int right);

/**
 * @brief  Crea runs ordenados de hasta M bytes desde el archivo.
 * @param  filename  Ruta del archivo de entrada.
 * @param  M         Tamaño máximo en bytes de cada run.
 * @return Vector con nombres de archivos temporales generados.
 */
std::vector<std::string> create_initial_runs(const std::string &filename, size_t M);

/**
 * @brief  Fusiona múltiple archivos en aridad `d` usando un heap.
 * @param  runs  Nombres de archivos intermedios (ordenados).
 * @param  d     Número de archivos a fusionar simultáneamente.
 * @return Vector con un único elemento: el archivo final.
 */
std::vector<std::string> k_way_merge_d(const std::vector<std::string> &runs, size_t d);

/**
 * @brief  Ejecuta MergeSort externo con aridad `d`.
 * @param  input_filename   Nombre del archivo de entrada.
 * @param  output_filename  Nombre donde escribir el resultado.
 * @param  M                Tamaño del buffer interno (bytes).
 * @param  d                Aridad del merge.
 */
void ext_aridad_mergesort(
    const std::string &input_filename,
    const std::string &output_filename,
    size_t M,
    size_t d
);

#endif // MERGESORT_EXT_ARIDAD_H
