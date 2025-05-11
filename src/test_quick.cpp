#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <random>
#include <cassert>
#include <cstdint>

#include "quicksort_externo.h"
#include "contadores.h"

using namespace std;

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

// Verifica si un archivo binario está ordenado
bool is_sorted_file(const string &filename, size_t num_elements) {
    ifstream in(filename, ios::binary);
    int64_t prev, curr;

    if (!in.read(reinterpret_cast<char*>(&prev), INT64_SIZE)) return true;

    for (size_t i = 1; i < num_elements; ++i) {
        if (!in.read(reinterpret_cast<char*>(&curr), INT64_SIZE)) break;
        if (curr < prev) return false;
        prev = curr;
    }

    return true;
}

// Ejecuta pruebas solo para QuickSort Externo
void run_quick_experiment(size_t N, size_t a, ofstream &out) {
    const string input_prefix = "quick_input_" + to_string(N);
    vector<size_t> quick_reads, quick_writes;

    for (int i = 0; i < 5; ++i) {
        string input_file = input_prefix + "_run" + to_string(i) + ".bin";

        generate_random_file(input_file, N);

        read_count = write_count = 0;
        fstream f(input_file, ios::in | ios::out | ios::binary);
        external_quicksort(f, 0, N, M, a);
        f.close();

        //assert(is_sorted_file(input_file, N) && "Error: archivo no quedó ordenado.");

        quick_reads.push_back(read_count);
        quick_writes.push_back(write_count);

        remove(input_file.c_str());
    }

    auto avg_size = [](const vector<size_t>& v) {
        size_t sum = 0;
        for (auto x : v) sum += x;
        return sum / v.size();
    };

    size_t N_bytes = N * INT64_SIZE;
    out << N_bytes << " " 
        << avg_size(quick_reads) << " " << avg_size(quick_writes) << "\n";
}

int main() {
    size_t values[] = {4, 8, 16, 32, 60}; 
    size_t a = 56;

    ofstream out("resultados_quick.txt");
    if (!out.is_open()) {
        cerr << "No se pudo abrir el archivo de salida.\n";
        return 1;
    }

    // Cabecera
    out << "# N_bytes Quick_lecturas Quick_escrituras\n";

    for (size_t factor : values) {
        size_t N = (M / INT64_SIZE) * factor;
        cout << "Valor de N: " << N << endl;
        run_quick_experiment(N, a, out);
    }

    out.close();
    return 0;
}
