#ifndef QUICKSORT_EXTERNO_H
#define QUICKSORT_EXTERNO_H

#include <fstream>
#include <cstddef>

// Función principal para realizar el Quicksort Externo
// Ordena el archivo utilizando el algoritmo Quicksort, con particiones basadas en pivotes.
// Si la sección del archivo cabe en memoria, se ordena directamente en memoria.
// Si es demasiado grande, se divide y ordena recursivamente.
void external_quicksort(std::fstream &file, size_t start, size_t end, size_t M, size_t a);

#endif