#include <iostream>
#include <vector>
#include <cassert>
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
    int demanda = 30;

    // 3) Crear solución y agregar la ruta
    Solution sol;
    sol.agregarRuta(ruta, distancias, demanda);

    // 4) Verificar que la ruta fue agregada correctamente
    const auto& rutas = sol.getRutas();
    assert(rutas.size() == 1);
    assert(rutas[0].size() == ruta.size());
    for (size_t i = 0; i < ruta.size(); ++i) {
        assert(rutas[0][i] == ruta[i]);
    }

    // 5) Verificar el costo total calculado
    double costo_esperado = distancias[0][2] + distancias[2][3] + distancias[3][0];
    assert(std::abs(sol.getCostoTotal() - costo_esperado) < 1e-6);

    std::cout << "✅ Test de Solution PASÓ correctamente." << std::endl;
    return 0;
}
