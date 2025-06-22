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
