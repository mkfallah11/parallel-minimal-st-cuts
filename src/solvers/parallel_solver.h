#ifndef PARALLEL_SOLVER_H
#define PARALLEL_SOLVER_H


#include "../algorithm2.h"
#include "../graph.h"
#include "../cut.h"
#include "../utils/configuration.h"
#include "../utils/statistics.h"


#include <chrono>
#include <cstddef>
#include <vector>


class ParallelSolver {
public:
    ParallelSolver(
        const Graph& graph,
        const Configuration& configuration
    );


    virtual ~ParallelSolver() = default;


    virtual std::vector<Cut> solve() = 0;


    const Statistics& getStatistics() const;


protected:
    const Graph& graph_;
    Configuration configuration_;
    Statistics statistics_;


    Algorithm2 algorithm_;


    std::chrono::steady_clock::time_point startTime_;


    std::vector<Algorithm2Branch> createBranches(
        int branchCount
    ) const;


    std::vector<Cut> executeBranch(
        Algorithm2Branch& branch
    ) const;


    void resetStatistics();


    void startTimer();


    void stopTimer();


    void updateStatistics(
        std::size_t cutCount
    );
};


#endif