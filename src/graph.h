#ifndef MINIMAL_CUT_GRAPH_H
#define MINIMAL_CUT_GRAPH_H

#include <string>
#include <utility>
#include <vector>

class Graph {
public:
    Graph();
    explicit Graph(int vertex_count);

    void clear();

    void setVertexCount(int vertex_count);
    void addEdge(int u, int v);

    bool loadFromFile(const std::string& filename);

    int vertexCount() const;
    int edgeCount() const;

    const std::vector<int>& neighbors(int vertex) const;
    const std::vector<std::pair<int, int>>& edges() const;

    std::pair<int, int> edge(int edge_id) const;

    bool hasEdge(int u, int v) const;

    bool validVertex(int vertex) const;
    bool validEdge(int edge_id) const;

private:
    int vertex_count_;
    std::vector<std::pair<int, int>> edges_;
    std::vector<std::vector<int>> adjacency_;

    void ensureVertex(int vertex) const;
};

#endif