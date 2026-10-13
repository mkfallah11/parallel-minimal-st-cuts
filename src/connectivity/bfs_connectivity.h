#ifndef MINIMAL_CUT_BFS_CONNECTIVITY_H
#define MINIMAL_CUT_BFS_CONNECTIVITY_H

#include "connectivity.h"

class BFSConnectivity : public Connectivity {
public:
    BFSConnectivity() = default;
    ~BFSConnectivity() override = default;

    bool isConnected(
        const Graph& graph,
        int source,
        int target
    ) const override;

    bool isConnectedAfterRemovingEdge(
        const Graph& graph,
        int source,
        int target,
        int removedEdge
    ) const override;

    bool isConnectedAfterRemovingEdges(
        const Graph& graph,
        int source,
        int target,
        const Cut& cut
    ) const override;

    bool isCut(
        const Graph& graph,
        int source,
        int target,
        const Cut& cut
    ) const override;

    bool isMinimalCut(
        const Graph& graph,
        int source,
        int target,
        const Cut& cut
    ) const override;

private:
    bool bfs(
        const Graph& graph,
        int source,
        int target,
        const Cut* removedCut,
        int removedEdge
    ) const;

    bool isConnectedAfterRemovingEdgesExcept(
        const Graph& graph,
        int source,
        int target,
        const Cut& cut,
        int exceptEdge
    ) const;
};

#endif