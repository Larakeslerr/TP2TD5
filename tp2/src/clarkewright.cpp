#include "clarkewright.h"
#include <cmath>
#include <algorithm>
#include <set>
#include <vector>
#include <unordered_map>
#include <tuple>
#include "armarRutasCortas.h" 
#include "CVRP_Solution.h"
#include "VRPLIBReader.h"
using namespace std;  // O cambiar vector por std::vector en cada uso

double distancia(const Cliente& a, const Cliente& b) {
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

vector<vector<int>> clarkewright(const vector<Cliente>& clientes, int capacidad) {
    const Cliente& deposito = clientes[0];
    unordered_map<int, vector<int>> rutas;
    unordered_map<int, int> demandas;

    for (size_t i = 1; i < clientes.size(); ++i) {
        rutas[clientes[i].id] = {deposito.id, clientes[i].id, deposito.id};
        demandas[clientes[i].id] = clientes[i].demanda;
    }

    vector<tuple<double, int, int>> savings;
    for (size_t i = 1; i < clientes.size(); ++i) {
        for (size_t j = i + 1; j < clientes.size(); ++j) {
        double ahorro = distancia(deposito, clientes[i]) + distancia(deposito, clientes[j]) - distancia(clientes[i], clientes[j]);
            savings.emplace_back(ahorro, clientes[i].id, clientes[j].id);
        }
    }

    sort(savings.rbegin(), savings.rend());

    for (const auto& s : savings) {
        double ahorro;
        int i, j;
        tie(ahorro, i, j) = s;

        if (rutas.count(i) && rutas.count(j) && rutas[i] != rutas[j]) {
            vector<int>& ruta_i = rutas[i];
            vector<int>& ruta_j = rutas[j];

            int demanda_total = 0;
            for (int k = 1; k < ruta_i.size() - 1; ++k)
                demanda_total += demandas[ruta_i[k]];
            for (int k = 1; k < ruta_j.size() - 1; ++k)
                demanda_total += demandas[ruta_j[k]];

            if (demanda_total <= capacidad && ruta_i[ruta_i.size() - 2] == i && ruta_j[1] == j) {
                vector<int> nueva_ruta;
                nueva_ruta.insert(nueva_ruta.end(), ruta_i.begin(), ruta_i.end() - 1);
                nueva_ruta.insert(nueva_ruta.end(), ruta_j.begin() + 1, ruta_j.end());

                for (int k = 1; k < nueva_ruta.size() - 1; ++k) {
                    rutas[nueva_ruta[k]] = nueva_ruta;
                }
            }
        }
    }

    set<vector<int>> unicas;
    for (const auto& par : rutas) {
        unicas.insert(par.second);
    }

    return vector<vector<int>>(unicas.begin(), unicas.end());
}

Solution solveClarkeWright(const VRPLIBReader& instance) {
    // Paso 1: convertir a vector<Cliente>
    vector<Node> nodos = instance.getNodes();
    vector<int> demandas = instance.getDemands();
    vector<Cliente> clientes;

    for (const auto& nodo : nodos) {
        Cliente c;
        c.id = nodo.id;
        c.x = nodo.x;
        c.y = nodo.y;
        c.demanda = demandas[nodo.id];
        clientes.push_back(c);
    }

    // Paso 2: obtener rutas
    const auto& distancias = instance.getDistanceMatrix();
    int capacidad = instance.getCapacity();
    vector<vector<int>> rutas = clarkewright(clientes, capacidad);

    // Paso 3: cargar la solucion final
    Solution sol;
    int suma_demanda = 0;
    for (const auto& ruta : rutas) {
        sol.agregarRuta(ruta, distancias,suma_demanda);
    }

    return sol;
}

/*
-----------------------------------------------------------
Complejidad del algoritmo Clarke & Wright
-----------------------------------------------------------

Sea n la cantidad total de clientes (excluyendo el depósito).

1. Cálculo de ahorros (savings):
   - Se computa un valor de ahorro para cada par (i, j) de clientes.
   - Esto implica recorrer todos los pares i < j → hay O(n²) combinaciones.

2. Ordenamiento del vector de ahorros:
   - Se ordenan los O(n²) elementos → complejidad O(n² log n).

3. Fusión de rutas:
   - En el peor caso, se intenta unir rutas para cada uno de los O(n²) pares.
   - Cada intento de fusión puede requerir copiar rutas y verificar demanda total, lo cual es O(n) en el peor caso (aunque en práctica es menor).

Por lo tanto, la complejidad temporal total es:

   → Cálculo de savings:        O(n²)  
   → Ordenamiento:              O(n² log n)  
   → Iteración para fusiones:   O(n² × n) = O(n³) en el peor caso

Complejidad temporal = O(n³) en el peor caso.

*/
