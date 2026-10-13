#include "work_queue_solver.h"

#include <algorithm>
#include <omp.h>
#include <vector>


WorkQueueSolver::WorkQueueSolver(
    const Graph& graph,
    const Configuration& configuration
)
    : ParallelSolver(
          graph,
          configuration
      )
{
}


std::vector<Cut> WorkQueueSolver::solve()
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
            threadCount * 4,
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


    std::deque<WorkItem> queue;


    for (std::size_t i = 0;
         i < branches.size();
         ++i) {

        queue.push_back(
            {i}
        );
    }


    std::mutex queueMutex;


    /*
     * each OpenMP thread owns one result vector.
     * this avoids concurrent writes to the same vector.
     */
    std::vector<std::vector<Cut>> localResults(
        static_cast<std::size_t>(
            threadCount
        )
    );


#pragma omp parallel num_threads(threadCount)
    {
        const int threadId =
            omp_get_thread_num();


        WorkItem item;


        while (
            popWork(
                queue,
                item,
                queueMutex
            )
        ) {

            const std::vector<Cut> cuts =
                executeBranch(
                    branches[
                        item.branchIndex
                    ]
                );


            std::vector<Cut>& local =
                localResults[
                    static_cast<std::size_t>(
                        threadId
                    )
                ];


            local.insert(
                local.end(),
                cuts.begin(),
                cuts.end()
            );
        }
    }


    /*
     * aggregate branch statistics after
     * all workers have completed.
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
     * collect all cuts from thread-local
     * result vectors.
     */
    std::size_t totalCuts = 0;


    for (const auto& cuts : localResults) {
        totalCuts += cuts.size();
    }


    std::vector<Cut> result;


    result.reserve(
        totalCuts
    );


    for (const auto& cuts : localResults) {

        result.insert(
            result.end(),
            cuts.begin(),
            cuts.end()
        );
    }


    /*
     * the actual result size is the
     * authoritative minimal-cut count.
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


bool WorkQueueSolver::popWork(
    std::deque<WorkItem>& queue,
    WorkItem& item,
    std::mutex& mutex
) const
{
    std::lock_guard<std::mutex> lock(
        mutex
    );


    if (queue.empty()) {
        return false;
    }


    item =
        queue.front();


    queue.pop_front();


    return true;
}