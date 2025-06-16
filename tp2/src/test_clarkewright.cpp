#include <iostream>
#include <vector>
#include "clarkewright.h"

bool verificarRutas(const std::vector<std::vector<int>>& rutas) {
    // Aquí podés poner condiciones esperadas para la solución,
    // Ejemplo: verificar que no haya rutas vacías, que nodos estén incluidos, etc.
    if (rutas.empty()) return false;
    // Agregar más chequeos según caso
    return true;
}

int main() {
    // Creamos clientes manualmente (id, x, y, demanda)
    std::vector<Cliente> clientes = {
        {0, 0.0, 0.0, 0},   // depósito
        {1, 1.0, 1.0, 2},
        {2, 2.0, 2.0, 2},
        {3, 3.0, 3.0, 2},
    };
    int capacidad = 4;

    auto rutas = clarkewright(clientes, capacidad);

    if (verificarRutas(rutas)) {
        std::cout << "Test pasó: rutas generadas correctamente\n";
    } else {
        std::cout << "Test falló: rutas incorrectas\n";
    }

    // Mostrar rutas
    for (const auto& ruta : rutas) {
        for (int id : ruta) {
            std::cout << id << " ";
        }
        std::cout << std::endl;
    }
    return 0;
}
