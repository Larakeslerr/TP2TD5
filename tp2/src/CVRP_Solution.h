#ifndef SOLUTION_H
#define SOLUTION_H

#include <vector>
#include <iostream>

class Solution {
private:
    std::vector<std::vector<int>> rutas; // Cada ruta empieza y termina en el depósito
    double costoTotal; // Se va actualizando a medida que se agregan rutas
    std::vector<int> demandas;  // suma de demandas por ruta


public:
    Solution();

    // Agrega una ruta a la solución y suma su costo al total
    void agregarRuta(const std::vector<int>& ruta, const std::vector<std::vector<double>>& distancias, int suma_demanda);

    // Calcula el costo de una sola ruta
    double calcularCostoRuta(const std::vector<int>& ruta, const std::vector<std::vector<double>>& distancias) const;

    // Imprime todas las rutas y el costo total
    void imprimir() const;

    // Getters
    const std::vector<std::vector<int>>& getRutas() const;
    double getCostoTotal() const;
};

#endif // SOLUTION_H
