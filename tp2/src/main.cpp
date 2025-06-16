#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>  // para setprecision
#include "VRPLIBReader.h"
#include "armarRutasCortas.h"
#include "busqueda_local.h"
#include "clarkewright.h"
#include "CVRP_Solution.h"


// Función para calcular costo total dado conjunto de rutas y matriz de distancias
double calcularCostoTotal(const std::vector<std::vector<int>>& rutas,
                         const std::vector<std::vector<double>>& dist_matrix,
                         const std::vector<Cliente>& clientes) {
    double costoTotal = 0.0;

    for (const auto& ruta : rutas) {
        if (ruta.empty()) continue;

        // Sumo distancia entre nodos consecutivos de la ruta (incluye depósito)
        for (size_t i = 0; i + 1 < ruta.size(); ++i) {
            // Buscar índice de clientes en vector clientes
            int from = ruta[i];
            int to = ruta[i + 1];

            auto it_from = std::find_if(clientes.begin(), clientes.end(), [&](const Cliente& c){ return c.id == from; });
            auto it_to = std::find_if(clientes.begin(), clientes.end(), [&](const Cliente& c){ return c.id == to; });

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


    std::cout << "Instance Name: " << reader.getName() << std::endl;
    std::cout << "Dimension: " << reader.getDimension() << std::endl;
    std::cout << "Number of Vehicles: " << reader.getNumVehicles() << std::endl;
    std::cout << "Capacity: " << reader.getCapacity() << std::endl;
    std::cout << "Depot ID: " << reader.getDepotId() << std::endl;

    std::vector<Node> clients = reader.getNodes();
    std::cout << "Cantidad de nodos: " << clients.size() << std::endl;

    // Matriz de distancias
    std::vector<std::vector<double>> dist_matrix = reader.getDistanceMatrix();

    // Convertir nodos a clientes
    std::vector<Cliente> clientes;
    for (const Node& n : clients) {
        clientes.push_back({n.id, n.x, n.y, n.demanda});
    }
    // 🔍 DEBUG: verificación de demandas
    for (const Cliente& c : clientes) {
     std::cout << "Cliente " << c.id << " demanda: " << c.demanda << std::endl;
    }
    // Mover depósito al índice 0
    int depot_id = reader.getDepotId();
    auto it = std::find_if(clientes.begin(), clientes.end(), [&](const Cliente& c) {
        return c.id == depot_id;
    });
    if (it != clientes.end()) {
        std::iter_swap(clientes.begin(), it);
    }

    // Ejecutar Clarke-Wright
    // Ejecutar heurística constructiva (Clarke & Wright)
    auto rutas_cw = clarkewright(clientes, reader.getCapacity());
    double costo_cw = calcularCostoTotal(rutas_cw, dist_matrix, clientes);

    // Aplicar búsqueda local (2-opt o swap)
    std::cout << "Aplicando búsqueda local 2-opt..." << std::endl;
    auto rutas_opt = busquedaLocalSwap(rutas_cw, dist_matrix);
    double costo_opt = calcularCostoTotal(rutas_opt, dist_matrix, clientes);

    // Mostrar comparación
    std::cout << "Costo original (CW): " << costo_cw << std::endl;
    std::cout << "Costo mejorado (2-opt): " << costo_opt << std::endl;

    // Usar las rutas mejoradas para el resto del programa
    std::vector<std::vector<int>> rutas;
    if (costo_opt < costo_cw) {
    std::cout << "✅ Se mejoró el costo con 2-opt." << std::endl;
    rutas = rutas_opt;
    } else {
    std::cout << "⚠️  2-opt no mejoró el costo. Se mantiene solución original." << std::endl;
    rutas = rutas_cw;
    }


    std::cout << "DEBUG: Se generaron " << rutas.size() << " rutas." << std::endl;

    // Ejecutar heurística Greedy
    // ---------------------------
    Solution sol_greedy = solveGreedy(reader);
    auto rutas_greedy = sol_greedy.getRutas();
    double costo_greedy = calcularCostoTotal(rutas_greedy, dist_matrix, clientes);

    // Aplicar 2-opt a greedy
    std::cout << "\nAplicando 2-opt a solución Greedy..." << std::endl;
    auto rutas_greedy_opt = busquedaLocalSwap(rutas_greedy, dist_matrix);
    double costo_greedy_opt = calcularCostoTotal(rutas_greedy_opt, dist_matrix, clientes);

    // Comparación
    std::cout << "\nResumen de heurísticas:\n";
    std::cout << "Costo CW              : " << costo_cw << std::endl;
    std::cout << "Costo CW + 2-opt      : " << costo_opt << std::endl;
    std::cout << "Costo Greedy          : " << costo_greedy << std::endl;
    std::cout << "Costo Greedy + 2-opt  : " << costo_greedy_opt << std::endl;


    // Preparar vectores para demandas y costos por ruta
    std::vector<int> demandasPorRuta;
    std::vector<double> costosPorRuta;

    for (const auto& ruta : rutas) {
        int demandaRuta = 0; 
        // Calcular demanda sólo de clientes (excluyendo depósito)
        for (size_t j = 0; j < ruta.size(); ++j) {
         int id_cliente = ruta[j];
         if (id_cliente == depot_id) continue; // saltear depósito
         auto it_c = std::find_if(clientes.begin(), clientes.end(), [&](const Cliente& c) { return c.id == id_cliente; });
         if (it_c != clientes.end()) demandaRuta += it_c->demanda;
        }

        demandasPorRuta.push_back(demandaRuta);
        // Calcular costo ruta sumando distancias entre nodos consecutivos
        double costoRuta = 0.0;
        for (size_t j = 0; j + 1 < ruta.size(); ++j) {
            int from = ruta[j];
            int to = ruta[j + 1];

            auto it_from = std::find_if(clientes.begin(), clientes.end(), [&](const Cliente& c){ return c.id == from; });
            auto it_to = std::find_if(clientes.begin(), clientes.end(), [&](const Cliente& c){ return c.id == to; });

            if (it_from != clientes.end() && it_to != clientes.end()) {
                int idx_from = std::distance(clientes.begin(), it_from);
                int idx_to = std::distance(clientes.begin(), it_to);
                costoRuta += dist_matrix[idx_from][idx_to];
            }
        }
        costosPorRuta.push_back(costoRuta);
    }

    double costoTotal = calcularCostoTotal(rutas, dist_matrix, clientes);

    // Imprimir resultado en formato esperado
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "\nNAME    : " << reader.getName() << std::endl;
    std::cout << "COMMENT : FISHER,1994" << std::endl;
    std::cout << "TYPE    : HREAL" << std::endl;
    std::cout << "ROUTES  : " << rutas.size() << std::endl;
    std::cout << "COST    : " << costoTotal << std::endl;
    std::cout << "SOLUTION_SECTION" << std::endl;
    std::cout << " #R   SUMD        COST      LENGTH   #C   SEQUENCE" << std::endl;

    for (size_t i = 0; i < rutas.size(); ++i) {
        int cantClientes = static_cast<int>(rutas[i].size()) - 2;  // excluyendo depósitos

        std::cout << std::setw(3) << i + 1 << "   "
                  << std::setw(4) << demandasPorRuta[i] << "    "
                  << std::setprecision(4) << std::setw(8) << costosPorRuta[i] << "    "
                  << std::setw(8) << costosPorRuta[i] << "   "
                  << std::setw(3) << cantClientes << "   ";

        // Imprimir sólo clientes, sin depósito (índices 1 a size-2)
        for (size_t j = 1; j + 1 < rutas[i].size(); ++j) {
            std::cout << rutas[i][j] << " ";
        }
        std::cout << std::endl;
    }

    std::cout << "DEPOT_SECTION" << std::endl;
    std::cout << depot_id << std::endl;  // Imprimir id real depósito
    std::cout << "END" << std::endl;

    return 0;
}
