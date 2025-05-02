#include "quicksort_externo.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <algorithm>
#include <cstring> // memcpy
#include <cassert>

using namespace std;

// Parámetros globales
const size_t INT64_SIZE = sizeof(int64_t);
const size_t BLOCK_SIZE = 4096; // B (por ejemplo: 4KB)
const size_t MAX_IN_MEMORY = 50 * 1024 * 1024; // M: 50MB

// Contadores de accesos a disco
size_t read_count = 0;
size_t write_count = 0;

// Leer un bloque de tamaño fijo desde el archivo
void read_block(ifstream &file, vector<int64_t> &buffer, size_t block_index, size_t block_elements) {
    file.seekg(block_index * BLOCK_SIZE, ios::beg);
    file.read(reinterpret_cast<char*>(buffer.data()), block_elements * INT64_SIZE);
    ++read_count;
}

// Escribir un bloque de tamaño fijo al archivo
void write_block(ofstream &file, const vector<int64_t> &buffer, size_t block_index, size_t block_elements) {
    file.seekp(block_index * BLOCK_SIZE, ios::beg);
    file.write(reinterpret_cast<const char*>(buffer.data()), block_elements * INT64_SIZE);
    ++write_count;
}

// Función para ordenar en memoria si N <= M
void sort_in_memory(fstream &file, size_t start, size_t end) {
    size_t num_elements = end - start;
    vector<int64_t> data(num_elements);

    file.seekg(start * INT64_SIZE, ios::beg);
    file.read(reinterpret_cast<char*>(data.data()), num_elements * INT64_SIZE);
    ++read_count;

    sort(data.begin(), data.end());

    file.seekp(start * INT64_SIZE, ios::beg);
    file.write(reinterpret_cast<const char*>(data.data()), num_elements * INT64_SIZE);
    ++write_count;
}

// Elegir (a - 1) pivotes aleatorios del archivo
vector<int64_t> select_pivots(fstream &file, size_t start, size_t end, size_t a) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<size_t> dist(start, end - 1);

    vector<int64_t> pivots;
    while (pivots.size() < a - 1) {
        size_t pos = dist(gen);
        int64_t val;
        file.seekg(pos * INT64_SIZE, ios::beg);
        file.read(reinterpret_cast<char*>(&val), INT64_SIZE);
        ++read_count;
        pivots.push_back(val);
    }
    sort(pivots.begin(), pivots.end());
    return pivots;
}

// Particionar el arreglo en 'a' subarreglos en archivos temporales
vector<string> partition(fstream &file, size_t start, size_t end, const vector<int64_t>& pivots) {
    size_t num_partitions = pivots.size() + 1;
    vector<ofstream> out_files(num_partitions);
    vector<string> filenames;

    // Crear archivos temporales para cada partición
    for (size_t i = 0; i < num_partitions; ++i) {
        string fname = "temp_part_" + to_string(i) + ".bin";
        filenames.push_back(fname);
        out_files[i].open(fname, ios::binary);
    }

    size_t total_elements = end - start;
    file.seekg(start * INT64_SIZE, ios::beg);

    // Procesar bloque por bloque
    size_t buffer_size = BLOCK_SIZE / INT64_SIZE;
    vector<int64_t> buffer(buffer_size);

    for (size_t i = 0; i < total_elements; i += buffer_size) {
        size_t count = min(buffer_size, total_elements - i);
        file.read(reinterpret_cast<char*>(buffer.data()), count * INT64_SIZE);
        ++read_count;

        for (size_t j = 0; j < count; ++j) {
            int64_t val = buffer[j];
            size_t k = 0;
            while (k < pivots.size() && val >= pivots[k]) ++k;
            out_files[k].write(reinterpret_cast<const char*>(&val), INT64_SIZE);
            ++write_count;
        }
    }

    for (auto& f : out_files) f.close();
    return filenames;
}

// Concatenar los archivos ordenados en el archivo original
void concatenate(fstream &file, size_t start, const vector<string>& partitions) {
    file.seekp(start * INT64_SIZE, ios::beg);
    for (const auto& fname : partitions) {
        ifstream in(fname, ios::binary);

        in.seekg(0, ios::end);
        size_t file_size = in.tellg();
        in.seekg(0, ios::beg);

        vector<char> buffer(BLOCK_SIZE);
        while (!in.eof()) {
            in.read(buffer.data(), BLOCK_SIZE);
            size_t read_bytes = in.gcount();
            if (read_bytes > 0) {
                file.write(buffer.data(), read_bytes);
                ++read_count;
                ++write_count;
            }
        }

        in.close();
        remove(fname.c_str());
    }
}

// Quicksort externo principal
void external_quicksort(fstream &file, size_t start, size_t end, size_t M, size_t a) {
    if ((end - start) * INT64_SIZE <= M) {
        sort_in_memory(file, start, end);
        return;
    }

    vector<int64_t> pivots = select_pivots(file, start, end, a);
    vector<string> partitions = partition(file, start, end, pivots);

    size_t current = start;
    for (const auto& fname : partitions) {
        fstream part_file(fname, ios::in | ios::out | ios::binary);
        part_file.seekg(0, ios::end);
        size_t bytes = part_file.tellg();
        size_t elems = bytes / INT64_SIZE;
        part_file.close();

        external_quicksort(file, current, current + elems, M, a);
        current += elems;
    }

    concatenate(file, start, partitions);
}
