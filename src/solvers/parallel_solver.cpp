#include "parallel_solver.h"


ParallelSolver::ParallelSolver(
    const Graph& graph,
    const Configuration& configuration
)
    : graph_(graph),
      configuration_(configuration),
      statistics_(),
      algorithm_(graph),
      startTime_() {
}


const Statistics& ParallelSolver::getStatistics() const {
    return statistics_;
}


std::vector<Algorithm2Branch> ParallelSolver::createBranches(
    int branchCount
) const {
    return algorithm_.createBranches(
        branchCount
    );
}


std::vector<Cut> ParallelSolver::executeBranch(
    Algorithm2Branch& branch
) const {
    return algorithm_.continueFromBranch(
        branch
    );
}


void ParallelSolver::resetStatistics() {
    statistics_.reset();
}


void ParallelSolver::startTimer() {
    startTime_ =
        std::chrono::steady_clock::now();
}


void ParallelSolver::stopTimer() {
    const auto endTime =
        std::chrono::steady_clock::now();


    const std::chrono::duration<double, std::milli>
        elapsed =
            endTime - startTime_;


    statistics_.setElapsedMilliseconds(
        elapsed.count()
    );
}


void ParallelSolver::updateStatistics(
    std::size_t cutCount
) {
    statistics_.incrementMinimalCuts(
        cutCount
    );
}