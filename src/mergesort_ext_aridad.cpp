#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <string>
#include <cstring>
#include <cassert>
#include <cstdlib>
#include <algorithm>
#include <cstdint>

using namespace std;

// Constantes globales
const size_t INT64_SIZE = sizeof(int64_t);
const size_t BLOCK_SIZE = 4096;              // B: Tamaño de bloque (ej. 4KB)
const size_t MAX_IN_MEMORY = 50 * 1024 * 1024; // M: Memoria disponible (ej. 50MB)

#include "contadores.h"

// Estructura para el heap durante el merge
struct HeapNode {
    int64_t value;
    size_t file_index;

    bool operator>(const HeapNode &other) const {
        return value > other.value;
    }
};

// Leer un bloque desde archivo
void read_block(ifstream &file, vector<int64_t> &buffer, size_t count) {
    file.read(reinterpret_cast<char*>(buffer.data()), count * INT64_SIZE);
    ++read_count;
}

// Escribir un bloque a archivo
void write_block(ofstream &file, const vector<int64_t> &buffer, size_t count) {
    file.write(reinterpret_cast<const char*>(buffer.data()), count * INT64_SIZE);
    ++write_count;
}

// Ordenar en memoria y guardar en archivo temporal
string sort_and_save_run(ifstream &input, size_t count) {
    vector<int64_t> data(count);
    input.read(reinterpret_cast<char*>(data.data()), count * INT64_SIZE);
    ++read_count;

    sort(data.begin(), data.end());

    string temp_name = "run_" + to_string(rand()) + ".bin";
    ofstream out(temp_name, ios::binary);
    write_block(out, data, count);
    out.close();
    return temp_name;
}

// Dividir el archivo en runs ordenadas
vector<string> create_initial_runs(const string &filename, size_t M) {
    ifstream input(filename, ios::binary);
    input.seekg(0, ios::end);
    size_t file_size = input.tellg();
    input.seekg(0, ios::beg);

    size_t total_elements = file_size / INT64_SIZE;
    size_t chunk_elements = M / INT64_SIZE;

    vector<string> runs;
    while (total_elements > 0) {
        size_t count = min(chunk_elements, total_elements);
        runs.push_back(sort_and_save_run(input, count));
        total_elements -= count;
    }
    input.close();
    return runs;
}

// Mezcla k-way de archivos temporales con aridad d
vector<string> k_way_merge_d(const vector<string> &runs, size_t d) {
    vector<string> current_runs = runs;

    while (current_runs.size() > 1) {
        vector<string> next_runs;

        for (size_t i = 0; i < current_runs.size(); i += d) {
            vector<string> group;
            for (size_t j = i; j < i + d && j < current_runs.size(); ++j) {
                group.push_back(current_runs[j]);
            }

            // Nombre para archivo temporal del merge
            string merged_run = "merged_" + to_string(rand()) + ".bin";

            // Realizar el merge k-way del grupo
            vector<ifstream> inputs(group.size());
            vector<int64_t> buffers(group.size());
            vector<bool> has_value(group.size(), false);

            for (size_t k = 0; k < group.size(); ++k) {
                inputs[k].open(group[k], ios::binary);
                if (inputs[k].read(reinterpret_cast<char*>(&buffers[k]), INT64_SIZE)) {
                    ++read_count;
                    has_value[k] = true;
                }
            }

            ofstream output(merged_run, ios::binary);
            priority_queue<HeapNode, vector<HeapNode>, greater<HeapNode>> min_heap;

            for (size_t k = 0; k < group.size(); ++k) {
                if (has_value[k]) {
                    min_heap.push({buffers[k], k});
                }
            }

            while (!min_heap.empty()) {
                HeapNode node = min_heap.top(); min_heap.pop();
                output.write(reinterpret_cast<const char*>(&node.value), INT64_SIZE);
                ++write_count;

                if (inputs[node.file_index].read(reinterpret_cast<char*>(&buffers[node.file_index]), INT64_SIZE)) {
                    ++read_count;
                    min_heap.push({buffers[node.file_index], node.file_index});
                }
            }

            // Cerrar y eliminar archivos de entrada
            for (size_t k = 0; k < group.size(); ++k) {
                inputs[k].close();
                remove(group[k].c_str());
            }

            output.close();
            next_runs.push_back(merged_run);
        }

        current_runs = next_runs;
    }

    return current_runs;
}

// MergeSort externo principal con aridad d
void ext_aridad_mergesort(const string &input_filename, const string &output_filename, size_t M, size_t d) {
    read_count = 0;
    write_count = 0;

    vector<string> runs = create_initial_runs(input_filename, M);
    vector<string> final_run = k_way_merge_d(runs, d);

    // Renombrar el archivo final como output
    if (!final_run.empty()) {
        rename(final_run[0].c_str(), output_filename.c_str());
    }

    cout << "Lecturas de bloques: " << read_count << endl;
    cout << "Escrituras de bloques: " << write_count << endl;
}

