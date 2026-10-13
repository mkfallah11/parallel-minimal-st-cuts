#include "dynamic_solver.h"

#include <algorithm>
#include <omp.h>
#include <vector>


DynamicSolver::DynamicSolver(
    const Graph& graph,
    const Configuration& configuration
)
    : ParallelSolver(
          graph,
          configuration
      )
{
}


std::vector<Cut> DynamicSolver::solve()
{
    resetStatistics();

    startTimer();


    const int threadCount =
        std::max(
            1,
            configuration_.getNumThreads()
        );


    const int branchCount =
        std::max(
            threadCount * 2,
            threadCount
        );


    std::vector<Algorithm2Branch> branches =
        createBranches(
            branchCount
        );


    if (branches.empty()) {
        stopTimer();
        return {};
    }


    /*
     * each branch owns an independent result vector.
     * this avoids concurrent writes to the same vector.
     */
    std::vector<std::vector<Cut>> branchResults(
        branches.size()
    );


#pragma omp parallel for schedule(dynamic) num_threads(threadCount)
    for (int i = 0;
         i < static_cast<int>(branches.size());
         ++i) {

        branchResults[
            static_cast<std::size_t>(i)
        ] =
            executeBranch(
                branches[
                    static_cast<std::size_t>(i)
                ]
            );
    }


    /*
     * aggregate branch statistics after all
     * parallel workers have completed.
     */
    std::size_t candidates = 0;

    std::size_t minimalCuts = 0;

    std::size_t connectivityTests = 0;

    std::size_t minimalityTests = 0;

    std::size_t repositoryQueries = 0;

    std::size_t prunedBranches = 0;

    std::size_t executedBranches = 0;


    for (const auto& branch : branches) {

        candidates +=
            branch.statistics.candidates;

        minimalCuts +=
            branch.statistics.minimalCuts;

        connectivityTests +=
            branch.statistics.connectivityTests;

        minimalityTests +=
            branch.statistics.minimalityTests;

        repositoryQueries +=
            branch.statistics.repositoryQueries;

        prunedBranches +=
            branch.statistics.prunedBranches;

        executedBranches +=
            branch.statistics.branches;
    }


    statistics_.incrementCandidates(
        candidates
    );


    statistics_.incrementMinimalCuts(
        minimalCuts
    );


    statistics_.incrementConnectivityTests(
        connectivityTests
    );


    statistics_.incrementMinimalityTests(
        minimalityTests
    );


    statistics_.incrementRepositoryQueries(
        repositoryQueries
    );


    statistics_.incrementPrunedBranches(
        prunedBranches
    );


    statistics_.incrementBranches(
        executedBranches
    );


    /*
     * collect all cuts.
     */
    std::size_t totalCuts = 0;


    for (const auto& cuts : branchResults) {
        totalCuts += cuts.size();
    }


    std::vector<Cut> result;

    result.reserve(
        totalCuts
    );


    for (const auto& cuts : branchResults) {

        result.insert(
            result.end(),
            cuts.begin(),
            cuts.end()
        );
    }


    /*
     * the actual result is the authoritative
     * minimal-cut count.
     */
    if (minimalCuts != result.size()) {

        statistics_.reset();


        statistics_.incrementCandidates(
            candidates
        );


        statistics_.incrementMinimalCuts(
            result.size()
        );


        statistics_.incrementConnectivityTests(
            connectivityTests
        );


        statistics_.incrementMinimalityTests(
            minimalityTests
        );


        statistics_.incrementRepositoryQueries(
            repositoryQueries
        );


        statistics_.incrementPrunedBranches(
            prunedBranches
        );


        statistics_.incrementBranches(
            executedBranches
        );
    }


    stopTimer();


    return result;
}