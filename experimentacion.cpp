#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <random>
#include <cassert>
#include "mergesort_externo.h"
#include "quicksort_externo.h"

using namespace std;
using namespace chrono;

extern size_t read_count;
extern size_t write_count;

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
void run_experiment(size_t N, size_t a) {
    const string input_prefix = "input_" + to_string(N);
    const string output_prefix = "output_" + to_string(N);

    vector<double> merge_times, quick_times;
    vector<size_t> merge_reads, merge_writes;
    vector<size_t> quick_reads, quick_writes;

    for (int i = 0; i < 5; ++i) {
        string input_file = input_prefix + "_run" + to_string(i) + ".bin";
        string output_file_merge = output_prefix + "_merge_run" + to_string(i) + ".bin";
        string output_file_quick = output_prefix + "_quick_run" + to_string(i) + ".bin";

        generate_random_file(input_file, N);

        // MergeSort externo
        read_count = write_count = 0;
        auto t1 = high_resolution_clock::now();
        external_mergesort(input_file, output_file_merge, M);
        auto t2 = high_resolution_clock::now();
        merge_times.push_back(duration<double>(t2 - t1).count());
        merge_reads.push_back(read_count);
        merge_writes.push_back(write_count);

        // QuickSort externo
        read_count = write_count = 0;
        fstream f(input_file, ios::in | ios::out | ios::binary);
        auto t3 = high_resolution_clock::now();
        external_quicksort(f, 0, N, M, a);
        auto t4 = high_resolution_clock::now();
        quick_times.push_back(duration<double>(t4 - t3).count());
        quick_reads.push_back(read_count);
        quick_writes.push_back(write_count);
        f.close();
    }

    // Promedios
    auto avg = [](const vector<double>& v) {
        double sum = 0;
        for (auto x : v) sum += x;
        return sum / v.size();
    };
    auto avg_size = [](const vector<size_t>& v) {
        size_t sum = 0;
        for (auto x : v) sum += x;
        return sum / v.size();
    };

    cout << "==== Resultados para N = " << N << " ====\n";
    cout << "MergeSort - Tiempo: " << avg(merge_times) << " s, Lecturas: "
         << avg_size(merge_reads) << ", Escrituras: " << avg_size(merge_writes) << "\n";
    cout << "QuickSort - Tiempo: " << avg(quick_times) << " s, Lecturas: "
         << avg_size(quick_reads) << ", Escrituras: " << avg_size(quick_writes) << "\n\n";
}

int main() {
    size_t values[] = {4, 8, 16, 32, 60}; // factores de M
    size_t a = 8; // Reemplazar con la aridad óptima encontrada en búsqueda binaria

    for (size_t factor : values) {
        size_t N = (M / INT64_SIZE) * factor;
        run_experiment(N, a);
    }

    return 0;
}
