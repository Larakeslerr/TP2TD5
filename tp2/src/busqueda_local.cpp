#include "busqueda_local.h"
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

double calcularDistanciaRuta(const vector<int>& ruta, const vector<vector<double>>& distancias) {
    double total = 0.0;
    for (size_t i = 0; i < ruta.size() - 1; ++i) {
        total += distancias[ruta[i]][ruta[i + 1]];
    }
    return total;
}


vector<vector<int>> BusquedaLocalSwap(
    const vector<vector<int>>& rutas,
    const vector<vector<double>>& distancias,
    const vector<int>& demandas,
    int capacidad
) {
    vector<vector<int>> mejor_rutas = rutas;
    bool hayMejora = true;

    auto calcularCarga = [&](const vector<int>& ruta) {
        int carga = 0;
        for (int cliente : ruta) {
            if (cliente != 0) carga += demandas[cliente];
        }
        return carga;
    };

    while (hayMejora) {
        hayMejora = false;

        for (size_t r1 = 0; r1 < mejor_rutas.size(); ++r1) {
            for (size_t i = 1; i < mejor_rutas[r1].size() - 1; ++i) {
                for (size_t r2 = 0; r2 < mejor_rutas.size(); ++r2) {
                    if (r1 == r2) continue;  // Solo entre rutas distintas

                    for (size_t j = 1; j < mejor_rutas[r2].size() - 1; ++j) {
                        // Copiar rutas para testeo
                        vector<int> nueva_ruta1 = mejor_rutas[r1];
                        vector<int> nueva_ruta2 = mejor_rutas[r2];

                        swap(nueva_ruta1[i], nueva_ruta2[j]);

                        // Validar factibilidad
                        if (calcularCarga(nueva_ruta1) <= capacidad &&
                            calcularCarga(nueva_ruta2) <= capacidad) {

                            double dist_original = calcularDistanciaRuta(mejor_rutas[r1], distancias) +
                                                   calcularDistanciaRuta(mejor_rutas[r2], distancias);
                            double nueva_dist = calcularDistanciaRuta(nueva_ruta1, distancias) +
                                                calcularDistanciaRuta(nueva_ruta2, distancias);

                            if (nueva_dist < dist_original) {
                                mejor_rutas[r1] = nueva_ruta1;
                                mejor_rutas[r2] = nueva_ruta2;
                                hayMejora = true;
                            }
                        }
                    }
                }
            }
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


