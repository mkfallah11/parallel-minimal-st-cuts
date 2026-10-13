#ifndef ADAPTIVE_SOLVER_H
#define ADAPTIVE_SOLVER_H

#include "parallel_solver.h"

class AdaptiveSolver : public ParallelSolver {
public:
    AdaptiveSolver(
        const Graph& graph,
        const Configuration& configuration
    );

    std::vector<Cut> solve() override;

private:
    enum class Strategy {
        Sections,
        Tasks,
        Dynamic,
        WorkQueue
    };

    Strategy selectStrategy(
        std::size_t branchCount,
        int threadCount
    ) const;

    std::vector<Cut> runSections();

    std::vector<Cut> runTasks();

    std::vector<Cut> runDynamic();

    std::vector<Cut> runWorkQueue();
};

#endif