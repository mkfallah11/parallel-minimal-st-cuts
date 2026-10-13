#include "graph.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

Graph::Graph()
    : vertex_count_(0) {
}

Graph::Graph(int vertex_count)
    : vertex_count_(0) {
    setVertexCount(vertex_count);
}

void Graph::clear() {
    vertex_count_ = 0;
    edges_.clear();
    adjacency_.clear();
}

void Graph::setVertexCount(int vertex_count) {
    if (vertex_count < 0) {
        throw std::invalid_argument("vertex count cannot be negative");
    }

    vertex_count_ = vertex_count;
    adjacency_.assign(vertex_count_, {});
    edges_.clear();
}

void Graph::addEdge(int u, int v) {
    ensureVertex(u);
    ensureVertex(v);

    if (u == v) {
        throw std::invalid_argument("self-loops are not supported");
    }

    edges_.emplace_back(u, v);

    adjacency_[u].push_back(v);
    adjacency_[v].push_back(u);
}

bool Graph::loadFromFile(const std::string& filename) {
    std::ifstream input(filename);

    if (!input.is_open()) {
        return false;
    }

    std::string line;
    int vertex_count;
    int edge_count;

    // read the first non-comment line
    while (std::getline(input, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::istringstream iss(line);
        if (!(iss >> vertex_count >> edge_count)) {
            return false;
        }

        break;
    }

    if (vertex_count < 0 || edge_count < 0) {
        return false;
    }

    clear();
    setVertexCount(vertex_count);

    int edges_read = 0;

    while (edges_read < edge_count && std::getline(input, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::istringstream iss(line);

        int u;
        int v;

        if (!(iss >> u >> v)) {
            clear();
            return false;
        }

        if (!validVertex(u) || !validVertex(v) || u == v) {
            clear();
            return false;
        }

        addEdge(u, v);
        ++edges_read;
    }

    if (edges_read != edge_count) {
        clear();
        return false;
    }

    return true;
}

int Graph::vertexCount() const {
    return vertex_count_;
}

int Graph::edgeCount() const {
    return static_cast<int>(edges_.size());
}

const std::vector<int>& Graph::neighbors(int vertex) const {
    ensureVertex(vertex);
    return adjacency_[vertex];
}

const std::vector<std::pair<int, int>>& Graph::edges() const {
    return edges_;
}

std::pair<int, int> Graph::edge(int edge_id) const {
    if (!validEdge(edge_id)) {
        throw std::out_of_range("invalid edge id");
    }

    return edges_[edge_id];
}

bool Graph::hasEdge(int u, int v) const {
    ensureVertex(u);
    ensureVertex(v);

    for (int neighbor : adjacency_[u]) {
        if (neighbor == v) {
            return true;
        }
    }

    return false;
}

bool Graph::validVertex(int vertex) const {
    return vertex >= 0 && vertex < vertex_count_;
}

bool Graph::validEdge(int edge_id) const {
    return edge_id >= 0 && edge_id < static_cast<int>(edges_.size());
}

void Graph::ensureVertex(int vertex) const {
    if (!validVertex(vertex)) {
        throw std::out_of_range("invalid vertex id");
    }
}