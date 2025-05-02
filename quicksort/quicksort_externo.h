#ifndef QUICKSORT_EXTERNO_H
#define QUICKSORT_EXTERNO_H

#include <cstdint>
#include <fstream>

extern size_t read_count;
extern size_t write_count;

void external_quicksort(std::fstream &file, size_t start, size_t end, size_t M, size_t a);

#endif
