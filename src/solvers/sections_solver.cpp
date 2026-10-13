#include "sections_solver.h"

#include <algorithm>
#include <omp.h>
#include <vector>

SectionsSolver::SectionsSolver(
    const Graph& graph,
    const Configuration& configuration)
    : ParallelSolver(
          graph,
          configuration) 
{
}


std::vector<Cut> SectionsSolver::solve() {
    resetStatistics();

    startTimer();


    const int threadCount =
        std::max(
            1,
            configuration_.getNumThreads());


    const int sectionCount =
        std::min(
            threadCount,
            16);

    const int branchCount =
        std::max(
            sectionCount,
            threadCount * 2);

    std::vector<Algorithm2Branch> branches =
        createBranches(branchCount);


    if (branches.empty()) {
        stopTimer();
        return {};
    }


    std::vector<std::vector<Cut>> sectionResults(
        static_cast<std::size_t>(sectionCount));

#pragma omp parallel sections num_threads(sectionCount)
    {
        #pragma omp section
        {
            for (std::size_t i = 0;
                 i < branches.size();
                 ++i) {

                if (i %
                    static_cast<std::size_t>(
                        sectionCount) != 0) {
                    continue;
                }


                const std::vector<Cut> cuts =
                    executeBranch(
                        branches[i]);


                auto& local =
                    sectionResults[0];


                local.insert(
                    local.end(),
                    cuts.begin(),
                    cuts.end());
            }
        }


        #pragma omp section
        {
            if (sectionCount > 1) {
                for (std::size_t i = 0;
                     i < branches.size();
                     ++i) {

                    if (i %
                        static_cast<std::size_t>(
                            sectionCount) != 1) {
                        continue;
                    }


                    const std::vector<Cut> cuts =
                        executeBranch(
                            branches[i]);


                    auto& local =
                        sectionResults[1];


                    local.insert(
                        local.end(),
                        cuts.begin(),
                        cuts.end());
                }
            }
        }


        #pragma omp section
        {
            if (sectionCount > 2) {
                for (std::size_t i = 0;
                     i < branches.size();
                     ++i) {

                    if (i %
                        static_cast<std::size_t>(
                            sectionCount) != 2) {
                        continue;
                    }


                    const std::vector<Cut> cuts =
                        executeBranch(
                            branches[i]);


                    auto& local =
                        sectionResults[2];


                    local.insert(
                        local.end(),
                        cuts.begin(),
                        cuts.end());
                }
            }
        }


        #pragma omp section
        {
            if (sectionCount > 3) {
                for (std::size_t i = 0;
                     i < branches.size();
                     ++i) {

                    if (i %
                        static_cast<std::size_t>(
                            sectionCount) != 3) {
                        continue;
                    }


                    const std::vector<Cut> cuts =
                        executeBranch(
                            branches[i]);


                    auto& local =
                        sectionResults[3];


                    local.insert(
                        local.end(),
                        cuts.begin(),
                        cuts.end());
                }
            }
        }


        #pragma omp section
        {
            if (sectionCount > 4) {
                for (std::size_t i = 0;
                     i < branches.size();
                     ++i) {

                    if (i %
                        static_cast<std::size_t>(
                            sectionCount) != 4) {
                        continue;
                    }


                    const std::vector<Cut> cuts =
                        executeBranch(
                            branches[i]);


                    auto& local =
                        sectionResults[4];


                    local.insert(
                        local.end(),
                        cuts.begin(),
                        cuts.end());
                }
            }
        }

        
        #pragma omp section
        {
            if (sectionCount > 5) {
                for (std::size_t i = 0;
                     i < branches.size();
                     ++i) {

                    if (i %
                        static_cast<std::size_t>(
                            sectionCount) != 5) {
                        continue;
                    }


                    const std::vector<Cut> cuts =
                        executeBranch(
                            branches[i]);


                    auto& local =
                        sectionResults[5];


                    local.insert(
                        local.end(),
                        cuts.begin(),
                        cuts.end());
                }
            }
        }


        #pragma omp section
        {
            if (sectionCount > 6) {
                for (std::size_t i = 0;
                     i < branches.size();
                     ++i) {

                    if (i %
                        static_cast<std::size_t>(
                            sectionCount) != 6) {
                        continue;
                    }


                    const std::vector<Cut> cuts =
                        executeBranch(
                            branches[i]);


                    auto& local =
                        sectionResults[6];


                    local.insert(
                        local.end(),
                        cuts.begin(),
                        cuts.end());
                }
            }
        }


        #pragma omp section
        {
            if (sectionCount > 7) {
                for (std::size_t i = 0;
                     i < branches.size();
                     ++i) {

                    if (i %
                        static_cast<std::size_t>(
                            sectionCount) != 7) {
                        continue;
                    }


                    const std::vector<Cut> cuts =
                        executeBranch(
                            branches[i]);


                    auto& local =
                        sectionResults[7];


                    local.insert(
                        local.end(),
                        cuts.begin(),
                        cuts.end());
                }
            }
        }


        #pragma omp section
        {
            if (sectionCount > 8) {
                for (std::size_t i = 0;
                     i < branches.size();
                     ++i) {

                    if (i %
                        static_cast<std::size_t>(
                            sectionCount) != 8) {
                        continue;
                    }


                    const std::vector<Cut> cuts =
                        executeBranch(
                            branches[i]);


                    auto& local =
                        sectionResults[8];


                    local.insert(
                        local.end(),
                        cuts.begin(),
                        cuts.end());
                }
            }
        }


        #pragma omp section
        {
            if (sectionCount > 9) {
                for (std::size_t i = 0;
                     i < branches.size();
                     ++i) {

                    if (i %
                        static_cast<std::size_t>(
                            sectionCount) != 9) {
                        continue;
                    }


                    const std::vector<Cut> cuts =
                        executeBranch(
                            branches[i]);


                    auto& local =
                        sectionResults[9];


                    local.insert(
                        local.end(),
                        cuts.begin(),
                        cuts.end());
                }
            }
        }


        #pragma omp section
        {
            if (sectionCount > 10) {
                for (std::size_t i = 0;
                     i < branches.size();
                     ++i) {

                    if (i %
                        static_cast<std::size_t>(
                            sectionCount) != 10) {
                        continue;
                    }


                    const std::vector<Cut> cuts =
                        executeBranch(
                            branches[i]);


                    auto& local =
                        sectionResults[10];


                    local.insert(
                        local.end(),
                        cuts.begin(),
                        cuts.end());
                }
            }
        }


        #pragma omp section
        {
            if (sectionCount > 11) {
                for (std::size_t i = 0;
                     i < branches.size();
                     ++i) {

                    if (i %
                        static_cast<std::size_t>(
                            sectionCount) != 11) {
                        continue;
                    }


                    const std::vector<Cut> cuts =
                        executeBranch(
                            branches[i]);


                    auto& local =
                        sectionResults[11];


                    local.insert(
                        local.end(),
                        cuts.begin(),
                        cuts.end());
                }
            }
        }


        #pragma omp section
        {
            if (sectionCount > 12) {
                for (std::size_t i = 0;
                     i < branches.size();
                     ++i) {

                    if (i %
                        static_cast<std::size_t>(
                            sectionCount) != 12) {
                        continue;
                    }


                    const std::vector<Cut> cuts =
                        executeBranch(
                            branches[i]);


                    auto& local =
                        sectionResults[12];


                    local.insert(
                        local.end(),
                        cuts.begin(),
                        cuts.end());
                }
            }
        }


        #pragma omp section
        {
            if (sectionCount > 13) {
                for (std::size_t i = 0;
                     i < branches.size();
                     ++i) {

                    if (i %
                        static_cast<std::size_t>(
                            sectionCount) != 13) {
                        continue;
                    }


                    const std::vector<Cut> cuts =
                        executeBranch(
                            branches[i]);


                    auto& local =
                        sectionResults[13];


                    local.insert(
                        local.end(),
                        cuts.begin(),
                        cuts.end());
                }
            }
        }


        #pragma omp section
        {
            if (sectionCount > 14) {
                for (std::size_t i = 0;
                     i < branches.size();
                     ++i) {

                    if (i %
                        static_cast<std::size_t>(
                            sectionCount) != 14) {
                        continue;
                    }


                    const std::vector<Cut> cuts =
                        executeBranch(
                            branches[i]);


                    auto& local =
                        sectionResults[14];


                    local.insert(
                        local.end(),
                        cuts.begin(),
                        cuts.end());
                }
            }
        }


        #pragma omp section
        {
            if (sectionCount > 15) {
                for (std::size_t i = 0;
                     i < branches.size();
                     ++i) {

                    if (i %
                        static_cast<std::size_t>(
                            sectionCount) != 15) {
                        continue;
                    }


                    const std::vector<Cut> cuts =
                        executeBranch(
                            branches[i]);


                    auto& local =
                        sectionResults[15];


                    local.insert(
                        local.end(),
                        cuts.begin(),
                        cuts.end());
                }
            }
        }
    }


    /*
     * aggregate branch statistics after all
     * parallel sections have completed
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
        candidates);


    statistics_.incrementMinimalCuts(
        minimalCuts);


    statistics_.incrementConnectivityTests(
        connectivityTests);


    statistics_.incrementMinimalityTests(
        minimalityTests);


    statistics_.incrementRepositoryQueries(
        repositoryQueries);


    statistics_.incrementPrunedBranches(
        prunedBranches);


    statistics_.incrementBranches(
        executedBranches);


    std::vector<Cut> result;


    std::size_t totalCuts = 0;


    for (const auto& cuts : sectionResults) {
        totalCuts += cuts.size();
    }


    result.reserve(totalCuts);


    for (const auto& cuts : sectionResults) {
        result.insert(
            result.end(),
            cuts.begin(),
            cuts.end());
    }


    /*
     * use the actual result size as the final
     * minimal-cut count
     */
    if (minimalCuts != result.size()) {
        statistics_.reset();

        statistics_.incrementCandidates(
            candidates);

        statistics_.incrementMinimalCuts(
            result.size());

        statistics_.incrementConnectivityTests(
            connectivityTests);

        statistics_.incrementMinimalityTests(
            minimalityTests);

        statistics_.incrementRepositoryQueries(
            repositoryQueries);

        statistics_.incrementPrunedBranches(
            prunedBranches);

        statistics_.incrementBranches(
            executedBranches);
    }


    stopTimer();


    return result;
}