#include <iostream>
#include <cassert>
#include "VRPLIBReader.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <ruta_archivo.dat>" << std::endl;
        return 1;
    }

    std::string path = argv[1];
    
    // Usá el constructor que LEE TODO
    VRPLIBReader reader(path);

    assert(!reader.getName().empty());
    assert(reader.getDimension() > 0);
    assert(reader.getCapacity() > 0);
    assert(reader.getDepotId() >= 0);

    const auto& nodos = reader.getNodes();
    assert(!nodos.empty());

    const auto& matriz = reader.getDistanceMatrix();
    assert(!matriz.empty());
    assert(matriz.size() == static_cast<size_t>(reader.getDimension()) + 1);

    std::cout << "✅ Test de VRPLIBReader PASÓ correctamente." << std::endl;
    return 0;
}
