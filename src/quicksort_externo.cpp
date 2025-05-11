#include "quicksort_externo.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <algorithm>
#include <cstring>
#include <cassert>
#include <cstdint>

using namespace std;

// Parámetros globales
const size_t INT64_SIZE = sizeof(int64_t);
const size_t BLOCK_SIZE = 4096; // 4KB
const size_t MAX_IN_MEMORY = 50 * 1024 * 1024; // 50MB

#include "contadores.h"

// Ordenar en memoria principal si el tamaño es suficientemente pequeño
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

// Seleccionar (a-1) pivotes aleatorios
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

vector<pair<string, size_t>> partition(fstream &file, size_t start, size_t end, const vector<int64_t>& pivots) {
    size_t num_partitions = pivots.size() + 1;
    vector<ofstream> out_files(num_partitions);
    vector<string> filenames(num_partitions);
    vector<size_t> counts(num_partitions, 0);

    // Generar nombres únicos para los archivos temporales
    for (size_t i = 0; i < num_partitions; ++i) {
        filenames[i] = "temp_part_" + to_string(i) + "_" + to_string(start) + ".bin";
        out_files[i].open(filenames[i], ios::binary);
    }

    // Búferes por partición
    const size_t buffer_capacity = BLOCK_SIZE / INT64_SIZE;
    vector<vector<int64_t>> buffers(num_partitions);

    size_t total_elements = end - start;
    size_t buffer_size = BLOCK_SIZE / INT64_SIZE;
    vector<int64_t> read_buffer(buffer_size);

    file.seekg(start * INT64_SIZE, ios::beg);

    for (size_t i = 0; i < total_elements; i += buffer_size) {
        size_t count = min(buffer_size, total_elements - i);
        file.read(reinterpret_cast<char*>(read_buffer.data()), count * INT64_SIZE);
        ++read_count;

        for (size_t j = 0; j < count; ++j) {
            int64_t val = read_buffer[j];
            size_t k = 0;
            while (k < pivots.size() && val >= pivots[k]) ++k;
            buffers[k].push_back(val);
            ++counts[k];

            // Si el búfer alcanza su capacidad, escribir en el archivo correspondiente
            if (buffers[k].size() >= buffer_capacity) {
                out_files[k].write(reinterpret_cast<const char*>(buffers[k].data()), buffers[k].size() * INT64_SIZE);
                ++write_count;
                buffers[k].clear();
            }
        }
    }

    // Escribir cualquier dato restante en los búferes
    for (size_t k = 0; k < num_partitions; ++k) {
        if (!buffers[k].empty()) {
            out_files[k].write(reinterpret_cast<const char*>(buffers[k].data()), buffers[k].size() * INT64_SIZE);
            ++write_count;
            buffers[k].clear();
        }
        out_files[k].close();
    }

    vector<pair<string, size_t>> result;
    for (size_t i = 0; i < num_partitions; ++i) {
        result.emplace_back(filenames[i], counts[i]);
    }
    return result;
}


// Quicksort externo
void external_quicksort(fstream &file, size_t start, size_t end, size_t M, size_t a) {
    if ((end - start) * INT64_SIZE <= M) {
        // Ordenar directamente en el archivo original si cabe en memoria
        sort_in_memory(file, start, end);
        return;
    }

    vector<int64_t> pivots = select_pivots(file, start, end, a);
    vector<pair<string, size_t>> partitions = partition(file, start, end, pivots);

    size_t current = start;
    for (const auto& [fname, elems] : partitions) {
        if (elems == 0) continue;

        // Ordenar el subarchivo recursivamente
        fstream part_file(fname, ios::in | ios::out | ios::binary);
        external_quicksort(part_file, 0, elems, M, a);
        part_file.close();

        // Solo si fue ordenado en el archivo temporal, copiar de nuevo al archivo principal
        if (elems * INT64_SIZE > M) {
            ifstream in(fname, ios::binary);
            file.seekp(current * INT64_SIZE, ios::beg);

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
        }

        remove(fname.c_str());
        current += elems;
    }

    cout << "QS: Lecturas de bloques: " << read_count << endl;
    cout << "QS: Escrituras de bloques: " << write_count << endl;
}
