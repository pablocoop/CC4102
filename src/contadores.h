#include <cstdint>
#include <fstream>

#include <cstddef>

/**
 * @brief  Contador global de bloques leídos de disco (tamaño fijo, p.ej. 4096 B).
 */
extern size_t read_count;

/**
 * @brief  Contador global de bloques escritos en disco (tamaño fijo, p.ej. 4096 B).
 */
extern size_t write_count;

