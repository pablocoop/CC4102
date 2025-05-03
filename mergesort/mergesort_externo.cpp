#include "mergesort_externo.h"
#include <fstream>
#include <algorithm>
#include <cstdio> // Para std::remove

MergeSortExterno::MergeSortExterno(size_t memoriaDisponible, size_t tamanoBloque)
    : M(memoriaDisponible), B(tamanoBloque) {}

void MergeSortExterno::ordenar(const std::string& archivoEntrada, const std::string& archivoSalida) {
    std::vector<std::string> archivosRuns;
    generarRunsIniciales(archivoEntrada, archivosRuns);
    mezclarRuns(archivosRuns, archivoSalida);

    for (const auto& archivo : archivosRuns) {
        std::remove(archivo.c_str()); 
    }
}

void MergeSortExterno::generarRunsIniciales(const std::string& archivoEntrada, std::vector<std::string>& archivosRuns) {
    // Leer el archivo en bloques de tamaño M, ordenar en memoria y escribir en archivos temporales
}

void MergeSortExterno::mezclarRuns(const std::vector<std::string>& archivosRuns, const std::string& archivoSalida) {
    // Repetir mezcla de archivos hasta obtener el archivo final ordenado
}

void MergeSortExterno::mezclarDosRuns(const std::string& archivo1, const std::string& archivo2, const std::string& archivoSalida) {
    // Leer con buffers de tamaño B y mezclar los archivos ordenadamente
}