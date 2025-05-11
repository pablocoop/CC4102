#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <random>
#include <cassert>
#include <cstdint>

#include "mergesort_ext_aridad.h"
#include "quicksort_externo.h"
#include "contadores.h"


using namespace std;
using namespace chrono;


const size_t M = 50 * 1024 * 1024; // 50 MB
const size_t INT64_SIZE = sizeof(int64_t);

// Genera archivo binario con números aleatorios
void generate_random_file(const string &filename, size_t num_elements) {
    ofstream out(filename, ios::binary);
    random_device rd;
    mt19937_64 gen(rd());
    uniform_int_distribution<int64_t> dist(1, 1000000);

    for (size_t i = 0; i < num_elements; ++i) {
        int64_t val = dist(gen);
        out.write(reinterpret_cast<const char *>(&val), INT64_SIZE);
    }
    out.close();
}

// Ejecuta experimento para un tamaño dado N y aridad a
void run_experiment(size_t N, size_t a, ofstream &out) {
    const string input_prefix = "input_" + to_string(N);
    const string output_prefix = "output_" + to_string(N);

    vector<size_t> merge_reads, merge_writes;
    vector<size_t> quick_reads, quick_writes;

    for (int i = 0; i < 5; ++i) {
        string input_file = input_prefix + "_run" + to_string(i) + ".bin";
        string output_file_merge = output_prefix + "_merge_run" + to_string(i) + ".bin";
        string output_file_quick = output_prefix + "_quick_run" + to_string(i) + ".bin";

        generate_random_file(input_file, N);

        // MergeSort externo
        read_count = write_count = 0;
        ext_aridad_mergesort(input_file, output_file_merge, M, a);
        merge_reads.push_back(read_count);
        merge_writes.push_back(write_count);

        // QuickSort externo
        read_count = write_count = 0;
        fstream f(input_file, ios::in | ios::out | ios::binary);
        external_quicksort(f, 0, N, M, a);
        quick_reads.push_back(read_count);
        quick_writes.push_back(write_count);
        f.close();

        remove(input_file.c_str());
        remove(output_file_merge.c_str());
        remove(output_file_quick.c_str());
    }

    auto avg_size = [](const vector<size_t>& v) {
        size_t sum = 0;
        for (auto x : v) sum += x;
        return sum / v.size();
    };

    size_t N_bytes = N * INT64_SIZE;
    out << N_bytes << " " 
        << avg_size(merge_reads) << " " << avg_size(merge_writes) << " "
        << avg_size(quick_reads) << " " << avg_size(quick_writes) << "\n";
}

int main() {
    size_t values[] = {4, 8, 16, 32, 60}; 
    size_t a = 56;

    ofstream out("resultados.txt");
    if (!out.is_open()) {
        cerr << "No se pudo abrir el archivo de salida.\n";
        return 1;
    }

    // Cabecera
    out << "# N_bytes Merge_lecturas Merge_escrituras Quick_lecturas Quick_escrituras\n";

    for (size_t factor : values) {
        size_t N = (M / INT64_SIZE) * factor;
        run_experiment(N, a, out);
    }

    out.close();
    return 0;
}

