# Tarea 1 — Mergesort externo vs. Quicksort externo

CC4102 — Diseño y Análisis de Algoritmos  
Facultad de Ciencias Físicas y Matemáticas — Universidad de Chile

---

## 📦 Estructura del proyecto

CC4102/
├── include/ # Archivos de cabecera (.h)
│ ├── contadores.h
│ ├── mergesort_ext_aridad.h
│ ├── mergesort_externo.h
│ └── quicksort_externo.h
│
├── src/ # Código fuente (.cpp)
│ ├── buscar_aridad.cpp # Búsqueda de aridad óptima
│ ├── contadores.cpp # Contadores de accesos a disco
│ ├── main.cpp # Experimento principal
│ ├── main_mergesort.cpp # Test unitario mergesort
│ ├── main_quicksort.cpp # Test unitario quicksort
│ ├── mergesort_ext_aridad.cpp # Mergesort con aridad
│ ├── mergesort_externo.cpp # Versión alternativa
│ └── quicksort_externo.cpp # Implementación de quicksort externo
│
├── Makefile # Compilación automática
└── README.md # Este archivo

yaml
Copiar
Editar

## 🛠️ Requisitos

Antes de compilar y ejecutar, asegúrate de tener lo siguiente instalado:

### ✅ Sistema operativo compatible
- Linux, macOS o Windows con WSL/MinGW/MSYS2

### ✅ Compilador C++
- `g++` versión 9 o superior (soporta C++17)

### ✅ Docker *(solo para experimentación final)*
Para la ejecución con memoria limitada:
- Docker instalado desde: https://www.docker.com/
- Imagen del curso: [https://hub.docker.com/r/pabloskewes/cc4102-cpp-env](https://hub.docker.com/r/pabloskewes/cc4102-cpp-env)

---

## ⚙️ Instrucciones de compilación

Desde la raíz del proyecto:

1. **Compilar el proyecto principal:**

```bash
make
```

# Extra:

2. **Compilar en MinGW:**

```bash
g++ -std=c++17 -IC:\Users\pablo\OneDrive\Documentos\CC4102\CC4102\include .\src\main.cpp .\src\quicksort_externo.cpp .\src\mergesort_ext_aridad.cpp .\src\contadores.cpp -o main

```

```bash
docker run --rm -it -m 50m -v ${PWD}:/workspace pabloskewes/cc4102-cpp-env bash

```

g++ -std=c++17 -IC:/Users/pablo/OneDrive/Documentos/CC4102/CC4102/include ./src/main.cpp ./src/quicksort_externo.cpp ./src/mergesort_ext_aridad.cpp ./src/contadores.cpp -o main


## Abrir el docker:

docker run --rm -it -m 50m -v ${PWD}:/workspace pabloskewes/cc4102-cpp-env bash

## Compilar programa en el docker:

g++ -std=c++17 -IC:/Users/pablo/OneDrive/Documentos/CC4102/CC4102/src ./src/main.cpp ./src/quicksort_externo.cpp ./src/mergesort_ext_aridad.cpp ./src/contadores.cpp -o main

## Ejecutar en Docker:

./main

## Compilar en MinGW:

g++ -std=c++17 -IC:\Users\pablo\OneDrive\Documentos\CC4102\CC4102\src .\src\main.cpp .\src\quicksort_externo.cpp .\src\mergesort_ext_aridad.cpp .\src\contadores.cpp -o main

## Ejecutar en MinGW:

.\main 

## Buscar aridad óptima en MinGW:

g++ -std=c++17 -IC:\Users\pablo\OneDrive\Documentos\CC4102\CC4102\src .\src\buscar_aridad_ternaria.cpp .\src\mergesort_ext_aridad.cpp .\src\contadores.cpp -o buscar_aridad

## Ejecutar en MinGW:

.\buscar_aridad






