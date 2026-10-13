#ifndef DYNAMIC_SOLVER_H
#define DYNAMIC_SOLVER_H

#include "parallel_solver.h"

class DynamicSolver : public ParallelSolver {
public:
    DynamicSolver(
        const Graph& graph,
        const Configuration& configuration
    );

    std::vector<Cut> solve() override;
};

#endif