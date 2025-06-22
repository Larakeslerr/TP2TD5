#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include "VRPLIBReader.h"
#include "armarRutasCortas.h"
#include "busqueda_local.h"
#include "clarkewright.h"
#include "CVRP_Solution.h"
#include "grasp.h"

double calcularCostoTotal(const std::vector<std::vector<int>>& rutas,
                          const std::vector<std::vector<double>>& dist_matrix,
                          const std::vector<Cliente>& clientes) {
    double costoTotal = 0.0;
    for (const auto& ruta : rutas) {
        for (size_t i = 0; i + 1 < ruta.size(); ++i) {
            int from = ruta[i], to = ruta[i + 1];
            auto it_from = std::find_if(clientes.begin(), clientes.end(), [&](const Cliente& c) { return c.id == from; });
            auto it_to = std::find_if(clientes.begin(), clientes.end(), [&](const Cliente& c) { return c.id == to; });
            if (it_from != clientes.end() && it_to != clientes.end()) {
                int idx_from = std::distance(clientes.begin(), it_from);
                int idx_to = std::distance(clientes.begin(), it_to);
                costoTotal += dist_matrix[idx_from][idx_to];
            }
        }
    }
    return costoTotal;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_vrp_file>" << std::endl;
        return 1;
    }

    VRPLIBReader reader(argv[1]);
    std::cout << "Nombre instancia: " << reader.getName() << std::endl;

    std::vector<Node> clients = reader.getNodes();
    std::vector<Cliente> clientes;
    for (const Node& n : clients) {
        clientes.push_back({n.id, n.x, n.y, n.demanda});
    }

    int depot_id = reader.getDepotId();
    auto it = std::find_if(clientes.begin(), clientes.end(), [&](const Cliente& c) {
        return c.id == depot_id;
    });
    if (it != clientes.end()) {
        std::iter_swap(clientes.begin(), it);
    }

    auto dist_matrix = reader.getDistanceMatrix();

    // Clarke-Wright + 2-opt
    auto rutas_cw = clarkewright(clientes, reader.getCapacity());
    double costo_cw = calcularCostoTotal(rutas_cw, dist_matrix, clientes);
    auto rutas_opt = busquedaLocalSwap(rutas_cw, dist_matrix);
    double costo_opt = calcularCostoTotal(rutas_opt, dist_matrix, clientes);
    std::cout << "\nClarke-Wright\n";
    std::cout << "Costo original (CW): " << costo_cw << "\n";
    std::cout << "Costo mejorado (2-opt): " << costo_opt << "\n";

    // Greedy + 2-opt
    Solution sol_greedy = solveGreedy(reader);
    auto rutas_greedy = sol_greedy.getRutas();
    double costo_greedy = calcularCostoTotal(rutas_greedy, dist_matrix, clientes);
    auto rutas_greedy_opt = busquedaLocalSwap(rutas_greedy, dist_matrix);
    double costo_greedy_opt = calcularCostoTotal(rutas_greedy_opt, dist_matrix, clientes);
    std::cout << "\nGreedy\n";
    std::cout << "Costo Greedy: " << costo_greedy << "\n";
    std::cout << "Costo Greedy + 2-opt: " << costo_greedy_opt << "\n";

    // GRASP
    int n_iters = 50;
    int rcl_size = 3;
    Solution sol_grasp = grasp(reader, n_iters, rcl_size);
    auto rutas_grasp = sol_grasp.getRutas();
    double costo_grasp = calcularCostoTotal(rutas_grasp, dist_matrix, clientes);
    std::cout << "\n➡️ Resultado GRASP\n";
    std::cout << "Costo GRASP: " << costo_grasp << "\n";

    // Imprimir detalle de rutas GRASP
    std::vector<int> demandasGrasp;
    std::vector<double> costosGrasp;

    for (const auto& ruta : rutas_grasp) {
        int demandaRuta = 0;
        for (int id : ruta) {
            if (id != depot_id) {
                auto it_c = std::find_if(clientes.begin(), clientes.end(), [&](const Cliente& c) { return c.id == id; });
                if (it_c != clientes.end()) demandaRuta += it_c->demanda;
            }
        }
        demandasGrasp.push_back(demandaRuta);

        double costoRuta = 0.0;
        for (size_t i = 0; i + 1 < ruta.size(); ++i) {
            int from = ruta[i], to = ruta[i + 1];
            auto it_from = std::find_if(clientes.begin(), clientes.end(), [&](const Cliente& c) { return c.id == from; });
            auto it_to = std::find_if(clientes.begin(), clientes.end(), [&](const Cliente& c) { return c.id == to; });
            if (it_from != clientes.end() && it_to != clientes.end()) {
                int idx_from = std::distance(clientes.begin(), it_from);
                int idx_to = std::distance(clientes.begin(), it_to);
                costoRuta += dist_matrix[idx_from][idx_to];
            }
        }
        costosGrasp.push_back(costoRuta);
    }

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "\nSOLUTION_SECTION (GRASP)\n";
    std::cout << " #R   SUMD        COST      LENGTH   #C   SEQUENCE\n";

    for (size_t i = 0; i < rutas_grasp.size(); ++i) {
        int cantClientes = static_cast<int>(rutas_grasp[i].size()) - 2;
        std::cout << std::setw(3) << i + 1 << "   "
                  << std::setw(4) << demandasGrasp[i] << "    "
                  << std::setprecision(4) << std::setw(8) << costosGrasp[i] << "    "
                  << std::setw(8) << costosGrasp[i] << "   "
                  << std::setw(3) << cantClientes << "   ";
        for (size_t j = 1; j + 1 < rutas_grasp[i].size(); ++j) {
            std::cout << rutas_grasp[i][j] << " ";
        }
        std::cout << "\n";
    }

    std::cout << "DEPOT_SECTION\n" << depot_id << "\nEND\n";

    return 0;
}
