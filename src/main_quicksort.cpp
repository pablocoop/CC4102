#include <iostream>
#include <fstream>
#include <random>
#include "quicksort_externo.h"
#include "contadores.h"


using namespace std;

int main() {
    const size_t N = 1000000; // número de elementos
    const size_t M = 50 * 1024 * 1024; // memoria disponible
    const size_t a = 4; // número de subarreglos

    // Crear archivo binario con datos aleatorios
    ofstream out("entrada.bin", ios::binary);
    random_device rd;
    mt19937_64 gen(rd());
    uniform_int_distribution<int64_t> dist(1, 1000000);

    for (size_t i = 0; i < N; ++i) {
        int64_t num = dist(gen);
        out.write(reinterpret_cast<const char *>(&num), sizeof(int64_t));
    }
    out.close();

    // Ejecutar quicksort externo
    fstream file("entrada.bin", ios::in | ios::out | ios::binary);
    external_quicksort(file, 0, N, M, a);
    file.close();

    cout << "Accesos a disco - Lecturas: " << read_count << ", Escrituras: " << write_count << endl;
    return 0;
}
