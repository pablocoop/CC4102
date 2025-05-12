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

/**
 * @brief  Genera un archivo binario con numeros aleatorios de 64 bits.
 * @tparam URNG  Tipo de motor de aleatoriedad.
 * @param  filename     Ruta del archivo de salida.
 * @param  num_elements Número de enteros a generar.
 * @param  gen          Generador de números aleatorios.
 */
template<typename URNG>
void generate_random_file(const string &filename, size_t num_elements, URNG &gen) {
    ofstream out(filename, ios::binary);
    uniform_int_distribution<int64_t> dist(1, 1000000);
    for (size_t i = 0; i < num_elements; ++i) {
        int64_t val = dist(gen);
        out.write(reinterpret_cast<const char *>(&val), INT64_SIZE);
    }
}

/**
 * @brief  Ejecuta 5 corridas de Mergesort y Quicksort externo,  
 *         promedia tiempos e I/Os y escribe en out.
 * @param  N_elems  Número de enteros en cada archivo de prueba.
 * @param  a        Aridad / número de pivotes.
 * @param  out      Stream abierto para volcar resultados.
 */
void run_experiment(size_t N_elems, size_t a, ofstream &out) {
    // Prefijos para archivos temporales
    string input_prefix = "input_" + to_string(N_elems);
    string merge_prefix = "merge_out_" + to_string(N_elems);

    // Contenedores para resultados (5 corridas)
    vector<double> merge_times, quick_times;
    vector<size_t> merge_reads, merge_writes;
    vector<size_t> quick_reads, quick_writes;

    // Random engine que persiste semilla para reproducibilidad
    random_device rd;
    mt19937_64 gen(rd());

    for (int run = 0; run < 5; ++run) {
        string in_file  = input_prefix + "_run"  + to_string(run) + ".bin";
        string out_file = merge_prefix + "_run" + to_string(run) + ".bin";

        // Generar datos aleatorios
        generate_random_file(in_file, N_elems, gen);

        // --- Mergesort externo ---
        read_count = write_count = 0;
        auto t0 = high_resolution_clock::now();
        ext_aridad_mergesort(in_file, out_file, M, a);
        auto t1 = high_resolution_clock::now();
        merge_times.push_back(duration<double>(t1 - t0).count());
        merge_reads .push_back(read_count);
        merge_writes.push_back(write_count);

        // --- Quicksort externo ---
        read_count = write_count = 0;
        fstream f(in_file, ios::in | ios::out | ios::binary);
        auto q0 = high_resolution_clock::now();
        external_quicksort(f, 0, N_elems, M, a);
        auto q1 = high_resolution_clock::now();
        quick_times .push_back(duration<double>(q1 - q0).count());
        quick_reads .push_back(read_count);
        quick_writes.push_back(write_count);
        f.close();

        // Eliminar archivos temporales
        remove(in_file.c_str());
        remove(out_file.c_str());
    }

    // Función auxiliar para promedio
    auto avg_size = [&](const auto &v) {
        double sum = 0;
        for (auto x : v) sum += x;
        return sum / v.size();
    };

    double avg_mt = avg_size(merge_times);
    double avg_mr = avg_size(merge_reads);
    double avg_mw = avg_size(merge_writes);
    double avg_qt = avg_size(quick_times);
    double avg_qr = avg_size(quick_reads);
    double avg_qw = avg_size(quick_writes);

    // Salida: N en bytes, tiempos e I/Os promedio
    size_t N_bytes = N_elems * INT64_SIZE;
    out << N_bytes << " ";
    out << avg_mt << " " << (avg_mr + avg_mw) << " ";
    out << avg_qt << " " << (avg_qr + avg_qw) << "\n";
}

/**
 * @brief  Función main: itera sobre {4M,8M,16M,32M,60M},  
 *         llama a run_experiment y cierra resultados.
 * @return Código de salida.
 */
int main() {
    // Factores de tamaño en múltiplos de M
    vector<size_t> factors = {4, 8, 16, 32, 60};
    size_t a = /* tu aridad óptima */ 50;

    ofstream out("resultados.txt");
    if (!out) {
        cerr << "No se pudo abrir resultados.txt\n";
        return 1;
    }

    // Cabecera: N_bytes Merge_time Merge_IO Quick_time Quick_IO
    out << "# N_bytes Merge_time Merge_IO Quick_time Quick_IO\n";

    for (auto f : factors) {
        size_t N_elems = (M / INT64_SIZE) * f;
        cout << "Experimentando N=" << N_elems << " elementos...\n";
        run_experiment(N_elems, a, out);
    }

    out.close();
    cout << "Experimento completado: resultados.txt generado.\n";
    return 0;
}
