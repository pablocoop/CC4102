#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <random>
#include <climits>
#include <cstdio>
#include <filesystem>
#include "contadores.h"
#include "mergesort_ext_aridad.h"

using namespace std;
using namespace chrono;

// Tamaño de memoria interna y tipo
const size_t M = 50 * 1024 * 1024; // 50 MB
const size_t INT64_SIZE = sizeof(int64_t);
const size_t N = 60 * (M / INT64_SIZE);  // número de elementos en archivo de prueba


/**
 * @brief  Elimina todos los archivos temporales run_*.bin y merged_*.bin en cwd.
 */
void eliminar_archivos_merged() {
    namespace fs = std::filesystem;
    for (const auto& entry : fs::directory_iterator(".")) {
        if (entry.is_regular_file()) {
            string nombre = entry.path().filename().string();
            if ((nombre.rfind("merged_", 0) == 0 || nombre.rfind("run_", 0) == 0) && entry.path().extension() == ".bin") {
                fs::remove(entry.path());
            }
        }
    }
}

/**
 * @brief  Genera un archivo binario con enteros aleatorios de 64 bits.
 * @param  filename     Ruta del archivo de salida.
 * @param  num_elements Número de enteros a generar.
 */
void generate_random_file(const string &filename, size_t num_elements) {
    ofstream out(filename, ios::binary);
    random_device rd;
    mt19937_64 gen(rd());
    uniform_int_distribution<int64_t> dist(1, 1000000);
    for (size_t i = 0; i < num_elements; ++i) {
        int64_t val = dist(gen);
        out.write(reinterpret_cast<const char*>(&val), INT64_SIZE);
    }
}

/**
 * @brief  Ejecuta ext_aridad_mergesort una vez y captura I/O y tiempo.
 * @param  in_file     Ruta del archivo de entrada.
 * @param  out_file    Ruta del archivo de salida.
 * @param  d           Aridad de merge.
 * @param[out] avg_reads  Bloques leídos (read_count).
 * @param[out] avg_writes Bloques escritos (write_count).
 * @return Tiempo de ejecución en segundos.
 */
double test_mergesort(const string& in_file, const string& out_file, size_t d, double& avg_reads, double& avg_writes) {
    eliminar_archivos_merged();
    read_count = write_count = 0;
    auto t1 = high_resolution_clock::now();
    ext_aridad_mergesort(in_file, out_file, M, d);
    auto t2 = high_resolution_clock::now();
    avg_reads = read_count;
    avg_writes = write_count;
    return duration<double>(t2 - t1).count();
}

/**
 * @brief  Búsqueda ternaria de la aridad óptima en [2..b],  
 *         imprimiendo comparaciones I/O(a1) vs I/O(a2).
 */
void buscar_aridad_optima_ternaria() {
    const string input_file = "busqueda_aridad_input.bin";
    const string output_file = "busqueda_aridad_output.bin";
    generate_random_file(input_file, N);

    size_t B = 4096;
    int b = int(B / 8);      // número máximo de pivotes según tamaño de bloque
    int lo = 2, hi = b;
    int best_d = lo;
    double best_metric = numeric_limits<double>::max();

    cout << "Buscando aridad óptima con búsqueda ternaria en ["<< lo <<","<< hi <<"]...\n";
    while (hi - lo > 3) {
        int m1 = lo + (hi - lo) / 3;
        int m2 = hi - (hi - lo) / 3;
        double r1, w1, r2, w2;
        double t1 = test_mergesort(input_file, output_file, m1, r1, w1);
        double metric1 = r1 + w1;
        double t2 = test_mergesort(input_file, output_file, m2, r2, w2);
        double metric2 = r2 + w2;
        cout << "  a1="<< m1 <<": I/Os="<< metric1 <<" (t="<< t1 <<"s)";
        cout << " | a2="<< m2 <<": I/Os="<< metric2 <<" (t="<< t2 <<"s)\n";
        if (metric1 <= metric2) {
            hi = m2;
            if (metric1 < best_metric) { best_metric = metric1; best_d = m1; }
        } else {
            lo = m1;
            if (metric2 < best_metric) { best_metric = metric2; best_d = m2; }
        }
    }
    for (int d = lo; d <= hi; ++d) {
        double r, w;
        test_mergesort(input_file, output_file, d, r, w);
        double metric = r + w;
        cout << "  probar d="<< d <<": I/Os="<< metric <<"\n";
        if (metric < best_metric) { best_metric = metric; best_d = d; }
    }

    cout << "\n>> Mejor aridad encontrada: a="<< best_d <<" con I/Os="<< best_metric <<"\n";
    eliminar_archivos_merged();
    remove(input_file.c_str());
    remove(output_file.c_str());
}

/**
 * @brief  Función main de prueba: compara sólo a=10 y a=50.
 * @return Código de salida.
 */
int main() {
    const string input_file  = "busqueda_aridad_input.bin";
    const string output_file = "busqueda_aridad_output.bin";

    // Genera el fichero de prueba
    generate_random_file(input_file, N);

    // Valores de aridad a probar
    vector<size_t> pruebas = {10, 50};
    for (size_t d : pruebas) {
        double reads, writes;
        cout << "\n>>> Probando aridad a = " << d << " <<<\n";
        double t = test_mergesort(input_file, output_file, d, reads, writes);
        cout << "Resultado para a=" << d << " -> "
             << "Tiempo: " << t << "s, "
             << "I/Os: " << (reads + writes)
             << " (Lecturas=" << reads
             << ", Escrituras=" << writes << ")\n";
    }

    // Limpieza
    eliminar_archivos_merged();
    remove(input_file.c_str());
    remove(output_file.c_str());
    return 0;
}
