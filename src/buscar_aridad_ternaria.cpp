#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <random>
#include <climits>
#include <cstdio> 
#include <filesystem>
namespace fs = std::filesystem;


#include "mergesort_ext_aridad.h"

using namespace std;
using namespace chrono;

extern size_t read_count;
extern size_t write_count;
size_t read_count = 0;
size_t write_count = 0;

const size_t M = 50 * 1024 * 1024; // 50 MB
const size_t INT64_SIZE = sizeof(int64_t);
const size_t N = 60 * (M / INT64_SIZE);

void eliminar_archivos_merged() {
    namespace fs = std::filesystem;
    for (const auto& entry : fs::directory_iterator(".")) {
        if (entry.is_regular_file()) {
            string nombre = entry.path().filename().string();
            if (nombre.rfind("merged_", 0) == 0 && entry.path().extension() == ".bin") {
                fs::remove(entry.path());
            }
        }
    }
}

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
    cout << ">>> Probando a = " << d << endl;  // <-- añade esto
    vector<double> times;
    vector<size_t> reads, writes;

    for (int i = 0; i < 2; ++i) {
        cout << ">>> Iteración " << i << endl;  // <-- añade esto
        eliminar_archivos_merged();
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
    // Eliminar archivos temporales merged_*.bin después de cada corrida
    eliminar_archivos_merged();
    return avg(times); 
}


void buscar_aridad_optima_ternaria() {
    string input_file = "busqueda_aridad_input.bin";
    string output_file = "busqueda_aridad_output.bin";
    generate_random_file(input_file, N);

    size_t B = 4096; // tamaño del bloque en bytes
    int b = B / 8;   // número máximo de pivotes
    int lo = 2, hi = b;

    int best_d = -1;
    double best_metric = numeric_limits<double>::max();

    cout << "Buscando aridad óptima con búsqueda ternaria...\n";

    while (hi - lo > 3) {
        int m1 = lo + (hi - lo) / 3;
        int m2 = hi - (hi - lo) / 3;

        double avg_reads1, avg_writes1;
        double avg_time1 = test_mergesort(input_file, output_file, m1, avg_reads1, avg_writes1);
        double metric1 = avg_time1*0.1 + (avg_reads1 + avg_writes1); // el factor puede ajustarse

        double avg_reads2, avg_writes2;
        double avg_time2 = test_mergesort(input_file, output_file, m2, avg_reads2, avg_writes2);
        double metric2 = avg_time2*0.1 + (avg_reads2 + avg_writes2);

        cout << "a = " << m1 << ": tiempo = " << avg_time1 << "s, I/Os = " << metric1 << "\n";
        cout << "a = " << m2 << ": tiempo = " << avg_time2 << "s, I/Os = " << metric2 << "\n";

        if (metric1 < metric2) {
            hi = m2;
            if (metric1 < best_metric) {
                best_metric = metric1;
                best_d = m1;
            }
        } else {
            lo = m1;
            if (metric2 < best_metric) {
                best_metric = metric2;
                best_d = m2;
            }
        }
    }

    for (int d = lo; d <= hi; ++d) {
        double avg_reads, avg_writes;
        double avg_time = test_mergesort(input_file, output_file, d, avg_reads, avg_writes);
        double metric = avg_reads + avg_writes;
        cout << "a = " << d << ": tiempo = " << avg_time << "s, I/Os = " << metric << "\n";
        if (metric < best_metric) {
            best_metric = metric;
            best_d = d;
        }
    }

    cout << "\n>> Mejor aridad encontrada: a = " << best_d << " con tiempo = " << best_metric << "s\n";

    if (remove(input_file.c_str()) == 0)
        cout << "Archivo de entrada eliminado.\n";
    else
        cerr << "Error al eliminar archivo de entrada.\n";

    if (remove(output_file.c_str()) == 0)
        cout << "Archivo de salida eliminado.\n";
    else
        cerr << "Error al eliminar archivo de salida.\n";
}

int main() {
    buscar_aridad_optima_ternaria();
    return 0;
}

