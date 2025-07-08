#include "busqueda_local.h"
#include <vector>
#include <algorithm>

using namespace std;

double calcularDistanciaRuta(const vector<int>& ruta, const vector<vector<double>>& distancias) {
    double total = 0.0;
    for (size_t i = 0; i < ruta.size() - 1; ++i) {
        total += distancias[ruta[i]][ruta[i + 1]];
    }
    return total;
}

vector<vector<int>> busquedaLocalSwap(
    const vector<vector<int>>& rutas,
    const vector<vector<double>>& distancias
) {
    vector<vector<int>> mejor_rutas = rutas;
    bool hayMejora = true;

    while (hayMejora) {
        hayMejora = false;

        for (auto& ruta : mejor_rutas) {
            double mejor_dist = calcularDistanciaRuta(ruta, distancias);
            vector<int> mejor_ruta = ruta;

            for (size_t i = 1; i < ruta.size() - 1; ++i) {
                for (size_t j = i + 1; j < ruta.size() - 1; ++j) {
                    vector<int> nueva_ruta = ruta;
                    swap(nueva_ruta[i], nueva_ruta[j]);

                    double nueva_dist = calcularDistanciaRuta(nueva_ruta, distancias);
                    if (nueva_dist < mejor_dist) {
                        mejor_dist = nueva_dist;
                        mejor_ruta = nueva_ruta;
                        hayMejora = true;
                    }
                }
            }

            ruta = mejor_ruta;
        }
    }

    return mejor_rutas;
}

vector<int> aplicar2opt(const vector<int>& ruta, const vector<vector<double>>& distancias) {
    vector<int> mejor_ruta = ruta;
    bool mejora = true;

    while (mejora) {
        mejora = false;
        double mejor_dist = calcularDistanciaRuta(mejor_ruta, distancias);

        for (size_t i = 1; i < mejor_ruta.size() - 2; ++i) {
            for (size_t j = i + 1; j < mejor_ruta.size() - 1; ++j) {
                vector<int> nueva_ruta = mejor_ruta;
                reverse(nueva_ruta.begin() + i, nueva_ruta.begin() + j + 1);

                double nueva_dist = calcularDistanciaRuta(nueva_ruta, distancias);
                if (nueva_dist < mejor_dist) {
                    mejor_ruta = nueva_ruta;
                    mejor_dist = nueva_dist;
                    mejora = true;
                }
            }
        }
    }

    return mejor_ruta;
}

vector<vector<int>> busquedaLocal2opt(
    const vector<vector<int>>& rutas,
    const vector<vector<double>>& distancias
) {
    vector<vector<int>> resultado;
    for (const auto& ruta : rutas) {
        resultado.push_back(aplicar2opt(ruta, distancias));
    }
    return resultado;
}

/*
-----------------------------------------------------------
Complejidad de búsqueda local: Swap y 2-opt
-----------------------------------------------------------

Sea:
- r = cantidad de rutas
- m = tamaño máximo de una ruta (cantidad de nodos en la ruta, típicamente m ≪ n)
- n = cantidad total de nodos (para referencia global)

-----------------------------------------------------------
1. busquedaLocalSwap(...)
-----------------------------------------------------------

Para cada ruta:
- Se considera cada par de posiciones (i, j) dentro de la ruta: O(m²)
- Para cada swap se recalcula la distancia de la ruta: O(m)
- Se repite mientras haya mejora (puede ser varias iteraciones)

Peor caso por ruta: O(k × m³), donde k es la cantidad de iteraciones hasta converger

Total para todas las rutas: O(r × k × m³)

En la práctica, k es pequeño y m ≪ n, así que el algoritmo es eficiente para rutas cortas.

-----------------------------------------------------------
2. aplicar2opt(...)
-----------------------------------------------------------

Para una sola ruta:
- Se consideran pares (i, j) donde 1 ≤ i < j ≤ m-2 → O(m²)
- Cada reversa de segmento es O(m), y se recalcula la distancia: O(m)
- Se repite hasta no mejorar → multiplicado por k iteraciones

Complejidad por ruta: O(k × m³)

-----------------------------------------------------------
3. busquedaLocal2opt(...)
-----------------------------------------------------------

Llama a aplicar2opt(...) sobre cada ruta → O(r × k × m³)

-----------------------------------------------------------
Resumen:
- Complejidad temporal Swap:      O(r × k × m³)
- Complejidad temporal 2-opt:     O(r × k × m³)
*/
