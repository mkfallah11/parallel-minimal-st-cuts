#include "adaptive_solver.h"

#include "dynamic_solver.h"
#include "sections_solver.h"
#include "task_solver.h"
#include "work_queue_solver.h"

#include <algorithm>
#include <cstddef>


AdaptiveSolver::AdaptiveSolver(
    const Graph& graph,
    const Configuration& configuration
)
    : ParallelSolver(
          graph,
          configuration
      )
{
}


std::vector<Cut> AdaptiveSolver::solve()
{
    resetStatistics();

    startTimer();

    const int threadCount =
        std::max(
            1,
            configuration_.getNumThreads()
        );

    const int requestedBranches =
        threadCount * 4;

    const std::vector<Algorithm2Branch> branches =
        createBranches(
            requestedBranches
        );

    const Strategy strategy =
        selectStrategy(
            branches.size(),
            threadCount
        );

    std::vector<Cut> result;

    switch (strategy) {

        case Strategy::Sections:
            result = runSections();
            break;

        case Strategy::Tasks:
            result = runTasks();
            break;

        case Strategy::Dynamic:
            result = runDynamic();
            break;

        case Strategy::WorkQueue:
            result = runWorkQueue();
            break;
    }

    stopTimer();

    return result;
}


AdaptiveSolver::Strategy
AdaptiveSolver::selectStrategy(
    std::size_t branchCount,
    int threadCount
) const
{
    if (threadCount <= 1) {
        return Strategy::Sections;
    }

    if (branchCount <= 4) {
        return Strategy::Sections;
    }

    if (branchCount <=
        static_cast<std::size_t>(
            threadCount * 2
        )) {
        return Strategy::Tasks;
    }

    if (branchCount <=
        static_cast<std::size_t>(
            threadCount * 4
        )) {
        return Strategy::Dynamic;
    }

    return Strategy::WorkQueue;
}


std::vector<Cut>
AdaptiveSolver::runSections()
{
    SectionsSolver solver(
        graph_,
        configuration_
    );

    const std::vector<Cut> result =
        solver.solve();

    statistics_ =
        solver.getStatistics();

    return result;
}


std::vector<Cut>
AdaptiveSolver::runTasks()
{
    TaskSolver solver(
        graph_,
        configuration_
    );

    const std::vector<Cut> result =
        solver.solve();

    statistics_ =
        solver.getStatistics();

    return result;
}


std::vector<Cut>
AdaptiveSolver::runDynamic()
{
    DynamicSolver solver(
        graph_,
        configuration_
    );

    const std::vector<Cut> result =
        solver.solve();

    statistics_ =
        solver.getStatistics();

    return result;
}


std::vector<Cut>
AdaptiveSolver::runWorkQueue()
{
    WorkQueueSolver solver(
        graph_,
        configuration_
    );

    const std::vector<Cut> result =
        solver.solve();

    statistics_ =
        solver.getStatistics();

    return result;
}