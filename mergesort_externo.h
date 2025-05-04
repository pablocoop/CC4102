#ifndef MERGESORT_EXTERNO_H
#define MERGESORT_EXTERNO_H

#include <string>

// Llama a la rutina principal de ordenamiento externo
void external_mergesort(const std::string &input_filename, const std::string &output_filename, size_t M);

// Contadores globales de lecturas y escrituras a disco
extern size_t read_count;
extern size_t write_count;

#endif // MERGESORT_EXTERNO_H
