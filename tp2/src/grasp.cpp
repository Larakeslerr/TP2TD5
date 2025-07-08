#include "grasp.h"
#include "armarRutasCortas.h"
#include "busqueda_local.h"
#include "VRPLIBReader.h"
#include "CVRP_Solution.h"
#include "armarRutasCortasAleatorizado.h"
#include <limits>
#include <random>
#include <algorithm>
#include <iostream>

Solution grasp(const VRPLIBReader& reader, int n_iters, int rcl_size) {
    // Paso 1: preparar datos
    std::vector<Node> nodos = reader.getNodes();
    std::vector<int> demandas = reader.getDemands();
    std::vector<Cliente> clientes;
    for (const auto& nodo : nodos) {
        clientes.push_back({nodo.id, nodo.x, nodo.y, demandas[nodo.id]});
    }

    // Asegurar que el depósito esté en la primera posición
    int depot_id = reader.getDepotId();
    auto it = std::find_if(clientes.begin(), clientes.end(), [&](const Cliente& c) { return c.id == depot_id; });
    if (it != clientes.end()) {
        std::iter_swap(clientes.begin(), it);
    }

    const auto& distancias = reader.getDistanceMatrix();
    int capacidad = reader.getCapacity();

    Solution mejorSol;
    double mejorCosto = std::numeric_limits<double>::infinity();

    for (int k = 0; k < n_iters; ++k) {
        // Paso 3: construir una solución greedy aleatorizada
        std::vector<std::vector<int>> rutas = armarRutasCortasAleatorizado(clientes, capacidad, distancias, rcl_size);

        // Paso 4: aplicar búsqueda local
        auto rutas_opt = busquedaLocal2opt(rutas, distancias);

        // Calcular costo
        double costo = 0.0;
        for (const auto& ruta : rutas_opt) {
            for (size_t i = 0; i + 1 < ruta.size(); ++i) {
                int from = ruta[i];
                int to = ruta[i + 1];
                auto it_from = std::find_if(clientes.begin(), clientes.end(), [&](const Cliente& c){ return c.id == from; });
                auto it_to = std::find_if(clientes.begin(), clientes.end(), [&](const Cliente& c){ return c.id == to; });
                if (it_from != clientes.end() && it_to != clientes.end()) {
                    int idx_from = std::distance(clientes.begin(), it_from);
                    int idx_to = std::distance(clientes.begin(), it_to);
                    costo += distancias[idx_from][idx_to];
                }
            }
        }

        // Paso 5 y 6: guardar si es mejor
        if (costo < mejorCosto) {
            mejorCosto = costo;
            mejorSol = Solution();
            for (const auto& ruta : rutas_opt) {
                int suma_demanda = 0;
                for (int i = 1; i < ruta.size() - 1; ++i) {
                    int cliente_id = ruta[i];
                    suma_demanda += clientes[cliente_id].demanda;
                }
                mejorSol.agregarRuta(ruta, distancias, suma_demanda);
            }
        }
    }

    return mejorSol;
}

/*
-----------------------------------------------------------
Complejidad del algoritmo GRASP
-----------------------------------------------------------

Parámetros:
- n: cantidad de nodos (clientes + depósito)
- r: cantidad de rutas generadas por solución
- m: tamaño promedio de cada ruta (en general, r × m ≈ n)
- k: cantidad de iteraciones que hace 2-opt hasta estabilizar
- n_iters: cantidad de iteraciones externas de GRASP

Análisis por iteración:
- Construcción aleatorizada (armarRutasCortasAleatorizado): O(n³)
- Búsqueda local con 2-opt sobre r rutas: O(r × k × m³)
- Cálculo de costo y verificación de mejora: O(n)

Asumiendo r × m ≈ n, el costo por iteración es:
    → O(n³ + k × n³) = O(k × n³)

Como se realizan n_iters iteraciones del algoritmo GRASP, la complejidad total es:

    → O(n_iters × k × n³)

En la práctica:
- k es acotado (pocas mejoras locales)
- n_iters se fija manualmente (por ejemplo, 10 o 20)

Entonces, para casos reales: 
    → Complejidad efectiva ≈ O(n³)

-----------------------------------------------------------
*/

