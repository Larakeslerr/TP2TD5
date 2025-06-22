#include <cassert>
#include "VRPLIBReader.h"
#include <iostream>

void testVRPLIBReader(const std::string& path) {
    VRPLIBReader reader(path);

    std::cout << "→ Nombre leído: '" << reader.getName() << "'" << std::endl;

    // Solo comprobamos que tenga algún nombre y que los campos sean razonables
    assert(!reader.getName().empty());
    assert(reader.getDimension() > 0);
    assert(reader.getCapacity() > 0);
    assert(reader.getDepotId() >= 1);

    const auto& nodes = reader.getNodes();
    assert(!nodes.empty());

    const auto& dist = reader.getDistanceMatrix();
    assert(dist.size() == reader.getDimension() + 1);
    assert(dist[1][1] == 0.0); // Distancia a sí mismo
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <archivo_vrp>" << std::endl;
        return 1;
    }

    testVRPLIBReader(argv[1]);
    std::cout << "✅ testVRPLIBReader pasó correctamente.\n";
    return 0;
}
