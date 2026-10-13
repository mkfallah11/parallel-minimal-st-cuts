#ifndef MINIMAL_CUT_CONNECTIVITY_H
#define MINIMAL_CUT_CONNECTIVITY_H

#include "../cut.h"
#include "../graph.h"

class Connectivity {
public:
    virtual ~Connectivity() = default;

    virtual bool isConnected(const Graph& graph, int source, int target) const = 0;

    virtual bool isConnectedAfterRemovingEdge(
        const Graph& graph,
        int source,
        int target,
        int removedEdge
    ) const = 0;

    virtual bool isConnectedAfterRemovingEdges(
        const Graph& graph,
        int source,
        int target,
        const Cut& cut
    ) const = 0;

    virtual bool isCut(
        const Graph& graph,
        int source,
        int target,
        const Cut& cut
    ) const = 0;

    virtual bool isMinimalCut(
        const Graph& graph,
        int source,
        int target,
        const Cut& cut
    ) const = 0;
};

#endif