#include "bfs_connectivity.h"

#include <queue>
#include <stdexcept>
#include <vector>

bool BFSConnectivity::isConnected(
    const Graph& graph,
    int source,
    int target
) const {
    if (!graph.validVertex(source) || !graph.validVertex(target)) {
        throw std::out_of_range("invalid source or target vertex");
    }

    return bfs(graph, source, target, nullptr, -1);
}

bool BFSConnectivity::isConnectedAfterRemovingEdge(
    const Graph& graph,
    int source,
    int target,
    int removedEdge
) const {
    if (!graph.validVertex(source) || !graph.validVertex(target)) {
        throw std::out_of_range("invalid source or target vertex");
    }

    if (!graph.validEdge(removedEdge)) {
        throw std::out_of_range("invalid removed edge");
    }

    return bfs(graph, source, target, nullptr, removedEdge);
}

bool BFSConnectivity::isConnectedAfterRemovingEdges(
    const Graph& graph,
    int source,
    int target,
    const Cut& cut
) const {
    if (!graph.validVertex(source) || !graph.validVertex(target)) {
        throw std::out_of_range("invalid source or target vertex");
    }

    if (cut.edgeCount() != graph.edgeCount()) {
        throw std::invalid_argument(
            "cut and graph must use the same edge universe"
        );
    }

    return bfs(graph, source, target, &cut, -1);
}

bool BFSConnectivity::isCut(
    const Graph& graph,
    int source,
    int target,
    const Cut& cut
) const {
    return !isConnectedAfterRemovingEdges(
        graph,
        source,
        target,
        cut
    );
}

bool BFSConnectivity::isMinimalCut(
    const Graph& graph,
    int source,
    int target,
    const Cut& cut
) const {
    if (!isCut(graph, source, target, cut)) {
        return false;
    }

    for (int edgeId : cut.edges()) {
        if (!isConnectedAfterRemovingEdgesExcept(
                graph,
                source,
                target,
                cut,
                edgeId
            )) {
            return false;
        }
    }

    return true;
}

bool BFSConnectivity::bfs(
    const Graph& graph,
    int source,
    int target,
    const Cut* removedCut,
    int removedEdge
) const {
    if (source == target) {
        return true;
    }

    const int vertexCount = graph.vertexCount();

    std::vector<unsigned char> visited(vertexCount, 0);
    std::queue<int> queue;

    visited[source] = 1;
    queue.push(source);

    while (!queue.empty()) {
        const int current = queue.front();
        queue.pop();

        for (int neighbor : graph.neighbors(current)) {
            int edgeId = -1;

            for (int candidateEdgeId = 0;
                 candidateEdgeId < graph.edgeCount();
                 ++candidateEdgeId) {
                const auto [u, v] = graph.edge(candidateEdgeId);

                if ((u == current && v == neighbor) ||
                    (u == neighbor && v == current)) {
                    edgeId = candidateEdgeId;
                    break;
                }
            }

            if (edgeId == -1) {
                throw std::logic_error(
                    "graph adjacency contains an unknown edge"
                );
            }

            if (edgeId == removedEdge) {
                continue;
            }

            if (removedCut != nullptr &&
                removedCut->contains(edgeId)) {
                continue;
            }

            if (!visited[neighbor]) {
                if (neighbor == target) {
                    return true;
                }

                visited[neighbor] = 1;
                queue.push(neighbor);
            }
        }
    }

    return false;
}

bool BFSConnectivity::isConnectedAfterRemovingEdgesExcept(
    const Graph& graph,
    int source,
    int target,
    const Cut& cut,
    int exceptEdge
) const {
    Cut reducedCut = cut;
    reducedCut.removeEdge(exceptEdge);

    return isConnectedAfterRemovingEdges(
        graph,
        source,
        target,
        reducedCut
    );
}