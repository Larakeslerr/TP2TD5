#include <iostream>
#include <vector>
#include <cassert>
#include "CVRP_Solution.h"

int main() {
    // Matriz de distancias simulada (4 nodos: 0, 1, 2, 3)
    std::vector<std::vector<double>> distancias = {
        {0, 2, 3, 4},
        {2, 0, 1, 5},
        {3, 1, 0, 2},
        {4, 5, 2, 0}
    };

    // Crear una ruta válida (ida y vuelta desde el depósito)
    std::vector<int> ruta = {0, 2, 3, 0};
    int demanda = 30;

    // Crear solución y agregar la ruta
    Solution sol;
    sol.agregarRuta(ruta, distancias, demanda);

    // Verificar que la ruta fue agregada correctamente
    const auto& rutas = sol.getRutas();
    assert(rutas.size() == 1);
    assert(rutas[0] == ruta);

    // Verificar el costo calculado
    double esperado = distancias[0][2] + distancias[2][3] + distancias[3][0];
    double obtenido = sol.getCostoTotal();
    std::cout << "Costo esperado: " << esperado << ", obtenido: " << obtenido << std::endl;
    assert(std::abs(obtenido - esperado) < 1e-6);

    std::cout << "✅ Test de Solution pasó correctamente.\n";
    return 0;
}
