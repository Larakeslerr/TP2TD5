#ifndef ARMAR_RUTAS_CORTAS_ALEATORIZADO_H
#define ARMAR_RUTAS_CORTAS_ALEATORIZADO_H

#include "Cliente.h"
#include <vector>

// Similar a armarRutasCortas, pero con aleatoriedad controlada por RCL
std::vector<std::vector<int>> armarRutasCortasAleatorizado(
    const std::vector<Cliente>& clientes,
    int capacidad,
    const std::vector<std::vector<double>>& distancias,
    int rcl_size);

#endif
