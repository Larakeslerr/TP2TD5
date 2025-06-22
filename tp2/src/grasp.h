#ifndef GRASP_H
#define GRASP_H

#include "VRPLIBReader.h"
#include "CVRP_Solution.h"

// Ejecuta la metaheurística GRASP con una cantidad de iteraciones y tamaño de RCL
Solution grasp(const VRPLIBReader& reader, int n_iters, int rcl_size);

#endif // GRASP_H
