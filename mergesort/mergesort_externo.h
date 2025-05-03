#ifndef MERGESORT_EXTERNO_H
#define MERGESORT_EXTERNO_H

#include <string>
#include <vector>

class MergeSortExterno {
public:
    MergeSortExterno(size_t memoriaDisponible, size_t tamanoBloque);

    void ordenar(const std::string& archivoEntrada, const std::string& archivoSalida);

private:
    size_t M; // Memoria disponible (en bytes o elementos)
    size_t B; // Tamaño del bloque (en bytes o elementos)

    void generarRunsIniciales(const std::string& archivoEntrada, std::vector<std::string>& archivosRuns);
    void mezclarRuns(const std::vector<std::string>& archivosRuns, const std::string& archivoSalida);
    void mezclarDosRuns(const std::string& archivo1, const std::string& archivo2, const std::string& archivoSalida);
};

#endif // MERGESORT_EXTERNO_H