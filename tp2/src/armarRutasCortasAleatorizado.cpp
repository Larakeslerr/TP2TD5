#include "armarRutasCortasAleatorizado.h"
#include <limits>
#include <random>
#include <algorithm>

std::vector<std::vector<int>> armarRutasCortasAleatorizado(
    const std::vector<Cliente>& clientes,
    int capacidad,
    const std::vector<std::vector<double>>& distancias,
    int rcl_size) {

    int n = clientes.size();
    std::vector<bool> visitado(n, false);
    visitado[0] = true;
    std::vector<std::vector<int>> rutas;

    std::random_device rd;
    std::mt19937 gen(rd());

    while (true) {
        int carga = 0;
        int actual = 0;
        std::vector<int> ruta;
        ruta.push_back(clientes[0].id);

        while (true) {
            std::vector<std::pair<int, double>> candidatos;
            for (int i = 1; i < n; ++i) {
                if (!visitado[i] && clientes[i].demanda + carga <= capacidad) {
                    double d = distancias[clientes[actual].id][clientes[i].id];
                    candidatos.emplace_back(i, d);
                }
            }

            if (candidatos.empty()) break;

            std::sort(candidatos.begin(), candidatos.end(),
                      [](const auto& a, const auto& b) { return a.second < b.second; });

            int limite = std::min(rcl_size, static_cast<int>(candidatos.size()));
            std::uniform_int_distribution<> distrib(0, limite - 1);
            int elegido = candidatos[distrib(gen)].first;

            ruta.push_back(clientes[elegido].id);
            carga += clientes[elegido].demanda;
            visitado[elegido] = true;
            actual = elegido;
        }

        ruta.push_back(clientes[0].id);
        rutas.push_back(ruta);

        if (std::all_of(visitado.begin() + 1, visitado.end(), [](bool v) { return v; }))
            break;
    }

    return rutas;
}

/*
-----------------------------------------------------------
Complejidad del algoritmo armarRutasCortasAleatorizado
-----------------------------------------------------------

Parámetros:
- n: cantidad total de nodos (clientes + depósito)
- rcl_size: tamaño de la lista RCL (aleatoriedad controlada)

Análisis:

- El algoritmo recorre los clientes hasta que todos fueron visitados.
- En cada iteración externa (por cada ruta generada), se construye una ruta válida:
    - Se evalúan hasta O(n) clientes no visitados.
    - Se calcula la distancia y se arma un vector de candidatos: O(n)
    - Luego se ordena ese vector (para tomar los mejores): O(n log n)
    - Se elige un candidato aleatoriamente dentro del top `rcl_size`: O(1)

En el peor caso, hay hasta O(n) rutas (una por cliente), por lo que:

    → Costo total = O(n) rutas × O(n log n) cada una = **O(n² log n)**

Este algoritmo es más eficiente que la versión determinista (greedy pura, O(n³)), 
ya que evita la comparación exhaustiva y privilegia un subconjunto de buenas opciones.

-----------------------------------------------------------
*/

