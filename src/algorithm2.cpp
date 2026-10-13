#include "algorithm2.h"

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>


Algorithm2::Algorithm2(
    const Graph& graph
)
    : Algorithm2(
          graph,
          0,
          graph.vertexCount() - 1
      )
{
}


Algorithm2::Algorithm2(
    const Graph& graph,
    int source,
    int target
)
    : graph_(graph),
      source_(source),
      target_(target),
      cuts_()
{
    if (graph_.vertexCount() < 2) {
        throw std::invalid_argument(
            "graph must contain at least two vertices"
        );
    }

    if (!graph_.validVertex(source_) ||
        !graph_.validVertex(target_)) {
        throw std::invalid_argument(
            "invalid source or target vertex"
        );
    }

    if (source_ == target_) {
        throw std::invalid_argument(
            "source and target must be different"
        );
    }
}


void Algorithm2::run()
{
    cuts_.clear();

    enumerateAllCuts();
}


const std::vector<Cut>& Algorithm2::cuts() const
{
    return cuts_;
}


std::size_t Algorithm2::cutCount() const
{
    return cuts_.size();
}


void Algorithm2::enumerateAllCuts()
{
    const int vertexCount =
        graph_.vertexCount();

    std::vector<unsigned char> sourceSide(
        static_cast<std::size_t>(vertexCount),
        0
    );

    sourceSide[source_] = 1;
    sourceSide[target_] = 0;

    enumerateSourceSides(
        0,
        sourceSide
    );
}


void Algorithm2::enumerateSourceSides(
    int vertexIndex,
    std::vector<unsigned char>& sourceSide
)
{
    const int vertexCount =
        graph_.vertexCount();

    if (vertexIndex >= vertexCount) {

        if (sourceSide[source_] == 0 ||
            sourceSide[target_] != 0) {
            return;
        }

        if (!isMinimalSTCut(
                sourceSide,
                nullptr
            )) {
            return;
        }

        const Cut candidate =
            buildCut(sourceSide);

        if (!containsCut(
                cuts_,
                candidate
            )) {
            cuts_.push_back(candidate);
        }

        return;
    }

    if (vertexIndex == source_ ||
        vertexIndex == target_) {

        enumerateSourceSides(
            vertexIndex + 1,
            sourceSide
        );

        return;
    }

    sourceSide[vertexIndex] = 0;

    enumerateSourceSides(
        vertexIndex + 1,
        sourceSide
    );

    sourceSide[vertexIndex] = 1;

    enumerateSourceSides(
        vertexIndex + 1,
        sourceSide
    );

    sourceSide[vertexIndex] = 0;
}


bool Algorithm2::isConnectedSide(
    const std::vector<unsigned char>& side,
    int startVertex,
    BranchStatistics* statistics
) const
{
    if (statistics != nullptr) {
        ++statistics->connectivityTests;
    }

    if (!graph_.validVertex(startVertex)) {
        return false;
    }

    if (!side[startVertex]) {
        return false;
    }

    std::vector<unsigned char> visited(
        static_cast<std::size_t>(
            graph_.vertexCount()
        ),
        0
    );

    std::vector<int> queue;

    queue.reserve(
        static_cast<std::size_t>(
            graph_.vertexCount()
        )
    );

    queue.push_back(startVertex);

    visited[startVertex] = 1;

    std::size_t position = 0;

    int visitedCount = 0;

    int expectedCount = 0;

    for (int vertex = 0;
         vertex < graph_.vertexCount();
         ++vertex) {

        if (side[vertex]) {
            ++expectedCount;
        }
    }

    while (position < queue.size()) {

        const int current =
            queue[position++];

        ++visitedCount;

        for (int neighbor :
             graph_.neighbors(current)) {

            if (!side[neighbor]) {
                continue;
            }

            if (visited[neighbor]) {
                continue;
            }

            visited[neighbor] = 1;

            queue.push_back(neighbor);
        }
    }

    return visitedCount == expectedCount;
}


Cut Algorithm2::buildCut(
    const std::vector<unsigned char>& sourceSide
) const
{
    Cut cut(
        graph_.edgeCount()
    );

    for (int edgeId = 0;
         edgeId < graph_.edgeCount();
         ++edgeId) {

        const auto [u, v] =
            graph_.edge(edgeId);

        if (sourceSide[u] !=
            sourceSide[v]) {

            cut.addEdge(edgeId);
        }
    }

    return cut;
}


bool Algorithm2::isMinimalSTCut(
    const std::vector<unsigned char>& sourceSide,
    BranchStatistics* statistics
) const
{
    if (statistics != nullptr) {
        ++statistics->minimalityTests;
    }

    if (!sourceSide[source_]) {
        return false;
    }

    if (sourceSide[target_]) {
        return false;
    }

    if (!isConnectedSide(
            sourceSide,
            source_,
            statistics
        )) {
        return false;
    }

    std::vector<unsigned char> targetSide(
        static_cast<std::size_t>(
            graph_.vertexCount()
        ),
        0
    );

    for (int vertex = 0;
         vertex < graph_.vertexCount();
         ++vertex) {

        if (!sourceSide[vertex]) {
            targetSide[vertex] = 1;
        }
    }

    if (!isConnectedSide(
            targetSide,
            target_,
            statistics
        )) {
        return false;
    }

    return true;
}


