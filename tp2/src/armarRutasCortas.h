#ifndef ARMAR_RUTAS_CORTAS_H
#define ARMAR_RUTAS_CORTAS_H
#include "Cliente.h"
#include <vector>

std::vector<std::vector<int>> armarRutasCortas(
    const std::vector<Cliente>& clientes, 
    int capacidad, 
    const std::vector<std::vector<double>>& distancias);



// Declaración (opcional) si distancia no está en otro lado
double distancia(const Cliente& a, const Cliente& b);

#endif // ARMAR_RUTAS_CORTAS_H