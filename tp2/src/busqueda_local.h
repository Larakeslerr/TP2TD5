#ifndef BUSQUEDA_LOCAL_H
#define BUSQUEDA_LOCAL_H

#include <vector>

using namespace std;

vector<vector<int>> BusquedaLocalSwap(
    const vector<vector<int>>& rutas,
    const vector<vector<double>>& distancias,
    const vector<int>& demandas,
    int capacidad
);
vector<vector<int>> busquedaLocal2opt(
    const vector<vector<int>>& rutas,
    const vector<vector<double>>& distancias
);


#endif
