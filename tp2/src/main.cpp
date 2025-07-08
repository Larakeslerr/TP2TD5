#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <chrono>
#include <fstream>

#include "VRPLIBReader.h"
#include "clarkewright.h"
#include "armarRutasCortas.h"
#include "armarRutasCortasAleatorizado.h"
#include "busqueda_local.h"
#include "Cliente.h"
#include "grasp.h"

using namespace std;
using namespace std::chrono;

void exportarRutas(const string& nombreArchivo,
                   const vector<vector<int>>& rutas,
                   const vector<Cliente>& clientes) {
    ofstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cerr << "No se pudo abrir el archivo de salida.\n";
        return;
    }

    for (size_t i = 0; i < rutas.size(); ++i) {
        archivo << "Ruta " << i + 1 << ":\n";
        for (int id : rutas[i]) {
            auto it = find_if(clientes.begin(), clientes.end(),
                              [&](const Cliente& c) { return c.id == id; });
            if (it != clientes.end()) {
                archivo << id << " " << it->x << " " << it->y << "\n";
            }
        }
        archivo << "\n";
    }

    archivo.close();
}

double calcularCostoTotal(const vector<vector<int>>& rutas,
                          const vector<vector<double>>& dist_matrix,
                          const vector<Cliente>& clientes) {
    double costoTotal = 0.0;
    for (const auto& ruta : rutas) {
        for (size_t i = 0; i + 1 < ruta.size(); ++i) {
            int from = ruta[i], to = ruta[i + 1];
            auto it_from = find_if(clientes.begin(), clientes.end(), [&](const Cliente& c) { return c.id == from; });
            auto it_to = find_if(clientes.begin(), clientes.end(), [&](const Cliente& c) { return c.id == to; });
            if (it_from != clientes.end() && it_to != clientes.end()) {
                int idx_from = distance(clientes.begin(), it_from);
                int idx_to = distance(clientes.begin(), it_to);
                costoTotal += dist_matrix[idx_from][idx_to];
            }
        }
    }
    return costoTotal;
}

void imprimirResumen(const string& nombre,
                     const vector<vector<int>>& rutas,
                     const vector<vector<double>>& dist_matrix,
                     const vector<Cliente>& clientes,
                     double tiempo_ms) {
    double costo = calcularCostoTotal(rutas, dist_matrix, clientes);
    cout << fixed << setprecision(3);
    cout << nombre << " | Rutas: " << rutas.size()
         << " | Costo: " << costo
         << " | Tiempo: " << tiempo_ms << " ms\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <path_to_vrp_file>\n";
        return 1;
    }

    VRPLIBReader reader(argv[1]);

    vector<Node> nodes = reader.getNodes();
    vector<Cliente> clientes;
    for (const Node& n : nodes) {
        clientes.push_back({n.id, n.x, n.y, n.demanda});
    }

    int depot_id = reader.getDepotId();
    auto it = find_if(clientes.begin(), clientes.end(), [&](const Cliente& c) { return c.id == depot_id; });
    if (it != clientes.end()) iter_swap(clientes.begin(), it);

    auto dist_matrix = reader.getDistanceMatrix();

    // Clarke-Wright base
    auto t1 = high_resolution_clock::now();
    auto rutas_cw = clarkewright(clientes, reader.getCapacity());
    auto t2 = high_resolution_clock::now();
    imprimirResumen("Clarke-Wright", rutas_cw, dist_matrix, clientes,
                    duration<double, milli>(t2 - t1).count());

    // Clarke-Wright + 2-opt
    t1 = high_resolution_clock::now();
    auto rutas_cw_2opt = busquedaLocal2opt(rutas_cw, dist_matrix);
    t2 = high_resolution_clock::now();
    imprimirResumen("Clarke-Wright + 2-opt", rutas_cw_2opt, dist_matrix, clientes,
                    duration<double, milli>(t2 - t1).count());

    // Rutas Cortas base
    t1 = high_resolution_clock::now();
    auto rutas_cortas = armarRutasCortas(clientes, reader.getCapacity(), dist_matrix);
    t2 = high_resolution_clock::now();
    imprimirResumen("Rutas Cortas", rutas_cortas, dist_matrix, clientes,
                    duration<double, milli>(t2 - t1).count());

    // Rutas Cortas + Swap
    t1 = high_resolution_clock::now();
    auto rutas_cortas_swap = busquedaLocalSwap(rutas_cortas, dist_matrix);
    t2 = high_resolution_clock::now();
    imprimirResumen("Rutas Cortas + Swap", rutas_cortas_swap, dist_matrix, clientes,
                    duration<double, milli>(t2 - t1).count());

    // Rutas Cortas + VND (Swap + 2-opt)
    t1 = high_resolution_clock::now();
    auto rutas_vnd = rutas_cortas;
    bool mejoro = true;
    while (mejoro) {
        mejoro = false;
        auto swap = busquedaLocalSwap(rutas_vnd, dist_matrix);
        if (calcularCostoTotal(swap, dist_matrix, clientes) < calcularCostoTotal(rutas_vnd, dist_matrix, clientes)) {
            rutas_vnd = swap;
            mejoro = true;
        }
        auto opt = busquedaLocal2opt(rutas_vnd, dist_matrix);
        if (calcularCostoTotal(opt, dist_matrix, clientes) < calcularCostoTotal(rutas_vnd, dist_matrix, clientes)) {
            rutas_vnd = opt;
            mejoro = true;
        }
    }
    t2 = high_resolution_clock::now();
    imprimirResumen("Rutas Cortas + VND (Swap + 2-opt)", rutas_vnd, dist_matrix, clientes,
                    duration<double, milli>(t2 - t1).count());

    // GRASP
    t1 = high_resolution_clock::now();
    int n_iters = 15;
    int rcl_size = 3;
    Solution sol_grasp = grasp(reader, n_iters, rcl_size);
    auto rutas_grasp = sol_grasp.getRutas();
    t2 = high_resolution_clock::now();
    imprimirResumen("GRASP", rutas_grasp, dist_matrix, clientes,
                    duration<double, milli>(t2 - t1).count());

                    
    exportarRutas("rutas_cw.txt", rutas_cw, clientes);
    exportarRutas("rutas_cw_2opt.txt", rutas_cw_2opt, clientes);
    exportarRutas("rutas_cortas.txt", rutas_cortas, clientes);
    exportarRutas("rutas_cortas_swap.txt", rutas_cortas_swap, clientes);
    exportarRutas("rutas_vnd.txt", rutas_vnd, clientes);
    exportarRutas("rutas_grasp.txt", rutas_grasp, clientes);

    return 0;
}
