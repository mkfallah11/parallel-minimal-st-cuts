#ifndef WORK_QUEUE_SOLVER_H
#define WORK_QUEUE_SOLVER_H

#include "parallel_solver.h"

#include <cstddef>
#include <deque>
#include <mutex>


class WorkQueueSolver : public ParallelSolver {
public:
    WorkQueueSolver(
        const Graph& graph,
        const Configuration& configuration
    );

    std::vector<Cut> solve() override;


private:
    struct WorkItem {
        std::size_t branchIndex;
    };


    bool popWork(
        std::deque<WorkItem>& queue,
        WorkItem& item,
        std::mutex& mutex
    ) const;
};

#endif