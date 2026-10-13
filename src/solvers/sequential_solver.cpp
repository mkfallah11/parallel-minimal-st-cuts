#include "sequential_solver.h"

#include <algorithm>
#include <vector>

namespace
{
    Configuration makeSequentialConfiguration(
        const Configuration& configuration)
    {
        Configuration config = configuration;
        config.setNumThreads(1);
        return config;
    }
}

SequentialSolver::SequentialSolver(
    const Graph& graph,
    const Configuration& configuration)
    : ParallelSolver(
          graph,
          makeSequentialConfiguration(configuration))
{
}


std::vector<Cut> SequentialSolver::solve() {
    resetStatistics();
    
    startTimer();

    const int branchCount = 1;

    std::vector<Algorithm2Branch> branches =
        createBranches(branchCount);

    if (branches.empty()) {
        stopTimer();
        return {};
    }

    std::vector<std::vector<Cut>> results(branches.size());

    for (std::size_t i = 0; i < branches.size(); ++i) {
        results[i] = executeBranch(branches[i]);
    }

    /*
     * aggregate branch statistics
     */
    for (const auto& branch : branches) {
        statistics_.incrementCandidates(
            branch.statistics.candidates);

        statistics_.incrementMinimalCuts(
            branch.statistics.minimalCuts);

        statistics_.incrementConnectivityTests(
            branch.statistics.connectivityTests);

        statistics_.incrementMinimalityTests(
            branch.statistics.minimalityTests);

        statistics_.incrementRepositoryQueries(
            branch.statistics.repositoryQueries);

        statistics_.incrementPrunedBranches(
            branch.statistics.prunedBranches);

        statistics_.incrementBranches(
            branch.statistics.branches);
    }

    std::vector<Cut> result;

    for (const auto& cuts : results) {
        result.insert(
            result.end(),
            cuts.begin(),
            cuts.end());
    }

    stopTimer();

    return result;
}