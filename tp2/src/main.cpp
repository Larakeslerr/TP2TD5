#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include "VRPLIBReader.h"
#include "armarRutasCortas.h"
#include "busqueda_local.h"
#include "Cliente.h"
#include "grasp.h"

double calcularCostoTotal(const std::vector<std::vector<int>>& rutas,
                          const std::vector<std::vector<double>>& dist_matrix,
                          const std::vector<Cliente>& clientes) {
    double costoTotal = 0.0;
    for (const auto& ruta : rutas) {
        for (size_t i = 0; i + 1 < ruta.size(); ++i) {
            int from = ruta[i], to = ruta[i + 1];
            auto it_from = std::find_if(clientes.begin(), clientes.end(),
                                        [&](const Cliente& c) { return c.id == from; });
            auto it_to = std::find_if(clientes.begin(), clientes.end(),
                                      [&](const Cliente& c) { return c.id == to; });
            if (it_from != clientes.end() && it_to != clientes.end()) {
                int idx_from = std::distance(clientes.begin(), it_from);
                int idx_to = std::distance(clientes.begin(), it_to);
                costoTotal += dist_matrix[idx_from][idx_to];
            }
        }
    }
    return costoTotal;
}

void imprimirResumen(const std::string& nombre,
                     const std::vector<std::vector<int>>& rutas,
                     const std::vector<std::vector<double>>& dist_matrix,
                     const std::vector<Cliente>& clientes) {
    double costoTotal = calcularCostoTotal(rutas, dist_matrix, clientes);
    std::cout << nombre << " → Rutas: " << rutas.size()
              << " | Costo: " << std::fixed << std::setprecision(3) << costoTotal << "\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_vrp_file>" << std::endl;
        return 1;
    }

    VRPLIBReader reader(argv[1]);

    std::vector<Node> nodes = reader.getNodes();
    std::vector<Cliente> clientes;
    for (const Node& n : nodes) {
        clientes.push_back({n.id, n.x, n.y, n.demanda});
    }

    int depot_id = reader.getDepotId();
    auto it = std::find_if(clientes.begin(), clientes.end(),
                           [&](const Cliente& c) { return c.id == depot_id; });
    if (it != clientes.end()) {
        std::iter_swap(clientes.begin(), it);
    }

    auto dist_matrix = reader.getDistanceMatrix();

    // ✅ Rutas Cortas base
    auto rutas_base = armarRutasCortas(clientes, reader.getCapacity(), dist_matrix);
    imprimirResumen("Rutas Cortas (Base)", rutas_base, dist_matrix, clientes);

    // ✅ GRASP con 15 iteraciones y RCL size 3
    int n_iters = 50;
    int rcl_size = 5;

    Solution mejor_sol = grasp(reader, n_iters, rcl_size);
    auto rutas_grasp = mejor_sol.getRutas();
    imprimirResumen("GRASP Mejor Solución", rutas_grasp, dist_matrix, clientes);

    return 0;
}
