#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <string>
#include <cstring>
#include <cassert>
#include <algorithm>
#include <cstdlib>
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

void merge(const vector<string> &group, const string &merged_run) {
    size_t group_size = group.size();
    size_t buffer_capacity = BLOCK_SIZE / INT64_SIZE;

    vector<ifstream> inputs(group_size);
    vector<vector<int64_t>> input_buffers(group_size, vector<int64_t>(buffer_capacity));
    vector<size_t> buffer_indices(group_size, 0);
    vector<size_t> buffer_sizes(group_size, 0);
    vector<bool> finished(group_size, false);

    ofstream output(merged_run, ios::binary);
    vector<int64_t> output_buffer;
    output_buffer.reserve(buffer_capacity);

    auto load_buffer = [&](size_t i) {
    if (!inputs[i].eof()) {
            inputs[i].read(reinterpret_cast<char*>(input_buffers[i].data()), buffer_capacity * INT64_SIZE);
            size_t read_bytes = inputs[i].gcount();
            buffer_sizes[i] = read_bytes / INT64_SIZE;
            buffer_indices[i] = 0;
            if (buffer_sizes[i] > 0) ++read_count;
            else finished[i] = true;
        } else {
            finished[i] = true;
        }
    };

    // Abrir archivos y cargar primer bloque
    for (size_t i = 0; i < group_size; ++i) {
        inputs[i].open(group[i], ios::binary);
        load_buffer(i);
    }

    // Heap mínimo
    priority_queue<HeapNode, vector<HeapNode>, greater<HeapNode>> min_heap;
    for (size_t i = 0; i < group_size; ++i) {
        if (!finished[i]) {
            min_heap.push({input_buffers[i][buffer_indices[i]++], i});
        }
    }

    while (!min_heap.empty()) {
        HeapNode node = min_heap.top(); min_heap.pop();
        output_buffer.push_back(node.value);

        if (output_buffer.size() == buffer_capacity) {
            write_block(output, output_buffer, buffer_capacity);
            output_buffer.clear();
        }

        size_t src = node.file_index;
        if (buffer_indices[src] < buffer_sizes[src]) {
            min_heap.push({input_buffers[src][buffer_indices[src]++], src});
        } else {
            load_buffer(src);
            if (!finished[src]) {
                min_heap.push({input_buffers[src][buffer_indices[src]++], src});
            }
        }
    }

    // Escribir lo que quede en el buffer de salida
    if (!output_buffer.empty()) {
        write_block(output, output_buffer, output_buffer.size());
    }

    // Cierre y limpieza
    for (size_t i = 0; i < group_size; ++i) {
        inputs[i].close();
        remove(group[i].c_str());
    }
    output.close();
}


// Mezcla k-way de los archivos temporales con aridad d
void k_way_merge_with_arity(const vector<string> &runs, const string &output_filename, size_t d, size_t M) {
    // Cálculo de la aridad máxima permitida
    size_t max_d = (M - BLOCK_SIZE) / (BLOCK_SIZE + sizeof(HeapNode));
    if (d > max_d) {
        cerr << "Advertencia: aridad d reducida de " << d << " a " << max_d << " para no exceder M.\n";
        d = max_d;
    }

    vector<string> current_runs = runs;

    while (current_runs.size() > 1) {
        vector<string> next_runs;

        for (size_t i = 0; i < current_runs.size(); i += d) {
            vector<string> group;
            for (size_t j = i; j < i + d && j < current_runs.size(); ++j) {
                group.push_back(current_runs[j]);
            }

            string merged_run = "merged_" + to_string(rand()) + ".bin";
            merge(group, merged_run);
            next_runs.push_back(merged_run);
        }

        current_runs = next_runs;
    }

    if (!current_runs.empty()) {
        rename(current_runs[0].c_str(), output_filename.c_str());
    }
}

// MergeSort externo principal con aridad
void external_mergesort(const string &input_filename, const string &output_filename, size_t M, size_t d) {
    vector<string> runs = create_initial_runs(input_filename, M);
    k_way_merge_with_arity(runs, output_filename, d, M);
}

