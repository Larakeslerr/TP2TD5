#include <vector>    // Para usar std::vector
#include <cmath>     // Para funciones matemáticas como distancia (si usa sqrt, etc.)
#include <limits>    // Para usar std::numeric_limits<double>::max() si reemplazas 1e9 
#include <vector>
#include "CVRP_Solution.h"
#include "armarrutascortas.h" 
#include "VRPLIBReader.h"
using namespace std;


vector<vector<int>> armarRutasCortas(const vector<Cliente>& clientes,int capacidad,const vector<vector<double>>& distancias) {
    int n = clientes.size();
    vector<bool> visitado(n, false);
    visitado[0] = true;

    vector<vector<int>> rutas;

    while (true) {
        int carga = 0;
        int actual = 0;
        vector<int> ruta;
        ruta.push_back(clientes[0].id);

        while (true) {
            int mejor = -1;
            double distMin = numeric_limits<double>::max();

            for (int i = 1; i < n; i++) {
                if (!visitado[i] && clientes[i].demanda + carga <= capacidad) {
                    double d = distancias[clientes[actual].id][clientes[i].id];
                    if (d < distMin) {
                        distMin = d;
                        mejor = i;
                    }
                }
            }

            if (mejor == -1) break;

            ruta.push_back(clientes[mejor].id);
            carga += clientes[mejor].demanda;
            visitado[mejor] = true;
            actual = mejor;
        }

        ruta.push_back(clientes[0].id);
        rutas.push_back(ruta);

        bool todosVisitados = true;
        for (int i = 1; i < n; i++) {
            if (!visitado[i]) {
                todosVisitados = false;
                break;
            }
        }
        if (todosVisitados) break;
    }

    return rutas;
}

/*
-----------------------------------------------------------
Complejidad del algoritmo armarRutasCortas
-----------------------------------------------------------

Sea n la cantidad total de nodos (incluido el depósito).

- El algoritmo genera rutas de forma iterativa hasta que todos los clientes han sido visitados.
- Por cada ruta:
    - Se ejecuta una búsqueda del cliente no visitado más cercano compatible con la capacidad.
    - Esta búsqueda tiene costo O(n) ya que revisa todos los nodos no visitados.
    - Como una ruta puede tener hasta O(n) clientes en el peor caso, el bucle interno también itera O(n) veces.
- En el peor caso (una ruta por cliente), hay O(n) rutas.

Por lo tanto, la complejidad temporal total en el peor caso es:

    O(n) rutas × O(n) iteraciones por ruta × O(n) comparaciones = O(n³)
*/

Solution solveGreedy(const VRPLIBReader& instance){
    
    vector<Node> nodos = instance.getNodes();
    vector<int> demandas = instance.getDemands();
    vector<Cliente> clientes;

    for (const auto& nodo : nodos) {
        Cliente c;
        c.id = nodo.id;
        c.x = nodo.x;
        c.y = nodo.y;
        c.demanda = demandas[nodo.id];
        clientes.push_back(c);
    }


    const auto& distancias = instance.getDistanceMatrix();
    int capacidad = instance.getCapacity();
    vector<vector<int>> rutas = armarRutasCortas(clientes, capacidad, distancias);


    Solution sol;
    for (const auto& ruta : rutas) {
    int suma_demanda = 0;
    for (int i = 1; i < ruta.size() - 1; ++i) { // excluye el depósito al inicio y fin
        int cliente_id = ruta[i];
        suma_demanda += clientes[cliente_id].demanda;
    }
    sol.agregarRuta(ruta, distancias, suma_demanda);
}


    return sol;
}






