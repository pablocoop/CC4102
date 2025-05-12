# Tarea 1 — Mergesort externo vs. Quicksort externo

**CC4102 — Diseño y Análisis de Algoritmos**  
Facultad de Ciencias Físicas y Matemáticas — Universidad de Chile

---

## 📦 Estructura del proyecto

```
CC4102/
│
├── src/                        # Código fuente (.cpp)
│   ├── buscar_aridad.cpp           # Búsqueda lineal de aridad
│   ├── buscar_aridad_ternaria.cpp  # Búsqueda ternaria de aridad
│   ├── contadores.cpp              # Implementación de contadores de I/O
│   ├── main.cpp                    # Experimento principal (genera resultados)
│   ├── mergesort_ext_aridad.cpp    # MergeSort externo con aridad variable
│   ├── quicksort_externo.cpp       # QuickSort externo por comparaciones
│   ├── (otros mains de test unitario, opcional)
│   ├── contadores.h
│   ├── mergesort_ext_aridad.h
│   └── quicksort_externo.h
├── Makefile                    # Compilación automática
└── README.md                   # Este archivo
```

---

## 🛠️ Requisitos

Antes de compilar y ejecutar, instala:

- **SO**: Linux, macOS o Windows (WSL / MinGW / MSYS2).  
- **Compilador C++**: `g++` ≥ 9 (C++17).  
- **Docker** *(opcional para entornos limitados)*  
  - Docker Desktop/Engine  
  - Imagen del curso: `pabloskewes/cc4102-cpp-env`

---

## ⚙️ Compilación

### Usando Makefile

```bash
# Desde la raíz del repositorio
make
```

Genera:

- `main`          → Ejecuta el experimento completo (Merge vs. Quick).  
- `buscar_aridad` → Busca la aridad óptima con búsqueda ternaria.

---

### Compilación Manual

#### En Linux/macOS / WSL

```bash
INC=include
SRC=src

# Experimento principal
g++ -std=c++17 -I${INC}     ${SRC}/main.cpp     ${SRC}/contadores.cpp     ${SRC}/mergesort_ext_aridad.cpp     ${SRC}/quicksort_externo.cpp     -o main

# Búsqueda de aridad óptima
g++ -std=c++17 -I${INC}     ${SRC}/buscar_aridad_ternaria.cpp     ${SRC}/contadores.cpp     ${SRC}/mergesort_ext_aridad.cpp     -o buscar_aridad
```

#### En Windows (MinGW)

Ajusta las rutas al include:

```bash
g++ -std=c++17 -IC:/ruta/a/CC4102/include     src\main.cpp src\contadores.cpp src\mergesort_ext_aridad.cpp src\quicksort_externo.cpp     -o main.exe

g++ -std=c++17 -IC:/ruta/a/CC4102/include     src\buscar_aridad_ternaria.cpp src\contadores.cpp src\mergesort_ext_aridad.cpp     -o buscar_aridad.exe
```

---

## 🐳 Uso con Docker

Para un contenedor limitado a 300 MB:

```bash
docker run --rm -it -m 300m -v "${PWD}:/workspace" pabloskewes/cc4102-cpp-env bash
cd /workspace
make
```

Dentro:

```bash
./buscar_aridad       # Busca aridad óptima
./main                # Ejecuta el experimento completo
```

---

## 🚀 Ejecución

1. **Buscar aridad óptima**  
   ```bash
   ./buscar_aridad
   ```
   Muestra en consola comparaciones I/O(a₁) vs. I/O(a₂) y el valor óptimo.

2. **Experimento completo**  
   ```bash
   ./main
   ```
   Genera `resultados.txt` con filas:
   ```
   N_bytes Merge_time Merge_IO Quick_time Quick_IO
   ```
   donde:
   - **N_bytes**: tamaño en bytes de la secuencia.
   - **Merge_time**: tiempo medio de MergeSort externo (s).
   - **Merge_IO**: I/O medio (lecturas+escrituras bloques).
   - **Quick_time**: tiempo medio de QuickSort externo (s).
   - **Quick_IO**: I/O medio (lecturas+escrituras bloques).

---


## 📖 Referencias

- Enunciado T1 (t1.pdf)  
- Apuntes de Memoria Externa CC4102  
- [Doxygen](https://www.doxygen.nl/)  
