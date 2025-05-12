#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <random>
#include <climits>
#include "mergesort_ext_aridad.h"
#include "contadores.h" 

using namespace std;
using namespace chrono;

/**
 * @brief Variables globales que cuentan el número de lecturas y escrituras realizadas durante las pruebas.
 */
extern size_t read_count;  ///< Contador de lecturas de bloques.
extern size_t write_count; ///< Contador de escrituras de bloques.

size_t read_count = 0;     ///< Inicialización del contador de lecturas.
size_t write_count = 0;    ///< Inicialización del contador de escrituras.

const size_t M = 50 * 1024 * 1024; ///< Tamaño del bloque (50 MB).
const size_t INT64_SIZE = sizeof(int64_t); ///< Tamaño en bytes de un valor de tipo int64_t.
const size_t N = 60 * (M / INT64_SIZE); ///< Número de elementos a procesar, basado en el tamaño de M y el tamaño de int64_t.

/**
 * @brief Genera un archivo binario con valores aleatorios de tipo int64_t.
 * 
 * @param filename Nombre del archivo donde se guardarán los datos generados.
 * @param num_elements Número de elementos (valores aleatorios) a generar.
 */
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

/**
 * @brief Realiza pruebas de rendimiento con el algoritmo de MergeSort para un archivo de entrada y salida.
 * 
 * @param input_file Nombre del archivo de entrada.
 * @param output_file Nombre del archivo de salida.
 * @param d Aridad para la función de MergeSort.
 * @param avg_reads Promedio de lecturas realizadas durante las pruebas.
 * @param avg_writes Promedio de escrituras realizadas durante las pruebas.
 * 
 * @return El tiempo promedio de ejecución en segundos.
 */
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
    return avg(times); 
}
/**
 * @brief Busca la aridad óptima para el algoritmo de MergeSort mediante una búsqueda binaria.
 * Realiza pruebas de rendimiento con distintas aridades y selecciona la mejor.
 */
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

        
        double metric = avg_time; 

        cout << "Aridad " << mid << ": tiempo=" << avg_time
             << "s, lecturas=" << avg_reads << ", escrituras=" << avg_writes << "\n";

        if (metric < best_metric) {
            best_metric = metric;
            best_d = mid;
        }

        if (mid < hi && metric > best_metric)
            hi = mid - 1;
        else
            lo = mid + 1;
    }

    cout << "\n>> Mejor aridad encontrada: d = " << best_d << " con métrica = " << best_metric << "\n";
}
/**
 * @brief Función principal que ejecuta el proceso de búsqueda de la aridad óptima.
 * 
 * @return Código de salida del programa (generalmente 0).
 */
int main() {
    buscar_aridad_optima();
    return 0;
}
