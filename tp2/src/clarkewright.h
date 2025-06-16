#ifndef CLARKEWRIGHT_H
#define CLARKEWRIGHT_H
#include "Cliente.h"
#include <vector>

// Declaración de la función Clarke-Wright
std::vector<std::vector<int>> clarkewright(const std::vector<Cliente>& clientes, int capacidad);

// Declaración (opcional) de distancia si no está en otro archivo
double distancia(const Cliente& a, const Cliente& b);

#endif // CLARKEWRIGHT_H