#ifndef ALGORITHM2_H
#define ALGORITHM2_H

#include "graph.h"
#include "cut.h"

#include <cstddef>
#include <vector>


struct BranchStatistics {
    std::size_t candidates = 0;
    std::size_t minimalCuts = 0;
    std::size_t connectivityTests = 0;
    std::size_t minimalityTests = 0;
    std::size_t repositoryQueries = 0;
    std::size_t prunedBranches = 0;
    std::size_t branches = 0;

    void reset() {
        candidates = 0;
        minimalCuts = 0;
        connectivityTests = 0;
        minimalityTests = 0;
        repositoryQueries = 0;
        prunedBranches = 0;
        branches = 0;
    }
};


struct Algorithm2Branch {
    std::size_t beginMask = 0;
    std::size_t endMask = 0;

    BranchStatistics statistics;
};


class Algorithm2 {
public:
    Algorithm2(
        const Graph& graph
    );

    Algorithm2(
        const Graph& graph,
        int source,
        int target
    );

    void run();

    const std::vector<Cut>& cuts() const;

    std::size_t cutCount() const;

    std::vector<Algorithm2Branch> createBranches(
        int requestedBranches
    ) const;

    std::vector<Algorithm2Branch> splitBranches(
        int requestedBranches
    ) const;

    std::vector<Cut> continueFromBranch(
        Algorithm2Branch& branch
    ) const;

    std::vector<Cut> enumerateRange(
        std::size_t beginMask,
        std::size_t endMask,
        BranchStatistics* statistics = nullptr
    ) const;

private:
    const Graph& graph_;

    int source_;

    int target_;

    std::vector<Cut> cuts_;

    bool isConnectedSide(
        const std::vector<unsigned char>& side,
        int startVertex,
        BranchStatistics* statistics = nullptr
    ) const;

    bool isMinimalSTCut(
        const std::vector<unsigned char>& sourceSide,
        BranchStatistics* statistics = nullptr
    ) const;

    Cut buildCut(
        const std::vector<unsigned char>& sourceSide
    ) const;

    void enumerateAllCuts();

    void enumerateSourceSides(
        int vertexIndex,
        std::vector<unsigned char>& sourceSide
    );

    bool containsCut(
        const std::vector<Cut>& cuts,
        const Cut& candidate
    ) const;

    void initializeBranch(
        Algorithm2Branch& branch
    ) const;
};

#endif