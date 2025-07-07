#include <iostream>
#include <cassert>
#include "CVRP_VRPLIBReader.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <ruta_archivo.vrp>" << std::endl;
        return 1;
    }

    std::string path = argv[1];
    VRPLIBReader reader;
    reader.leerArchivo(path);

    // Verifica que el nombre no esté vacío
    assert(!reader.getNombre().empty());

    // Verifica que la dimensión sea positiva
    assert(reader.getDimension() > 0);

    // Verifica que la capacidad sea positiva
    assert(reader.getCapacidad() > 0);

    // Verifica que el depósito esté definido
    assert(reader.getDeposito() >= 0);

    // Verifica que existan nodos y matriz de distancias
    assert(!reader.getNodos().empty());
    const auto& matriz = reader.getMatrizDistancias();
    assert(!matriz.empty());
    assert(matriz.size() == static_cast<size_t>(reader.getDimension()));

    std::cout << "✅ Test de VRPLIBReader PASÓ correctamente." << std::endl;
    return 0;
}