bool Algorithm2::containsCut(
    const std::vector<Cut>& cuts,
    const Cut& candidate
) const
{
    const std::string candidateString =
        candidate.toString();

    for (const Cut& cut : cuts) {

        if (cut.toString() ==
            candidateString) {

            return true;
        }
    }

    return false;
}


std::vector<Cut> Algorithm2::enumerateRange(
    std::size_t beginMask,
    std::size_t endMask,
    BranchStatistics* statistics
) const
{
    std::vector<Cut> result;

    const int vertexCount =
        graph_.vertexCount();

    std::vector<int> freeVertices;

    freeVertices.reserve(
        static_cast<std::size_t>(
            std::max(
                0,
                vertexCount - 2
            )
        )
    );

    for (int vertex = 0;
         vertex < vertexCount;
         ++vertex) {

        if (vertex == source_ ||
            vertex == target_) {
            continue;
        }

        freeVertices.push_back(vertex);
    }

    const std::size_t freeVertexCount =
        freeVertices.size();

    if (freeVertexCount >=
        sizeof(std::size_t) * 8) {

        throw std::overflow_error(
            "graph is too large for exhaustive enumeration"
        );
    }

    const std::size_t combinations =
        static_cast<std::size_t>(1)
        << freeVertexCount;

    if (beginMask >= combinations) {
        return result;
    }

    endMask =
        std::min(
            endMask,
            combinations
        );

    if (beginMask >= endMask) {
        return result;
    }

    std::vector<unsigned char> sourceSide(
        static_cast<std::size_t>(
            vertexCount
        ),
        0
    );

    /*result.reserve(
        endMask - beginMask
    );*/

    for (std::size_t mask = beginMask;
         mask < endMask;
         ++mask) {

        if (statistics != nullptr) {
            ++statistics->candidates;
        }

        std::fill(
            sourceSide.begin(),
            sourceSide.end(),
            0
        );

        sourceSide[source_] = 1;

        sourceSide[target_] = 0;

        for (std::size_t i = 0;
             i < freeVertices.size();
             ++i) {

            if (mask &
                (static_cast<std::size_t>(1) << i)) {

                sourceSide[
                    freeVertices[i]
                ] = 1;
            }
        }

        if (!isMinimalSTCut(
                sourceSide,
                statistics
            )) {
            continue;
        }

        result.push_back(
            buildCut(sourceSide)
        );

        if (statistics != nullptr) {
            ++statistics->minimalCuts;
        }
    }

    return result;
}


void Algorithm2::initializeBranch(
    Algorithm2Branch& branch
) const
{
    branch.beginMask = 0;
    branch.endMask = 0;
    branch.statistics.reset();
}


std::vector<Algorithm2Branch>
Algorithm2::splitBranches(
    int requestedBranches
) const
{
    std::vector<Algorithm2Branch> branches;

    if (requestedBranches < 1) {
        requestedBranches = 1;
    }

    const int vertexCount =
        graph_.vertexCount();

    const int freeVertexCount =
        vertexCount - 2;

    if (freeVertexCount < 0) {
        return branches;
    }

    if (freeVertexCount >=
        static_cast<int>(
            sizeof(std::size_t) * 8
        )) {

        throw std::overflow_error(
            "graph is too large for exhaustive branch enumeration"
        );
    }

    const std::size_t totalAssignments =
        static_cast<std::size_t>(1)
        << freeVertexCount;

    const std::size_t branchCount =
        std::min(
            static_cast<std::size_t>(
                requestedBranches
            ),
            std::max(
                static_cast<std::size_t>(1),
                totalAssignments
            )
        );

    const std::size_t chunk =
        (
            totalAssignments +
            branchCount -
            1
        ) / branchCount;

    branches.reserve(
        branchCount
    );

    for (std::size_t i = 0;
         i < branchCount;
         ++i) {

        const std::size_t begin =
            i * chunk;

        const std::size_t end =
            std::min(
                totalAssignments,
                begin + chunk
            );

        if (begin >= end) {
            continue;
        }

        Algorithm2Branch branch;

        initializeBranch(branch);

        branch.beginMask = begin;
        branch.endMask = end;

        branch.statistics.branches = 1;

        branches.push_back(
            std::move(branch)
        );
    }

    return branches;
}


std::vector<Algorithm2Branch>
Algorithm2::createBranches(
    int requestedBranches
) const
{
    return splitBranches(
        requestedBranches
    );
}


std::vector<Cut>
Algorithm2::continueFromBranch(
    Algorithm2Branch& branch
) const
{
    branch.statistics.reset();

    branch.statistics.branches = 1;

    return enumerateRange(
        branch.beginMask,
        branch.endMask,
        &branch.statistics
    );
}