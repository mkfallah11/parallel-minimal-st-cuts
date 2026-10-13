#ifndef SEQUENTIAL_SOLVER_H
#define SEQUENTIAL_SOLVER_H

#include "parallel_solver.h"

class SequentialSolver : public ParallelSolver {
public:
    SequentialSolver(
        const Graph& graph,
        const Configuration& configuration
    );

    std::vector<Cut> solve() override;
};

#endif