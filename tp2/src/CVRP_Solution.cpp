#include "CVRP_Solution.h"
using namespace std;

Solution::Solution() : costoTotal(0.0) {
    // Constructor: inicializa con costo cero y sin rutas
}

void Solution::agregarRuta(const std::vector<int>& ruta, const std::vector<std::vector<double>>& distancias, int suma_demanda) {
    rutas.push_back(ruta);
    demandas.push_back(suma_demanda);
    costoTotal += calcularCostoRuta(ruta, distancias);
}


double Solution::calcularCostoRuta(const std::vector<int>& ruta, const std::vector<std::vector<double>>& distancias) const {
    double total = 0.0;
    for (size_t i = 0; i < ruta.size() - 1; ++i) {
        int from = ruta[i];
        int to = ruta[i + 1];
        total += distancias[from][to];
    }
    return total;
}

void Solution::imprimir() const {
    std::cout << "Rutas:" << std::endl;
    for (size_t i = 0; i < rutas.size(); ++i) {
    std::cout << "Ruta " << i + 1 << ": ";
    for (int nodo : rutas[i]) {
        std::cout << nodo << " ";
    }
    std::cout << "| SUMD = " << demandas[i] << std::endl;
}

    std::cout << "Costo total: " << costoTotal << std::endl;
}

const std::vector<std::vector<int>>& Solution::getRutas() const {
    return rutas;
}

double Solution::getCostoTotal() const {
    return costoTotal;
}
