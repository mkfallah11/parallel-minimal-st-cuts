#ifndef SECTIONS_SOLVER_H
#define SECTIONS_SOLVER_H

#include "parallel_solver.h"

class SectionsSolver : public ParallelSolver {
public:
    SectionsSolver(
        const Graph& graph,
        const Configuration& configuration
    );

    std::vector<Cut> solve() override;
};

#endif