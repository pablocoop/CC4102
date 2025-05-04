#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <string>
#include <cstring>
#include <cassert>

using namespace std;

// Constantes globales
const size_t INT64_SIZE = sizeof(int64_t);
const size_t BLOCK_SIZE = 4096;              // B: Tamaño de bloque (ej. 4KB)
const size_t MAX_IN_MEMORY = 50 * 1024 * 1024; // M: Memoria disponible (ej. 50MB)

size_t read_count = 0;
size_t write_count = 0;

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

// Mezcla k-way de los archivos temporales
void k_way_merge(const vector<string> &runs, const string &output_filename) {
    vector<ifstream> inputs(runs.size());
    vector<int64_t> buffers(runs.size());
    vector<bool> has_value(runs.size(), false);

    // Abrir todos los archivos
    for (size_t i = 0; i < runs.size(); ++i) {
        inputs[i].open(runs[i], ios::binary);
        if (inputs[i].read(reinterpret_cast<char*>(&buffers[i]), INT64_SIZE)) {
            ++read_count;
            has_value[i] = true;
        }
    }

    ofstream output(output_filename, ios::binary);
    priority_queue<HeapNode, vector<HeapNode>, greater<HeapNode>> min_heap;

    for (size_t i = 0; i < runs.size(); ++i) {
        if (has_value[i]) {
            min_heap.push({buffers[i], i});
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

    // Cerrar y eliminar archivos temporales
    for (size_t i = 0; i < runs.size(); ++i) {
        inputs[i].close();
        remove(runs[i].c_str());
    }

    output.close();
}

// MergeSort externo principal
void external_mergesort(const string &input_filename, const string &output_filename, size_t M) {
    vector<string> runs = create_initial_runs(input_filename, M);
    k_way_merge(runs, output_filename);
}
