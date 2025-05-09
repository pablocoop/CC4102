#include "mergesort_externo.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <cassert>
#include "contadores.h"

using namespace std;

void generate_input_file(const string &filename, size_t num_elements) {
    ofstream out(filename, ios::binary);
    random_device rd;
    mt19937_64 gen(rd());
    uniform_int_distribution<int64_t> dist(0, 1000000000);

    for (size_t i = 0; i < num_elements; ++i) {
        int64_t val = dist(gen);
        out.write(reinterpret_cast<const char*>(&val), sizeof(int64_t));
    }

    out.close();
}

bool is_file_sorted(const string &filename) {
    ifstream in(filename, ios::binary);
    int64_t prev, curr;

    if (!in.read(reinterpret_cast<char*>(&prev), sizeof(int64_t)))
        return true; // vacío => ordenado

    while (in.read(reinterpret_cast<char*>(&curr), sizeof(int64_t))) {
        if (curr < prev)
            return false;
        prev = curr;
    }

    return true;
}

int main() {
    string input_file = "input.bin";
    string output_file = "sorted_output.bin";

    size_t num_elements = 500000; // 500 mil enteros (~4MB)
    size_t M = 10 * 1024 * 1024;  // 10 MB de memoria

    cout << "Generando archivo de entrada...\n";
    generate_input_file(input_file, num_elements);

    cout << "Ejecutando MergeSort externo...\n";
    external_mergesort(input_file, output_file, M);

    cout << "Verificando orden...\n";
    assert(is_file_sorted(output_file));

    cout << "Archivo ordenado correctamente.\n";
    cout << "Lecturas: " << read_count << ", Escrituras: " << write_count << endl;

    return 0;
}
