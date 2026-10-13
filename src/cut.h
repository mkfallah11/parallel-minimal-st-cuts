#ifndef MINIMAL_CUT_CUT_H
#define MINIMAL_CUT_CUT_H

#include <cstddef>
#include <string>
#include <vector>

class Graph;

class Cut {
public:
    Cut();
    explicit Cut(int edge_count);

    void clear();
    void resize(int edge_count);

    void addEdge(int edge_id);
    void removeEdge(int edge_id);

    bool contains(int edge_id) const;

    bool empty() const;

    std::size_t size() const;

    int edgeCount() const;

    const std::vector<int>& edges() const;

    std::vector<int> edgeList() const;

    bool isSubsetOf(const Cut& other) const;

    bool containsSubset(const Cut& candidate) const;

    bool operator==(const Cut& other) const;
    bool operator!=(const Cut& other) const;

    std::string toString() const;

private:
    int edge_count_;
    std::vector<unsigned char> selected_;
    std::vector<int> edges_;

    void validateEdgeId(int edge_id) const;
};

#endif