#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <random>
#include <climits>
#include "mergesort_ext_aridad.h"
#include "contadores.h" // si usas un archivo .h para declarar extern

using namespace std;
using namespace chrono;

extern size_t read_count;
extern size_t write_count;
size_t read_count = 0;
size_t write_count = 0;

const size_t M = 50 * 1024 * 1024; // 50 MB
const size_t INT64_SIZE = sizeof(int64_t);
const size_t N = 60 * (M / INT64_SIZE);

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

double test_mergesort(const string& input_file, const string& output_file, size_t d, double& avg_reads, double& avg_writes) {
    vector<double> times;
    vector<size_t> reads, writes;

    for (int i = 0; i < 5; ++i) {
        read_count = write_count = 0;
        auto t1 = high_resolution_clock::now();
        ext_aridad_mergesort(input_file, output_file, M, d); 
        auto t2 = high_resolution_clock::now();
        times.push_back(duration<double>(t2 - t1).count());
        reads.push_back(read_count);
        writes.push_back(write_count);
    }

    auto avg = [](const vector<double>& v) {
        double sum = 0;
        for (double x : v) sum += x;
        return sum / v.size();
    };
    auto avg_size = [](const vector<size_t>& v) {
        size_t sum = 0;
        for (size_t x : v) sum += x;
        return static_cast<double>(sum) / v.size();
    };

    avg_reads = avg_size(reads);
    avg_writes = avg_size(writes);
    return avg(times); // retorna tiempo promedio
}

void buscar_aridad_optima() {
    string input_file = "busqueda_aridad_input.bin";
    string output_file = "busqueda_aridad_output.bin";
    generate_random_file(input_file, N);

    int lo = 2, hi = 64;
    int best_d = -1;
    double best_metric = numeric_limits<double>::max();

    cout << "Buscando aridad óptima...\n";

    while (lo <= hi) {
        int mid = (lo + hi) / 2;

        double avg_reads, avg_writes;
        double avg_time = test_mergesort(input_file, output_file, mid, avg_reads, avg_writes);

        // Cambia esta línea según qué métrica optimizar: tiempo, lecturas o escrituras
        double metric = avg_time; // o avg_reads, o avg_reads + avg_writes

        cout << "Aridad " << mid << ": tiempo=" << avg_time
             << "s, lecturas=" << avg_reads << ", escrituras=" << avg_writes << "\n";

        if (metric < best_metric) {
            best_metric = metric;
            best_d = mid;
        }

        // Heurística: avanzar más rápido hacia mejores tiempos
        if (mid < hi && metric > best_metric)
            hi = mid - 1;
        else
            lo = mid + 1;
    }

    cout << "\n>> Mejor aridad encontrada: d = " << best_d << " con métrica = " << best_metric << "\n";
}

int main() {
    buscar_aridad_optima();
    return 0;
}
