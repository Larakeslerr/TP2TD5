#include <iostream>
#include <vector>
#include <cassert>
#include <map>
#include "CVRP_Solution.h"

int main() {
    // 1) Matriz de distancias simulada (4 nodos: 0, 1, 2, 3)
    std::vector<std::vector<double>> distancias = {
        {0, 2, 3, 4},
        {2, 0, 1, 5},
        {3, 1, 0, 2},
        {4, 5, 2, 0}
    };

    // 2) Ruta válida (ida y vuelta desde el depósito)
    std::vector<int> ruta = {0, 2, 3, 0};
    int suma_demanda_ruta = 30;

    // 3) Crear solución y agregar la ruta
    Solution sol;
    sol.agregarRuta(ruta, distancias, suma_demanda_ruta);

    // 4) Verificar estructura de la ruta
    const auto& rutas = sol.getRutas();
    assert(rutas.size() == 1);
    assert(rutas[0].size() == ruta.size());
    for (size_t i = 0; i < ruta.size(); ++i) {
        assert(rutas[0][i] == ruta[i]);
    }

    // 5) Verificar el costo total calculado
    double costo_esperado = distancias[0][2] + distancias[2][3] + distancias[3][0];
    assert(std::abs(sol.getCostoTotal() - costo_esperado) < 1e-6);

    // 6) Verificar factibilidad por capacidad (sin acceder a Solution::demandas)
    std::map<int, int> demanda_por_cliente = {
        {0, 0},  // depósito
        {1, 10},
        {2, 15},
        {3, 15}
    };
    int capacidad_vehiculo = 40;

    for (const auto& r : rutas) {
        int suma = 0;
        for (int nodo : r) {
            if (nodo != 0) suma += demanda_por_cliente[nodo];
        }
        assert(suma <= capacidad_vehiculo);
    }

    std::cout << "✅ Test de Solution pasó correctamente." << std::endl;
    return 0;
}
