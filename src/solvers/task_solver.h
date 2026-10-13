#ifndef TASK_SOLVER_H
#define TASK_SOLVER_H

#include "parallel_solver.h"

class TaskSolver : public ParallelSolver {
public:
    TaskSolver(
        const Graph& graph,
        const Configuration& configuration
    );

    std::vector<Cut> solve() override;
};

#endif